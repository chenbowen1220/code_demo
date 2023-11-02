/*
 * INCLUDE FILES
 **************************************************************************************************
 */
/* hal includes */

/* reg includes */

/* other includes */
#include "co_dsp_lib.h"

/* self includes */
#include "hal_lm.h"

/*
 * MACRO DEFINES
 **************************************************************************************************
 */
#define DLM_BASE_ADDR                       (0x0)                   // DLM 起始地址

#define FIFO_PROC_LEN_CALC(int_num)         (int_num << 2)          // 计算 FIFO 处理长度
#define FIFO_TOTAL_LEN_CALC(int_num)        ((int_num << 2) << 1)   // 计算 FIFO 总长度

#define FIFO_ID_IS_IN(id)                   ((id) <= HAL_LM_FIFO_IN_END)
#define FIFO_ID_IS_OUT(id)                  ((id) >= HAL_LM_FIFO_OUT_START)
#define FIFO_ID_IS_TDM(id)                  (((id) == HAL_LM_FIFO_TDM_I) || ((id) == HAL_LM_FIFO_TDM_O))

#define TDM_TRACK_NUM_IS_VALID(num)         (((num) <= HAL_LM_FIFO_TDM_TRACK_NUM_MAX) && ((num) > 0))
#define TDM_TRACK_SEL_IS_VALID(start, end)  ((((start) < HAL_LM_FIFO_TDM_TRACK_NUM_MAX) && ((start) >= 0)) && \
                                            (((end) < HAL_LM_FIFO_TDM_TRACK_NUM_MAX) && ((end) >= 0)) && \
                                            ((end) >= (start)))

#define FIFO_GROUP_NUM_MAX                  (8)

/*
 * ENUM DEFINES
 **************************************************************************************************
 */

/*
 * STRUCTURE DEFINES
 **************************************************************************************************
 */
typedef struct _lm_fifo_handle {
    void *p_v_nxt;          /* next fifo handle */
    void *p_v_env;          /* 上层模块指针 */
    void *p_v_callback;     /* fifo 中断回调 */
    int32_t  s32_value;     /* fifo init value, set by hal_lm_fifo_enable */
    uint16_t u16_ptr;       /* fifo 当前读写指针，输入 fifo 为读指针，输出 fifo 为写指针 */
    uint8_t u8_int_type;    /* fifo 中断类型 */
    uint8_t u8_mem_self;    /* fifo 内部开辟的空间 */
    uint8_t u8_id;          /* fifo id */
} lm_fifo_handle_t;

typedef struct _lm_fifo_group_proc {
    void *p_v_pre;          /* pre group handle */
    void *p_v_nxt;          /* next group handle */
    void *p_v_callback;     /* group int callback */
    void *p_v_handle;       /* one fifo handle belong to this group */
    uint32_t u32_fifo_bit;  /* which fifo in current group, each bit for each fifo */
    uint8_t u8_st;          /* 0:free, 1:sel, 2:int set, 3:work */
    uint8_t u8_id;          /* group id */
} fifo_group_handle_t;

/*
 * LOCAL FUNCTION DECLARATIONS
 **************************************************************************************************
 */

/*
 * GLOBAL VARIABLE DEFINITIONS
 **************************************************************************************************
 */

/*
 * LOCAL VARIABLE DEFINITIONS
 **************************************************************************************************
 */
lm_fifo_handle_t *s_p_st_first_fifo = NULL;
fifo_group_handle_t *s_p_st_first_group_i = NULL;
fifo_group_handle_t *s_p_st_first_group_o = NULL;
volatile uint16_t s_v_u16_group_st; /* bit7:0 for group in, bit15:8 for group out*/

/*
 * LOCAL FUNCTION DEFINITIONS
 **************************************************************************************************
 */
void hal_lm_fifo_single_irq_handler(void)
{
    uint32_t u32_int_en = 0; /* TODO: get fifo int en from register */
    uint32_t u32_int_st = 0; /* TODO: get fifo int st from register */

    for (int i = 0; i < HAL_LM_FIFO_NUM; i++) {
        /* 依次判断使能的 FIFO 是否产生中断 */
        if (((u32_int_st >> i) & 0x1) && ((u32_int_en >> i) & 0x1)) {
            /* TODO: clr fifo int by fifo id i */
            ;

            lm_fifo_handle_t *p_st_handle = s_p_st_first_fifo;

            for (;;) {
                if (!p_st_handle) {
                    return;
                }

                if (p_st_handle->u8_id == i) {
                    break;
                }

                p_st_handle = p_st_handle->p_v_nxt;
            }

            if (p_st_handle && p_st_handle->p_v_callback) {
                hal_callback *cb = (hal_callback *)p_st_handle->p_v_callback;
                cb(p_st_handle->p_v_env);
            }
        }
    }
}

void hal_lm_fifo_group_irq_handler(void)
{
    uint32_t u32_int_st = 0;        /* TODO: get fifo int st from register */
    uint8_t u8_group_int_st_i = 0;  /* TODO: get group int st from register */
    uint8_t u8_group_int_st_o = 0;  /* TODO: get group int st from register */
    fifo_group_handle_t *p_st_cur_group;

    if (u8_group_int_st_o) {
        /* out group 产生中断 */
        p_st_cur_group = s_p_st_first_group_o;  /* 获取第一个 out group handle */

        for (int i = 0; i < FIFO_GROUP_NUM_MAX; i++) {  /* 遍历每一 bit 判断是否产生中断*/
            if ((u8_group_int_st_o >> i) & 0x1) {
                /* 中断来自当前 group_id i */
                for (;;) {                              /* 遍历注册的 group 找到对应的 handle */
                    if (!p_st_cur_group) {
                        /* 理论上不会走到这里 */
                        return;
                    }

                    if (p_st_cur_group->u8_id == i) {
                        /* 找到触发中断的 group_id 对应的 handle */
                        break;
                    }

                    p_st_cur_group = p_st_cur_group->p_v_nxt;   /* 获取下一个 group 的 handle */
                }

                if (p_st_cur_group->u32_fifo_bit) {
                    /* 该 group 有 fifo 存在 */
                    if (u32_int_st & p_st_cur_group->u32_fifo_bit) {
                        /* 有中断状态的 fifo 和 group 中包含的 fifo 一致 */
                        ;   /* TODO: 清除该 group 中所有 fifo 对应的中断状态 */
                        ;   /* TODO：清除该 group 中断 out */

                        if (p_st_cur_group->p_v_callback) {
                            /* 该 group 的callback 存在 */
                            hal_callback *callback = (hal_callback *)p_st_cur_group->p_v_callback;
                            lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_st_cur_group->p_v_handle;
                            callback(p_st_handle->p_v_env); /* 执行 callback */
                        }
                    } else {
                        ;   /* TODO：清除该 group 中断 out */
                    }
                }
            }
        }
    }

    if (u8_group_int_st_i) {
        /* in group 产生中断 */
        p_st_cur_group = s_p_st_first_group_i;  /* 获取第一个 in group handle */

        for (int i = 0; i < FIFO_GROUP_NUM_MAX; i++) {  /* 遍历每一 bit 判断是否产生中断*/
            if ((u8_group_int_st_i >> i) & 0x1) {
                /* 中断来自当前 group_id i */
                for (;;) {                              /* 遍历注册的 group 找到对应的 handle */
                    if (!p_st_cur_group) {
                        /* 理论上不会走到这里 */
                        return;
                    }

                    if (p_st_cur_group->u8_id == i) {
                        /* 找到触发中断的 group_id 对应的 handle */
                        break;
                    }

                    p_st_cur_group = p_st_cur_group->p_v_nxt;   /* 获取下一个 group 的 handle */
                }

                if (p_st_cur_group->u32_fifo_bit) {
                    /* 该 group 有 fifo 存在 */
                    if (u32_int_st & p_st_cur_group->u32_fifo_bit) {
                        /* 有中断状态的 fifo 和 group 中包含的 fifo 一致 */
                        ;   /* TODO: 清除该 group 中所有 fifo 对应的中断状态 */
                        ;   /* TODO：清除该 group 中断 in */

                        if (p_st_cur_group->p_v_callback) {
                            /* 该 group 的callback 存在 */
                            hal_callback *callback = (hal_callback *)p_st_cur_group->p_v_callback;
                            lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_st_cur_group->p_v_handle;
                            callback(p_st_handle->p_v_env); /* 执行 callback */
                        }
                    } else {
                        ;   /* TODO：清除该 group 中断 in */
                    }
                }
            }
        }
    }
}

void hal_lm_fifo_config(hal_lm_fifo_cfg_t *p_st_cfg, uint8_t *p_u8_addr)
{
    /* TODO: add fifo register config */
    ;
}

/*
 * EXPORTED FUNCTION DEFINITIONS
 **************************************************************************************************
 */
hal_lm_fifo_err_enum_t hal_lm_fifo_mem_need(uint8_t u8_track_num, uint16_t u16_int_num, uint32_t *p_u32_size)
{
    if (!u8_track_num || (u8_track_num > HAL_LM_FIFO_TDM_TRACK_NUM_MAX)) {
        *p_u32_size = 0;
        return HAL_LM_FIFO_ERR_TRACK_NUM;
    }

    *p_u32_size = ALIGN(sizeof(lm_fifo_handle_t), 4) + FIFO_TOTAL_LEN_CALC(u16_int_num) * u8_track_num;
    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_unused_group_get(uint8_t u8_io_sel, uint8_t *p_u8_group, void *p_v_mem_malloc)
{
    if (!p_u8_group || !p_v_mem_malloc) return HAL_LM_FIFO_ERR_NULL;

    fifo_group_handle_t *p_st_pre_group = NULL;
    fifo_group_handle_t *p_st_cur_group = u8_io_sel ? s_p_st_first_group_o : s_p_st_first_group_i;

    for (;;) {
        if (!p_st_cur_group) {
            hal_mem_malloc *p_v_malloc = (hal_mem_malloc *)p_v_mem_malloc;
            p_st_cur_group = (fifo_group_handle_t *)p_v_malloc(ALIGN(sizeof(fifo_group_handle_t), 4));

            if (!p_st_cur_group) {
                goto ERR;
            }

            if (u8_io_sel && !s_p_st_first_group_o) {
                s_p_st_first_group_o = p_st_cur_group;
            }

            if (!u8_io_sel && !s_p_st_first_group_i) {
                s_p_st_first_group_i = p_st_cur_group;
            }

            p_st_cur_group->u8_id = p_st_pre_group ? (p_st_pre_group->u8_id - 1) : (FIFO_GROUP_NUM_MAX - 1);
            p_st_cur_group->u8_st = 0;
            p_st_cur_group->p_v_nxt = NULL;
            p_st_cur_group->p_v_callback = NULL;
            p_st_cur_group->u32_fifo_bit = 0;
        }

        if (p_st_cur_group->u8_st == 0) {
            p_st_cur_group->u8_st = 1;
            *p_u8_group = p_st_cur_group->u8_id;
            return HAL_LM_FIFO_OK;
        } else if (p_st_cur_group->u8_id == 0) {
            goto ERR;
        }

        if (p_st_pre_group) {
            p_st_pre_group->p_v_nxt = p_st_cur_group;
            p_st_cur_group->p_v_pre = p_st_pre_group;
        }

        p_st_pre_group = p_st_cur_group;
        p_st_cur_group = p_st_cur_group->p_v_nxt;
    }

ERR:
    *p_u8_group = 0xFF;
    return HAL_LM_FIFO_ERR_GROUP;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_used_group_release(uint8_t u8_io_sel, uint8_t u8_group, void *p_v_mem_free)
{
    if (u8_group >= FIFO_GROUP_NUM_MAX) return HAL_LM_FIFO_ERR_GROUP_ID;
    if (!p_v_mem_free) return HAL_LM_FIFO_ERR_NULL;

    fifo_group_handle_t *p_st_cur_group = u8_io_sel ? s_p_st_first_group_o : s_p_st_first_group_i;

    for (;;) {
        if (!p_st_cur_group) {
            return HAL_LM_FIFO_ERR_GROUP_ID;
        }

        if (p_st_cur_group->u8_id == u8_group) {
            if (p_st_cur_group->u8_st == 1) {
                fifo_group_handle_t *p_st_pre_group;
                fifo_group_handle_t *p_st_nxt_group;
                p_st_cur_group->u8_st = 0;
                p_st_pre_group = p_st_cur_group->p_v_pre;
                p_st_nxt_group = p_st_cur_group->p_v_nxt;

                if (p_st_pre_group) {
                    p_st_pre_group->p_v_nxt = p_st_nxt_group;
                }

                p_st_nxt_group->p_v_pre = p_st_pre_group;
                hal_mem_free *p_v_free = (hal_mem_free *)p_v_mem_free;
                p_v_free(p_st_cur_group);
                return HAL_LM_FIFO_OK;
            } else {
                return HAL_LM_FIFO_ERR_GROUP_ST;
            }
        }

        p_st_cur_group = p_st_cur_group->p_v_nxt;
    }
}

hal_lm_fifo_err_enum_t hal_lm_fifo_used_group_en_cfg(uint8_t u8_io_sel, uint8_t u8_group, uint8_t u8_en)
{
    if (u8_group >= FIFO_GROUP_NUM_MAX) return HAL_LM_FIFO_ERR_GROUP_ID;

    fifo_group_handle_t *p_st_cur_group = u8_io_sel ? s_p_st_first_group_o : s_p_st_first_group_i;

    for (;;) {
        if (!p_st_cur_group) {
            return HAL_LM_FIFO_ERR_GROUP_ID;
        }

        if (p_st_cur_group->u8_id == u8_group) {
            if (u8_en) {
                /* enable */
                if (p_st_cur_group->u8_st == 2) {
                    p_st_cur_group->u8_st = 3;
                } else {
                    return HAL_LM_FIFO_ERR_GROUP_ST;
                }

                /* TODO: add register config, seperate in out */
                ;
            } else {
                /* disable */
                if (p_st_cur_group->u8_st == 3) {
                    p_st_cur_group->u8_st = 2;
                } else {
                    return HAL_LM_FIFO_ERR_GROUP_ST;
                }

                /* TODO: add register config, seperate in out */
                ;
            }

            break;
        }

        p_st_cur_group = p_st_cur_group->p_v_nxt;
    }

    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_en_cfg(hal_lm_fifo_en_cfg_t *p_st_en, uint8_t u8_en)
{
    if (!p_st_en) return HAL_LM_FIFO_ERR_NULL;

    uint32_t *p_u32_en = (uint32_t *)p_st_en;
    uint32_t u32_value = 0; /* TODO: get from register */

    if (u8_en) {
        /* enable */
        for (int i = 0; i < HAL_LM_FIFO_NUM; i++) {
            if ((*p_u32_en >> i) & 0x1) {
                u32_value |= CO_BIT(i);
            }
        }
    } else {
        /* disable */
        for (int i = 0; i < HAL_LM_FIFO_NUM; i++) {
            if ((*p_u32_en >> i) & 0x1) {
                u32_value &= ~CO_BIT(i);
            }
        }
    }

    /* TODO: add register set */
    ;
    return HAL_LM_FIFO_OK;
}

void *hal_lm_fifo_init(hal_lm_fifo_cfg_t *p_st_cfg, void *p_v_mem_malloc)
{
    if (!p_st_cfg) return NULL;

    lm_fifo_handle_t *p_st_handle = NULL;
    uint8_t *p_u8_addr;

    if (p_st_cfg->p_u8_addr) {
        /* dont need to malloc */
        p_u8_addr = p_st_cfg->p_u8_addr;
        p_st_handle = (lm_fifo_handle_t *)p_u8_addr;
        p_u8_addr += ALIGN(sizeof(lm_fifo_handle_t), 4);
        p_st_handle->u8_mem_self = 0;
    } else {
        /* need to malloc addr */
        if (!p_v_mem_malloc) {
            p_st_cfg->u8_err_code = HAL_LM_FIFO_ERR_MALLOC;
            return NULL;
        }

        uint8_t u8_track_num = 1;
        uint32_t u32_size = 0;

        if (FIFO_ID_IS_TDM(p_st_cfg->u8_id)) {
            u8_track_num = p_st_cfg->u8_tdm_ch_end - p_st_cfg->u8_tdm_ch_start + 1;

            if (!TDM_TRACK_NUM_IS_VALID(u8_track_num)) {
                p_st_cfg->u8_err_code = HAL_LM_FIFO_ERR_TRACK_NUM;
                return NULL;
            }

            if (!TDM_TRACK_SEL_IS_VALID(p_st_cfg->u8_tdm_ch_start, p_st_cfg->u8_tdm_ch_end)) {
                p_st_cfg->u8_err_code = HAL_LM_FIFO_ERR_TRACK_SEL;
                return NULL;
            }
        }

        hal_lm_fifo_mem_need(u8_track_num, p_st_cfg->u16_num, &u32_size);
        hal_mem_malloc *p_v_malloc = (hal_mem_malloc *)p_v_mem_malloc;
        p_u8_addr = (uint8_t *)p_v_malloc(u32_size);

        if (p_u8_addr == NULL) {
            p_st_cfg->u8_err_code = HAL_LM_FIFO_ERR_MALLOC;
            return NULL;
        }

        p_st_handle = (lm_fifo_handle_t *)p_u8_addr;
        co_dsp_set_q7(0, (int8_t *)p_st_handle, u32_size);
        p_u8_addr += ALIGN(sizeof(lm_fifo_handle_t), 4);
        p_st_handle->u8_mem_self = 1;
    }

    p_st_handle->u8_id = p_st_cfg->u8_id;
    p_st_handle->p_v_env = p_st_cfg->p_v_env;
    p_st_handle->s32_value = p_st_cfg->s32_value;
    p_st_handle->u16_ptr = 0;
    p_st_handle->p_v_nxt = NULL;
    hal_lm_fifo_config(p_st_cfg, p_u8_addr);
    hal_lm_fifo_int_config(p_st_handle, p_st_cfg->u8_int_type, p_st_cfg->p_v_callback, p_st_cfg->u16_num);

    if (!s_p_st_first_fifo) {
        s_p_st_first_fifo = p_st_handle;
    } else {
        lm_fifo_handle_t *p_st_cur = s_p_st_first_fifo;

        for (;;) {
            if (p_st_cur->p_v_nxt == NULL) {
                p_st_cur->p_v_nxt = p_st_handle;
                break;
            }

            p_st_cur = p_st_cur->p_v_nxt;
        }
    }

    return (void *)p_st_handle;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_deinit(void *p_v_handle, void *p_v_mem_free)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;

    if (hal_lm_fifo_int_clear(p_st_handle) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_DEINIT;
    }
    
    fifo_group_handle_t *p_st_cur_group = FIFO_ID_IS_OUT(p_st_handle->u8_id) ? s_p_st_first_group_o : s_p_st_first_group_i;
    uint8_t u8_group = 0; /* TODO: get from register according to p_st_handle->u8_id */

    for (;;) {
        if (!p_st_cur_group) {
            goto NEXT;
        }

        if (p_st_cur_group->u8_id == u8_group) {
            break;
        }

        p_st_cur_group = p_st_cur_group->p_v_nxt;
    }

    if (p_st_cur_group->u32_fifo_bit & CO_BIT(p_st_handle->u8_id)) {
        p_st_cur_group->u32_fifo_bit &= ~CO_BIT(p_st_handle->u8_id);
    }

NEXT:

    if (s_p_st_first_fifo == p_st_handle) {
        s_p_st_first_fifo = p_st_handle->p_v_nxt;
    } else {
        lm_fifo_handle_t *p_st_cur = s_p_st_first_fifo;

        for (;;) {
            if (!p_st_cur) {
                break;
            }

            if (p_st_cur->p_v_nxt == p_st_handle) {
                p_st_cur->p_v_nxt = p_st_handle->p_v_nxt;
                break; 
            }

            p_st_cur = p_st_cur->p_v_nxt;
        }
    }

    if (p_st_handle->u8_mem_self) {
        hal_mem_free *p_v_free = (hal_mem_free *)p_v_mem_free;

        if (p_v_free) {
            p_v_free(p_st_handle);
        } else {
            return HAL_LM_FIFO_ERR_DEINIT;
        }
    }

    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_enable(void *p_v_handle)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;

    if (FIFO_ID_IS_OUT(p_st_handle->u8_id) && (p_st_handle->s32_value != 0)) {
        uint32_t u32_start_addr = 0; /* TODO: fifo start addr get from register */
        uint16_t u16_fifo_len = 0; /* TODO: fifo len get from register */

        if (FIFO_ID_IS_TDM(p_st_handle->u8_id)) {
            uint8_t u8_start_track = 0; /* TODO: tdm start track get from register */
            uint8_t u8_end_track = 0;   /* TODO: tdm end track get from register */

            for (int i = u8_start_track; i <= u8_end_track; i++) {
                uint32_t u32_addr_val = 0; /* TODO: tdm offset addr get from register */
                uint32_t u32_act_addr = u32_addr_val >> ((i % 2) * 16);
                u32_act_addr &= 0xFFFF;
                u32_act_addr <<= 2;
                int32_t *p_s32_addr = (int32_t *)(u32_start_addr + u32_act_addr);
                co_dsp_set_q31(p_st_handle->s32_value, p_s32_addr, u16_fifo_len >> 2);
            }
        } else {
            int32_t *p_s32_addr = (int32_t *)u32_start_addr;
            co_dsp_set_q31(p_st_handle->s32_value, p_s32_addr, u16_fifo_len >> 2);
        }
    }

    p_st_handle->u16_ptr = 0;
    /* TODO: set fifo en to register according to p_st_handle->u8_id */
    ;
    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_disable(void *p_v_handle)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    /* TODO: set fifo dis to register according to p_st_handle->u8_id */
    ;
    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_in_which_group_get(void *p_v_handle, uint8_t *p_u8_group)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    *p_u8_group = 0; /* TODO: get from register according to p_st_handle->u8_id */
    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_int_config(void *p_v_handle, hal_lm_fifo_int_type_enum_t e_int, void *p_v_callback, uint16_t u16_int_num)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint32_t u32_value = 0;
    /* TODO: fifo int st clr */
    ;
    p_st_handle->u8_int_type = e_int;
    p_st_handle->p_v_callback = p_v_callback;
    
    switch (e_int) {
    case HAL_LM_FIFO_SINGLE_INT: {
        u32_value = 0; /* TODO: get value from register */
        u32_value |= CO_BIT(p_st_handle->u8_id);
        /* TODO: set value to register */
        ;
        break;
    }
    
    case HAL_LM_FIFO_GROUP_INT: {
        fifo_group_handle_t *p_st_cur_group = NULL;
        fifo_group_handle_t *p_st_nxt_group = FIFO_ID_IS_IN(p_st_handle->u8_id) ? s_p_st_first_group_i : s_p_st_first_group_o;
        uint8_t u8_group = 0; /* TODO: get from register according to p_st_handle->u8_id */

        /* 检查一个FIFO是否存在于多个group中 */
        for (;;) {
            if (p_st_nxt_group == NULL) {
                break;
            }

            if (p_st_nxt_group->u8_id == u8_group) {
                /* 获取当前正确 group 的处理结构体指针 */
                p_st_cur_group = p_st_nxt_group;
            } else {
                if (p_st_nxt_group->u32_fifo_bit & CO_BIT(p_st_handle->u8_id)) {
                    return HAL_LM_FIFO_ERR_GROUP_ID;
                }
            }

            p_st_nxt_group = p_st_nxt_group->p_v_nxt;
        }

        if (!p_st_cur_group == NULL) return HAL_LM_FIFO_ERR_GROUP;

        if (p_st_cur_group->u8_st == 1) p_st_cur_group->u8_st = 2;

        if (p_st_cur_group->u8_st != 2) return HAL_LM_FIFO_ERR_GROUP_ST;

        p_st_cur_group->p_v_callback = p_v_callback;
        p_st_cur_group->u32_fifo_bit |= CO_BIT(p_st_handle->u8_id);
        s_v_u16_group_st |= CO_BIT(p_st_cur_group->u8_id + FIFO_GROUP_NUM_MAX * FIFO_ID_IS_OUT(p_st_handle->u8_id));

        if (FIFO_ID_IS_IN(p_st_handle->u8_id)) {
            /* in group */
            /* TODO: set value to register, seperate by in/out */
            ;
        } else {
            /* out group */
            /* TODO: set value to register, seperate by in/out */
            ;
        }

        break;
    }

    case HAL_LM_FIFO_NULL_INT:
        return HAL_LM_FIFO_OK;
    }

    /* TODO: set io cnt thre register */
    ;

    if (u32_value) {
        /* TODO: 使能 single int */
        ;
    }

    if (s_v_u16_group_st) {
        /* TODO: 使能 group int */
        ;
    }

    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_int_clear(void *p_v_handle)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint32_t u32_value = 0;

    switch (p_st_handle->u8_int_type) {
    case HAL_LM_FIFO_SINGLE_INT: {
        u32_value = 0; /* TODO: get int_en value from register */
        u32_value &= ~CO_BIT(p_st_handle->u8_id);
        /* TODO: set int_en value to register */
        ;
        /* TODO: fifo int st clr */
        ;
        p_st_handle->p_v_callback = NULL;

        if (u32_value == 0) {
            /* TODO: 关闭 single int */

        }

        break;
    }

    case HAL_LM_FIFO_GROUP_INT: {
        fifo_group_handle_t *p_st_cur_group = NULL;
        fifo_group_handle_t *p_st_nxt_group;
        uint8_t u8_group = 0; /* TODO: get from register according to p_st_handle->u8_id */

        if (FIFO_ID_IS_IN(p_st_handle->u8_id)) {
            /* TODO: 清除该 group 的中断状态, 区分 in/out */
            ;
            /* in group */
            p_st_nxt_group = s_p_st_first_group_i;
        } else {
            /* TODO: 清除该 group 的中断状态, 区分 in/out */
            ;
            /* out group */
            p_st_nxt_group = s_p_st_first_group_o;
        }

        for (;;) {
            if (!p_st_nxt_group) {
                break;
            }

            if (p_st_nxt_group->u8_id == u8_group) {
                p_st_cur_group = p_st_nxt_group;
                break;
            }

            p_st_nxt_group = p_st_nxt_group->p_v_nxt;
        }

        if (!p_st_cur_group) return HAL_LM_FIFO_ERR_GROUP_ID;

        if (p_st_cur_group->u8_st == 2) p_st_cur_group->u8_st = 1;

        if (p_st_cur_group->u8_st != 1) return HAL_LM_FIFO_ERR_GROUP_ST;

        /* TODO: 清除该 group 中所有 FIFO 对应的中断状态 */
        ;
        p_st_cur_group->u32_fifo_bit &= ~CO_BIT(p_st_handle->u8_id);

        if (p_st_cur_group->u32_fifo_bit == 0) {
            p_st_cur_group->p_v_callback = NULL;
            s_v_u16_group_st &= ~CO_BIT(p_st_cur_group->u8_id + FIFO_GROUP_NUM_MAX * FIFO_ID_IS_OUT(p_st_handle->u8_id));
        }

        if (s_v_u16_group_st == 0) {
            /* TODO: 关闭 group int */

        }

        break;
    }

    case HAL_LM_FIFO_NULL_INT:
        return HAL_LM_FIFO_OK;
    }

    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_start_addr_get(void *p_v_handle, uint32_t *p_u32_addr)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint32_t u32_addr = 0; /* TODO: get from register according to p_st_handle->u8_id */
    *p_u32_addr = u32_addr;
    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_int_num_get(void *p_v_handle, uint16_t *p_u16_int_num)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint16_t u16_fifo_len = 0; /* TODO: get from register according to p_st_handle->u8_id */
    u16_fifo_len >>= 3;
    *p_u16_int_num = u16_fifo_len;
    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_rw_ptr_get(void *p_v_handle, uint16_t *p_u16_ptr)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    *p_u16_ptr = p_st_handle->u16_ptr;
    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_rw_ptr_set(void *p_v_handle, uint16_t u16_ptr)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    p_st_handle->u16_ptr = u16_ptr;
    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_ro_ptr_get(void *p_v_handle, uint16_t *p_u16_ptr)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;

    if (FIFO_ID_IS_IN(p_st_handle->u8_id)) {
        *p_u16_ptr = 0; /* TODO: get wr ptr from register */
    } else {
        *p_u16_ptr = 0; /* TODO: get rd ptr from register */
    }

    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_valid_num_get(void *p_v_handle, uint16_t *p_u16_valid_num)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint16_t u16_fifo_len = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint16_t u16_ro_ptr;

    if (FIFO_ID_IS_IN(p_st_handle->u8_id)) {
        u16_ro_ptr = 0; /* TODO: get wr ptr from register */

        if (u16_ro_ptr >= p_st_handle->u16_ptr) {
            *p_u16_valid_num = u16_ro_ptr - p_st_handle->u16_ptr;
        } else {
            *p_u16_valid_num = u16_ro_ptr + (u16_fifo_len >> 2) - p_st_handle->u16_ptr;
        }
    } else {
        u16_ro_ptr = 0; /* TODO: get rd ptr from register */

        if (p_st_handle->u16_ptr >= u16_ro_ptr) {
            *p_u16_valid_num = p_st_handle->u16_ptr - u16_ro_ptr;
        } else {
            *p_u16_valid_num = p_st_handle->u16_ptr + (u16_fifo_len >> 2) - u16_ro_ptr;
        }
    }

    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_proc_addr_get(void *p_v_handle, uint32_t *p_u32_proc_addr, uint8_t u8_tdm_sel)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint32_t u32_start_addr = 0; /* TODO: get from register according to p_st_handle->u8_id */

    if (FIFO_ID_IS_TDM(p_st_handle->u8_id)) {
        uint32_t u32_addr_val;

        if (p_st_handle->u8_id == HAL_LM_FIFO_TDM_I) {
            u32_addr_val = 0; /* TODO: get tdm offset addr from register by u8_tdm_sel */
        } else {
            u32_addr_val = 0; /* TODO: get tdm offset addr from register by u8_tdm_sel */
        }

        uint32_t u32_act_addr = ((u32_addr_val >> ((u8_tdm_sel % 2) * 16)) & 0xFFFF) << 2;
        *p_u32_proc_addr = u32_start_addr + u32_act_addr + (p_st_handle->u16_ptr << 2);
    } else {
        *p_u32_proc_addr = u32_start_addr + (p_st_handle->u16_ptr << 2);
    }

    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_proc_addr_set(void *p_v_handle, uint32_t u32_proc_addr, uint16_t u16_proc_num, uint8_t u8_tdm_sel)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint32_t u32_start_addr = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint16_t u16_fifo_len = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint16_t u16_ptr = 0;

    if (FIFO_ID_IS_TDM(p_st_handle->u8_id)) {
        uint32_t u32_addr_val;

        if (p_st_handle->u8_id == HAL_LM_FIFO_TDM_I) {
            u32_addr_val = 0; /* TODO: get tdm offset addr from register by u8_tdm_sel */
        } else {
            u32_addr_val = 0; /* TODO: get tdm offset addr from register by u8_tdm_sel */
        }

        uint32_t u32_act_addr = ((u32_addr_val >> ((u8_tdm_sel % 2) * 16)) & 0xFFFF) << 2;
        u16_ptr = u32_proc_addr - u32_start_addr - u32_act_addr;
    } else {
        u16_ptr = u32_proc_addr - u32_start_addr;
    }

    if ((u16_ptr >> 2) != p_st_handle->u16_ptr) {
        return HAL_LM_FIFO_ERR_PTR;
    }

    u16_ptr += (u16_proc_num << 2);
    u16_ptr = (u16_ptr >= u16_fifo_len) ? (u16_ptr - u16_fifo_len) : u16_ptr;
    p_st_handle->u16_ptr = u16_ptr >> 2;
    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_total_len_get(void *p_v_handle, uint16_t *p_u16_total_len)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint16_t u16_fifo_len = 0; /* TODO: get from register according to p_st_handle->u8_id */
    *p_u16_total_len = u16_fifo_len;
    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_read_s32(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint32_t u32_start_addr = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint16_t u16_fifo_len = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint32_t u32_proc_addr;

    if (hal_lm_fifo_proc_addr_get(p_st_handle, &u32_proc_addr, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    int32_t *p_s32_src = (int32_t *)u32_proc_addr;
    int32_t *p_s32_dst = (int32_t *)p_v_buf;

    if ((u32_proc_addr + (u16_data_num << 2)) > (u32_start_addr + u16_fifo_len)) {
        /* 分两次读取 */
        uint16_t u16_rest_len = (u32_start_addr + u16_fifo_len) - u32_proc_addr;
        uint16_t u16_proc_num = u16_rest_len >> 2;
        co_dsp_dup_q31(p_s32_src, p_s32_dst, u16_proc_num);
        p_s32_dst += u16_proc_num;
        p_s32_src = (int32_t *)u32_start_addr;
        u16_proc_num = u16_data_num - u16_proc_num;
        co_dsp_dup_q31(p_s32_src, p_s32_dst, u16_proc_num);
    } else {
        /* 一次读取 */
        co_dsp_dup_q31(p_s32_src, p_s32_dst, u16_data_num);
    }

    if (hal_lm_fifo_proc_addr_set(p_st_handle, u32_proc_addr, u16_data_num, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_read_s24(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint32_t u32_start_addr = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint16_t u16_fifo_len = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint32_t u32_proc_addr;

    if (hal_lm_fifo_proc_addr_get(p_st_handle, &u32_proc_addr, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    int32_t *p_s32_src = (int32_t *)u32_proc_addr;
    int32_t *p_s32_dst = (int32_t *)p_v_buf;

    if ((u32_proc_addr + (u16_data_num << 2)) > (u32_start_addr + u16_fifo_len)) {
        /* 分两次读取 */
        uint16_t u16_rest_len = (u32_start_addr + u16_fifo_len) - u32_proc_addr;
        uint16_t u16_proc_num = u16_rest_len >> 2;
        co_dsp_convert_q31_q23(p_s32_src, p_s32_dst, u16_proc_num);
        p_s32_dst += u16_proc_num;
        p_s32_src = (int32_t *)u32_start_addr;
        u16_proc_num = u16_data_num - u16_proc_num;
        co_dsp_convert_q31_q23(p_s32_src, p_s32_dst, u16_proc_num);
    } else {
        /* 一次读取 */
        co_dsp_convert_q31_q23(p_s32_src, p_s32_dst, u16_data_num);
    }

    if (hal_lm_fifo_proc_addr_set(p_st_handle, u32_proc_addr, u16_data_num, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_read_s16(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint32_t u32_start_addr = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint16_t u16_fifo_len = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint32_t u32_proc_addr;

    if (hal_lm_fifo_proc_addr_get(p_st_handle, &u32_proc_addr, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    int32_t *p_s32_src = (int32_t *)u32_proc_addr;
    int16_t *p_s16_dst = (int16_t *)p_v_buf;

    if ((u32_proc_addr + (u16_data_num << 2)) > (u32_start_addr + u16_fifo_len)) {
        /* 分两次读取 */
        uint16_t u16_rest_len = (u32_start_addr + u16_fifo_len) - u32_proc_addr;
        uint16_t u16_proc_num = u16_rest_len >> 2;
        co_dsp_convert_q31_q15(p_s32_src, p_s16_dst, u16_proc_num);
        p_s16_dst += u16_proc_num;
        p_s32_src = (int32_t *)u32_start_addr;
        u16_proc_num = u16_data_num - u16_proc_num;
        co_dsp_convert_q31_q15(p_s32_src, p_s16_dst, u16_proc_num);
    } else {
        /* 一次读取 */
        co_dsp_convert_q31_q15(p_s32_src, p_s16_dst, u16_data_num);
    }

    if (hal_lm_fifo_proc_addr_set(p_st_handle, u32_proc_addr, u16_data_num, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_read_s08(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint32_t u32_start_addr = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint16_t u16_fifo_len = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint32_t u32_proc_addr;

    if (hal_lm_fifo_proc_addr_get(p_st_handle, &u32_proc_addr, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    int32_t *p_s32_src = (int32_t *)u32_proc_addr;
    int8_t *p_s8_dst = (int8_t *)p_v_buf;

    if ((u32_proc_addr + (u16_data_num << 2)) > (u32_start_addr + u16_fifo_len)) {
        /* 分两次读取 */
        uint16_t u16_rest_len = (u32_start_addr + u16_fifo_len) - u32_proc_addr;
        uint16_t u16_proc_num = u16_rest_len >> 2;
        co_dsp_convert_q31_q7(p_s32_src, p_s8_dst, u16_proc_num);
        p_s8_dst += u16_proc_num;
        p_s32_src = (int32_t *)u32_start_addr;
        u16_proc_num = u16_data_num - u16_proc_num;
        co_dsp_convert_q31_q7(p_s32_src, p_s8_dst, u16_proc_num);
    } else {
        /* 一次读取 */
        co_dsp_convert_q31_q7(p_s32_src, p_s8_dst, u16_data_num);
    }

    if (hal_lm_fifo_proc_addr_set(p_st_handle, u32_proc_addr, u16_data_num, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_read_null(void *p_v_handle, uint16_t u16_data_num, uint8_t u8_tdm_sel)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint32_t u32_proc_addr;

    if (hal_lm_fifo_proc_addr_get(p_st_handle, &u32_proc_addr, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    if (hal_lm_fifo_proc_addr_set(p_st_handle, u32_proc_addr, u16_data_num, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_write_s32(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint32_t u32_start_addr = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint16_t u16_fifo_len = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint32_t u32_proc_addr;

    if (hal_lm_fifo_proc_addr_get(p_st_handle, &u32_proc_addr, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    int32_t *p_s32_src = (int32_t *)p_v_buf;
    int32_t *p_s32_dst = (int32_t *)u32_proc_addr;

    if ((u32_proc_addr + (u16_data_num << 2)) > (u32_start_addr + u16_fifo_len)) {
        /* 分两次写入 */
        uint16_t u16_rest_len = (u32_start_addr + u16_fifo_len) - u32_proc_addr;
        uint16_t u16_proc_num = u16_rest_len >> 2;
        co_dsp_dup_q31(p_s32_src, p_s32_dst, u16_proc_num);
        p_s32_src += u16_proc_num;
        p_s32_dst = (int32_t *)u32_start_addr;
        u16_proc_num = u16_data_num - u16_proc_num;
        co_dsp_dup_q31(p_s32_src, p_s32_dst, u16_proc_num);
    } else {
        /* 一次写入 */
        co_dsp_dup_q31(p_s32_src, p_s32_dst, u16_data_num);
    }

    if (hal_lm_fifo_proc_addr_set(p_st_handle, u32_proc_addr, u16_data_num, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_write_s24(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint32_t u32_start_addr = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint16_t u16_fifo_len = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint32_t u32_proc_addr;

    if (hal_lm_fifo_proc_addr_get(p_st_handle, &u32_proc_addr, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    int32_t *p_s32_src = (int32_t *)p_v_buf;
    int32_t *p_s32_dst = (int32_t *)u32_proc_addr;

    if ((u32_proc_addr + (u16_data_num << 2)) > (u32_start_addr + u16_fifo_len)) {
        /* 分两次写入 */
        uint16_t u16_rest_len = (u32_start_addr + u16_fifo_len) - u32_proc_addr;
        uint16_t u16_proc_num = u16_rest_len >> 2;
        co_dsp_convert_q23_q31(p_s32_src, p_s32_dst, u16_proc_num);
        p_s32_src += u16_proc_num;
        p_s32_dst = (int32_t *)u32_start_addr;
        u16_proc_num = u16_data_num - u16_proc_num;
        co_dsp_convert_q23_q31(p_s32_src, p_s32_dst, u16_proc_num);
    } else {
        /* 一次写入 */
        co_dsp_convert_q23_q31(p_s32_src, p_s32_dst, u16_data_num);
    }

    if (hal_lm_fifo_proc_addr_set(p_st_handle, u32_proc_addr, u16_data_num, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_write_s16(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint32_t u32_start_addr = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint16_t u16_fifo_len = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint32_t u32_proc_addr;

    if (hal_lm_fifo_proc_addr_get(p_st_handle, &u32_proc_addr, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    int16_t *p_s16_src = (int16_t *)p_v_buf;
    int32_t *p_s32_dst = (int32_t *)u32_proc_addr;

    if ((u32_proc_addr + (u16_data_num << 2)) > (u32_start_addr + u16_fifo_len)) {
        /* 分两次写入 */
        uint16_t u16_rest_len = (u32_start_addr + u16_fifo_len) - u32_proc_addr;
        uint16_t u16_proc_num = u16_rest_len >> 2;
        co_dsp_convert_q15_q31(p_s16_src, p_s32_dst, u16_proc_num);
        p_s16_src += u16_proc_num;
        p_s32_dst = (int32_t *)u32_start_addr;
        u16_proc_num = u16_data_num - u16_proc_num;
        co_dsp_convert_q15_q31(p_s16_src, p_s32_dst, u16_proc_num);
    } else {
        /* 一次写入 */
        co_dsp_convert_q15_q31(p_s16_src, p_s32_dst, u16_data_num);
    }

    if (hal_lm_fifo_proc_addr_set(p_st_handle, u32_proc_addr, u16_data_num, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_write_s08(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint32_t u32_start_addr = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint16_t u16_fifo_len = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint32_t u32_proc_addr;

    if (hal_lm_fifo_proc_addr_get(p_st_handle, &u32_proc_addr, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    int8_t *p_s8_src = (int8_t *)p_v_buf;
    int32_t *p_s32_dst = (int32_t *)u32_proc_addr;

    if ((u32_proc_addr + (u16_data_num << 2)) > (u32_start_addr + u16_fifo_len)) {
        /* 分两次写入 */
        uint16_t u16_rest_len = (u32_start_addr + u16_fifo_len) - u32_proc_addr;
        uint16_t u16_proc_num = u16_rest_len >> 2;
        co_dsp_convert_q7_q31(p_s8_src, p_s32_dst, u16_proc_num);
        p_s8_src += u16_proc_num;
        p_s32_dst = (int32_t *)u32_start_addr;
        u16_proc_num = u16_data_num - u16_proc_num;
        co_dsp_convert_q7_q31(p_s8_src, p_s32_dst, u16_proc_num);
    } else {
        /* 一次写入 */
        co_dsp_convert_q7_q31(p_s8_src, p_s32_dst, u16_data_num);
    }

    if (hal_lm_fifo_proc_addr_set(p_st_handle, u32_proc_addr, u16_data_num, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    return HAL_LM_FIFO_OK;
}

hal_lm_fifo_err_enum_t hal_lm_fifo_write_zero(void *p_v_handle, uint16_t u16_data_num, uint8_t u8_tdm_sel)
{
    if (!p_v_handle) return HAL_LM_FIFO_NO_INIT;

    lm_fifo_handle_t *p_st_handle = (lm_fifo_handle_t *)p_v_handle;
    uint32_t u32_start_addr = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint16_t u16_fifo_len = 0; /* TODO: get from register according to p_st_handle->u8_id */
    uint32_t u32_proc_addr;

    if (hal_lm_fifo_proc_addr_get(p_st_handle, &u32_proc_addr, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    int32_t *p_s32_dst = (int32_t *)u32_proc_addr;

    if ((u32_proc_addr + (u16_data_num << 2)) > (u32_start_addr + u16_fifo_len)) {
        /* 分两次写入 */
        uint16_t u16_rest_len = (u32_start_addr + u16_fifo_len) - u32_proc_addr;
        uint16_t u16_proc_num = u16_rest_len >> 2;
        co_dsp_set_q31(0, p_s32_dst, u16_proc_num);
        p_s32_dst = (int32_t *)u32_start_addr;
        u16_proc_num = u16_data_num - u16_proc_num;
        co_dsp_set_q31(0, p_s32_dst, u16_proc_num);
    } else {
        /* 一次写入 */
        co_dsp_set_q31(0, p_s32_dst, u16_data_num);
    }

    if (hal_lm_fifo_proc_addr_set(p_st_handle, u32_proc_addr, u16_data_num, u8_tdm_sel) != HAL_LM_FIFO_OK) {
        return HAL_LM_FIFO_ERR_PROC_ADDR;
    }

    return HAL_LM_FIFO_OK;
}
