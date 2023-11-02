#ifndef __HAL_LM_H__
#define __HAL_LM_H__

/*
 * INCLUDE FILES
 **************************************************************************************************
 */
#include "hal_def.h"

/*
 * MACRO DEFINES
 **************************************************************************************************
 */
#define HAL_LM_FIFO_TDM_TRACK_NUM_MAX       (8)

/*
 * ENUM DEFINES
 **************************************************************************************************
 */
typedef enum _hal_lm_fifo_id_enum {
    HAL_LM_FIFO_ADC_0,                          /* 0 */
    HAL_LM_FIFO_ADC_1,                          /* 1 */
    HAL_LM_FIFO_PDM_0,                          /* 2 */
    HAL_LM_FIFO_PDM_1,                          /* 3 */
    HAL_LM_FIFO_I2S_I_L,                        /* 4 */
    HAL_LM_FIFO_I2S_I_R,                        /* 5 */
    HAL_LM_FIFO_SPDIF_I_L,                      /* 6 */
    HAL_LM_FIFO_SPDIF_I_R,                      /* 7 */
    HAL_LM_FIFO_TDM_I,                          /* 8 */
    HAL_LM_FIFO_IN_END = 15,

    HAL_LM_FIFO_OUT_START,
    HAL_LM_FIFO_DAC_0 = HAL_LM_FIFO_OUT_START,  /* 16 */
    HAL_LM_FIFO_DAC_1,                          /* 17 */
    HAL_LM_FIFO_I2S_O_L,                        /* 18 */
    HAL_LM_FIFO_I2S_O_R,                        /* 19 */
    HAL_LM_FIFO_SPDIF_O_L,                      /* 20 */
    HAL_LM_FIFO_SPDIF_O_R,                      /* 21 */
    HAL_LM_FIFO_TDM_O,                          /* 22 */

    HAL_LM_FIFO_NUM,
} hal_lm_fifo_id_enum_t;

typedef enum _hal_lm_fifo_err_enum {
    HAL_LM_FIFO_OK,             /* 0 fifo process ok */
    HAL_LM_FIFO_NO_INIT,        /* 1 fifo 没有进行初始化 */
    HAL_LM_FIFO_ERR_DEINIT,     /* 2 fifo 反初始化错误，一般是没有传入 mem_free 函数指针 */
    HAL_LM_FIFO_ERR_NULL,       /* 3 fifo 参数存在 NULL */
    HAL_LM_FIFO_ERR_TRACK_NUM,  /* 4 fifo 通道数量错误，一般是传 0 或者超过 TDM 最大通道数 */
    HAL_LM_FIFO_ERR_TRACK_SEL,  /* 5 fifo tdm 通道选择错误 */
    HAL_LM_FIFO_ERR_ADDR,       /* 6 fifo 地址错误，一般是地址处于无效区域 */
    HAL_LM_FIFO_ERR_MALLOC,     /* 7 fifo 地址开辟错误 */
    HAL_LM_FIFO_ERR_GROUP,      /* 8 fifo group 获取错误，一般是没有可用的 group 了 */
    HAL_LM_FIFO_ERR_GROUP_ID,   /* 9 fifo group 选择错误 */
    HAL_LM_FIFO_ERR_GROUP_ST,   /* 10 fifo group 状态错误 */
    HAL_LM_FIFO_ERR_PTR,        /* 11 fifo 指针错误 */
    HAL_LM_FIFO_ERR_PROC_ADDR,  /* 12 fifo 处理地址错误 */
} hal_lm_fifo_err_enum_t;

typedef enum _hal_lm_fifo_int_type_enum {
    HAL_LM_FIFO_SINGLE_INT,         /* 独立中断 */
    HAL_LM_FIFO_GROUP_INT,          /* 小组中断 */
    HAL_LM_FIFO_NULL_INT = 0xFF,    /* 不使用中断 */
} hal_lm_fifo_int_type_enum_t;

/*
 * STRUCTURE DEFINES
 **************************************************************************************************
 */
typedef struct _hal_lm_fifo_cfg {
    void     *p_v_env;          /* 上层模块指针 */
    void     *p_v_callback;     /* fifo int callback, ref hal_callback */
    uint8_t  *p_u8_addr;        /* fifo used start addr, 上层传递或内部开辟，如果上层传递，总长度需要通过 hal_lm_fifo_mem_need 获取 */
    int32_t  s32_value;         /* fifo init value */
    uint16_t u16_num;           /* fifo int num */
    uint8_t  u8_int_type;       /* fifo 中断类型, ref hal_lm_fifo_int_type_enum_t */
    uint8_t  u8_tdm_ch_start;   /* fifo 为 TDM 时, 起始通道号, valid with u8_id==HAL_LM_FIFO_TDM_I||u8_id==HAL_LM_FIFO_TDM_O*/
    uint8_t  u8_tdm_ch_end;     /* fifo 为 TDM 时, 终止通道号, valid with u8_id==HAL_LM_FIFO_TDM_I||u8_id==HAL_LM_FIFO_TDM_O*/
    uint8_t  u8_group;          /* fifo in which group, set by hal_lm_fifo_free_group_get */
    uint8_t  u8_id;             /* fifo id, ref hal_lm_fifo_id_enum_t */
    uint8_t  u8_err_code;       /* ref hal_lm_fifo_err_enum_t */
} hal_lm_fifo_cfg_t;

typedef struct _hal_lm_fifo_en_cfg {
    volatile uint32_t   u32_adc_0       : 1, /* [0] 0:no proc, 1:do proc */
                        u32_adc_1       : 1, /* [1] 0:no proc, 1:do proc */
                        u32_pdm_0       : 1, /* [2] 0:no proc, 1:do proc */
                        u32_pdm_1       : 1, /* [3] 0:no proc, 1:do proc */
                        u32_i2s_i_l     : 1, /* [4] 0:no proc, 1:do proc */
                        u32_i2s_i_r     : 1, /* [5] 0:no proc, 1:do proc */
                        u32_spdif_i_l   : 1, /* [6] 0:no proc, 1:do proc */
                        u32_spdif_i_r   : 1, /* [7] 0:no proc, 1:do proc */
                        u32_tdm_i       : 1, /* [8] 0:no proc, 1:do proc */
                        u32_rfu_0       : 7,
                        u32_dac_0       : 1, /* [16] 0:no proc, 1:do proc */
                        u32_dac_1       : 1, /* [17] 0:no proc, 1:do proc */
                        u32_i2s_o_l     : 1, /* [18] 0:no proc, 1:do proc */
                        u32_i2s_o_r     : 1, /* [19] 0:no proc, 1:do proc */
                        u32_spdif_o_l   : 1, /* [20] 0:no proc, 1:do proc */
                        u32_spdif_o_r   : 1, /* [21] 0:no proc, 1:do proc */
                        u32_tdm_o       : 1, /* [22] 0:no proc, 1:do proc */
                        u32_rfu_1       : 9;
} hal_lm_fifo_en_cfg_t;

/*
 * GLOBAL VARIABLE DECLARATIONS
 **************************************************************************************************
 */

/*
 * EXPORTED FUNCTION DECLARATIONS
 **************************************************************************************************
 */
hal_lm_fifo_err_enum_t hal_lm_fifo_mem_need(uint8_t u8_track_num, uint16_t u16_int_num, uint32_t *p_u32_size);

hal_lm_fifo_err_enum_t hal_lm_fifo_unused_group_get(uint8_t u8_io_sel, uint8_t *p_u8_group, void *p_v_mem_malloc);

hal_lm_fifo_err_enum_t hal_lm_fifo_used_group_release(uint8_t u8_io_sel, uint8_t u8_group, void *p_v_mem_free);

hal_lm_fifo_err_enum_t hal_lm_fifo_used_group_en_cfg(uint8_t u8_io_sel, uint8_t u8_group, uint8_t u8_en);

hal_lm_fifo_err_enum_t hal_lm_fifo_en_cfg(hal_lm_fifo_en_cfg_t *p_st_en, uint8_t u8_en);

void *hal_lm_fifo_init(hal_lm_fifo_cfg_t *p_st_cfg, void *p_v_mem_malloc);

hal_lm_fifo_err_enum_t hal_lm_fifo_deinit(void *p_v_handle, void *p_v_mem_free);

hal_lm_fifo_err_enum_t hal_lm_fifo_enable(void *p_v_handle);

hal_lm_fifo_err_enum_t hal_lm_fifo_disable(void *p_v_handle);

hal_lm_fifo_err_enum_t hal_lm_fifo_in_which_group_get(void *p_v_handle, uint8_t *p_u8_group);

hal_lm_fifo_err_enum_t hal_lm_fifo_int_config(void *p_v_handle, hal_lm_fifo_int_type_enum_t e_int, void *p_v_callback, uint16_t u16_int_num);

hal_lm_fifo_err_enum_t hal_lm_fifo_int_clear(void *p_v_handle);

hal_lm_fifo_err_enum_t hal_lm_fifo_start_addr_get(void *p_v_handle, uint32_t *p_u32_addr);

hal_lm_fifo_err_enum_t hal_lm_fifo_int_num_get(void *p_v_handle, uint16_t *p_u16_int_num);

hal_lm_fifo_err_enum_t hal_lm_fifo_rw_ptr_get(void *p_v_handle, uint16_t *p_u16_ptr);

hal_lm_fifo_err_enum_t hal_lm_fifo_rw_ptr_set(void *p_v_handle, uint16_t u16_ptr);

hal_lm_fifo_err_enum_t hal_lm_fifo_ro_ptr_get(void *p_v_handle, uint16_t *p_u16_ptr);

hal_lm_fifo_err_enum_t hal_lm_fifo_valid_num_get(void *p_v_handle, uint16_t *p_u16_valid_num);

hal_lm_fifo_err_enum_t hal_lm_fifo_proc_addr_get(void *p_v_handle, uint32_t *p_u32_proc_addr, uint8_t u8_tdm_sel);

hal_lm_fifo_err_enum_t hal_lm_fifo_proc_addr_set(void *p_v_handle, uint32_t u32_proc_addr, uint16_t u16_proc_num, uint8_t u8_tdm_sel);

hal_lm_fifo_err_enum_t hal_lm_fifo_total_len_get(void *p_v_handle, uint16_t *p_u16_total_len);

hal_lm_fifo_err_enum_t hal_lm_fifo_read_s32(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_fifo_err_enum_t hal_lm_fifo_read_s24(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_fifo_err_enum_t hal_lm_fifo_read_s16(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_fifo_err_enum_t hal_lm_fifo_read_s08(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_fifo_err_enum_t hal_lm_fifo_read_null(void *p_v_handle, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_fifo_err_enum_t hal_lm_fifo_write_s32(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_fifo_err_enum_t hal_lm_fifo_write_s24(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_fifo_err_enum_t hal_lm_fifo_write_s16(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_fifo_err_enum_t hal_lm_fifo_write_s08(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_fifo_err_enum_t hal_lm_fifo_write_zero(void *p_v_handle, uint16_t u16_data_num, uint8_t u8_tdm_sel);

#endif /* __HAL_LM_H__ */
