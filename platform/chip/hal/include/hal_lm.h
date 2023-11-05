/*
 * @Author: chenbw 1069381755@qq.com
 * @Date: 2023-11-02 23:05:55
 * @LastEditors: chenbw 1069381755@qq.com
 * @LastEditTime: 2023-11-06 00:26:22
 * @FilePath: \code_demo\platform\chip\hal\include\hal_lm.h
 * @Description: 
 * 
 * Copyright (c) 2023 by chenbw, All Rights Reserved. 
 */
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

/*
 * ENUM DEFINES
 **************************************************************************************************
 */
typedef enum _hal_lm_fifo_id_enum {
    HAL_LM_FIFO_ADC_0,                          /* 0 */
    HAL_LM_FIFO_ADC_1,                          /* 1 */
    HAL_LM_FIFO_PDM_0,                          /* 2 */
    HAL_LM_FIFO_PDM_1,                          /* 3 */
    HAL_LM_FIFO_I2S0_I_L,                       /* 4 */
    HAL_LM_FIFO_I2S0_I_R,                       /* 5 */
    HAL_LM_FIFO_I2S1_I_L,                       /* 6 */
    HAL_LM_FIFO_I2S1_I_R,                       /* 7 */
    HAL_LM_FIFO_I2S2_I_L,                       /* 8 */
    HAL_LM_FIFO_I2S2_I_R,                       /* 9 */
    HAL_LM_FIFO_I2S3_I_L,                       /* 10 */
    HAL_LM_FIFO_I2S3_I_R,                       /* 11 */
    HAL_LM_FIFO_SPDIF_I_L,                      /* 12 */
    HAL_LM_FIFO_SPDIF_I_R,                      /* 13 */
    HAL_LM_FIFO_TDM0_I,                         /* 14 */
    HAL_LM_FIFO_TDM1_I,                         /* 15 */
    HAL_LM_FIFO_IN_END = HAL_LM_FIFO_TDM1_I,

    HAL_LM_FIFO_OUT_START,
    HAL_LM_FIFO_DAC_0 = HAL_LM_FIFO_OUT_START,  /* 16 */
    HAL_LM_FIFO_DAC_1,                          /* 17 */
    HAL_LM_FIFO_RFU_0,                          /* 18 */
    HAL_LM_FIFO_RFU_1,                          /* 19 */
    HAL_LM_FIFO_I2S0_O_L,                       /* 20 */
    HAL_LM_FIFO_I2S0_O_R,                       /* 21 */
    HAL_LM_FIFO_I2S1_O_L,                       /* 22 */
    HAL_LM_FIFO_I2S1_O_R,                       /* 23 */
    HAL_LM_FIFO_I2S2_O_L,                       /* 24 */
    HAL_LM_FIFO_I2S2_O_R,                       /* 25 */
    HAL_LM_FIFO_I2S3_O_L,                       /* 26 */
    HAL_LM_FIFO_I2S3_O_R,                       /* 27 */
    HAL_LM_FIFO_SPDIF_O_L,                      /* 28 */
    HAL_LM_FIFO_SPDIF_O_R,                      /* 29 */
    HAL_LM_FIFO_TDM0_O,                         /* 30 */
    HAL_LM_FIFO_TDM1_O,                         /* 31 */

    HAL_LM_FIFO_NUM,
} hal_lm_fifo_id_enum_t;

typedef enum _hal_lm_fifo_int_type_enum {
    HAL_LM_FIFO_SINGLE_INT,         /* 独立中断 */
    HAL_LM_FIFO_GROUP_INT,          /* 小组中断 */
    HAL_LM_FIFO_NULL_INT = 0xFF,    /* 不使用中断 */
} hal_lm_fifo_int_type_enum_t;

typedef enum _hal_lm_dma_id_enum {
    HAL_LM_DMA_CH_0,    /* DLM->DLM */
    HAL_LM_DMA_CH_1,    /* DLM->DLM */
    HAL_LM_DMA_CH_2,    /* DLM->DLM */
    HAL_LM_DMA_CH_3,    /* DLM->DLM */
    HAL_LM_DMA_CH_4,    /* Scatter/Gather */
    HAL_LM_DMA_NUM,
} hal_lm_dma_id_enum_t;

typedef enum _hal_lm_err_enum {
    HAL_LM_OK,                  /* 0 process ok */
    HAL_LM_NO_INIT,             /* 1 没有进行初始化 */
    HAL_LM_ERR_INIT,            /* 2 初始化错误 */
    HAL_LM_ERR_DEINIT,          /* 3 反初始化错误，一般是没有传入 mem_free 函数指针 */
    HAL_LM_ERR_NULL,            /* 4 参数存在 NULL */
    HAL_LM_ERR_MALLOC,          /* 5 地址开辟错误 */
    HAL_LM_ERR_ADDR,            /* 6 地址错误，一般是地址处于无效区域 */
    HAL_LM_ERR_FIFO_CH_NUM,     /* 7 fifo 通道数量错误，一般是传 0 或者超过 TDM 最大通道数 */
    HAL_LM_ERR_FIFO_TRACK_SEL,  /* 8 fifo tdm 通道选择错误 */
    HAL_LM_ERR_FIFO_GROUP,      /* 9 fifo group 获取错误，一般是没有可用的 group 了 */
    HAL_LM_ERR_FIFO_GROUP_ID,   /* 10 fifo group 选择错误 */
    HAL_LM_ERR_FIFO_GROUP_ST,   /* 11 fifo group 状态错误 */
    HAL_LM_ERR_FIFO_PTR,        /* 12 fifo 指针错误 */
    HAL_LM_ERR_FIFO_PROC_ADDR,  /* 13 fifo 处理地址错误 */
    HAL_LM_ERR_DMA_CH_NUM,      /* 14 dma 通道错误 */
    HAL_LM_ERR_DMA_BUSY,        /* 15 dma 通道正在工作 */
} hal_lm_err_enum_t;

/*
 * STRUCTURE DEFINES
 **************************************************************************************************
 */
typedef void(hal_lm_callback)(void *p_v_handle, uint8_t u8_id);

typedef struct _hal_lm_fifo_cfg {
    void     *p_v_env;          /* 上层模块指针 */
    void     *p_v_callback;     /* fifo int callback, ref hal_lm_callback */
    uint8_t  *p_u8_addr;        /* fifo used start addr, 上层传递或内部开辟，如果上层传递，总长度需要通过 hal_lm_fifo_mem_need 获取 */
    int32_t  s32_value;         /* fifo init value */
    uint16_t u16_half_depth;    /* fifo 一半的深度, 必须为 u16_int_num 的整数倍, words */
    uint16_t u16_int_num;       /* fifo 中断点数, words */
    uint8_t  u8_int_type;       /* fifo 中断类型, ref hal_lm_fifo_int_type_enum_t */
    uint8_t  u8_tdm_ch_start;   /* fifo 为 TDM 时, 起始通道号, valid with u8_id==HAL_LM_FIFO_TDM_I||u8_id==HAL_LM_FIFO_TDM_O*/
    uint8_t  u8_tdm_ch_end;     /* fifo 为 TDM 时, 终止通道号, valid with u8_id==HAL_LM_FIFO_TDM_I||u8_id==HAL_LM_FIFO_TDM_O*/
    uint8_t  u8_group;          /* fifo in which group, set by hal_lm_fifo_free_group_get */
    uint8_t  u8_id;             /* fifo id, ref hal_lm_fifo_id_enum_t */
    uint8_t  u8_err_code;       /* ref hal_lm_err_enum_t */
} hal_lm_fifo_cfg_t;

typedef struct _hal_lm_fifo_en_cfg {
    volatile uint32_t   u32_adc_0       : 1, /* [0] 0:no proc, 1:do proc */
                        u32_adc_1       : 1, /* [1] 0:no proc, 1:do proc */
                        u32_pdm_0       : 1, /* [2] 0:no proc, 1:do proc */
                        u32_pdm_1       : 1, /* [3] 0:no proc, 1:do proc */
                        u32_i2s0_i_l    : 1, /* [4] 0:no proc, 1:do proc */
                        u32_i2s0_i_r    : 1, /* [5] 0:no proc, 1:do proc */
                        u32_i2s1_i_l    : 1, /* [6] 0:no proc, 1:do proc */
                        u32_i2s1_i_r    : 1, /* [7] 0:no proc, 1:do proc */
                        u32_i2s2_i_l    : 1, /* [8] 0:no proc, 1:do proc */
                        u32_i2s2_i_r    : 1, /* [9] 0:no proc, 1:do proc */
                        u32_i2s3_i_l    : 1, /* [10] 0:no proc, 1:do proc */
                        u32_i2s3_i_r    : 1, /* [11] 0:no proc, 1:do proc */
                        u32_spdif_i_l   : 1, /* [12] 0:no proc, 1:do proc */
                        u32_spdif_i_r   : 1, /* [13] 0:no proc, 1:do proc */
                        u32_tdm0_i      : 1, /* [14] 0:no proc, 1:do proc */
                        u32_tdm1_i      : 1, /* [15] 0:no proc, 1:do proc */
                        u32_dac_0       : 1, /* [16] 0:no proc, 1:do proc */
                        u32_dac_1       : 1, /* [17] 0:no proc, 1:do proc */
                        u32_rfu_0       : 2,
                        u32_i2s0_o_l    : 1, /* [20] 0:no proc, 1:do proc */
                        u32_i2s0_o_r    : 1, /* [21] 0:no proc, 1:do proc */
                        u32_i2s1_o_l    : 1, /* [22] 0:no proc, 1:do proc */
                        u32_i2s1_o_r    : 1, /* [23] 0:no proc, 1:do proc */
                        u32_i2s2_o_l    : 1, /* [24] 0:no proc, 1:do proc */
                        u32_i2s2_o_r    : 1, /* [25] 0:no proc, 1:do proc */
                        u32_i2s3_o_l    : 1, /* [26] 0:no proc, 1:do proc */
                        u32_i2s3_o_r    : 1, /* [27] 0:no proc, 1:do proc */
                        u32_spdif_o_l   : 1, /* [28] 0:no proc, 1:do proc */
                        u32_spdif_o_r   : 1, /* [29] 0:no proc, 1:do proc */
                        u32_tdm0_o      : 1, /* [30] 0:no proc, 1:do proc */
                        u32_tdm1_o      : 1; /* [31] 0:no proc, 1:do proc */
} hal_lm_fifo_en_cfg_t;

typedef struct _hal_lm_dma_normal_cfg {
    uint32_t u32_src_addr;  /* 源数据起始地址 */
    uint32_t u32_dst_addr;  /* 目的数据起始地址 */
    uint16_t u16_src_step;  /* 源数据步长 */
    uint16_t u16_dst_step;  /* 目的数据步长 */
} hal_lm_dma_normal_cfg_t;

typedef struct _hal_lm_dma_sg_cfg {
    uint32_t u32_scatter_addr;      /* 交织起始地址 */
    uint32_t u32_gather_addr[8];    /* 分拆起始地址 */
    uint8_t u8_sign_extension;      /* 位数扩展, 0:高位对齐低位补零, 1:高8bit符号扩展低8bit补零(源数据16bit有效) */
    uint8_t u8_sg_mode;             /* 模式, 0:scatter, 1:scatter+search */
    uint8_t u8_scatter_size;        /* 交织数据位宽, 0:16, 1:24, 2:32 */
    uint8_t u8_gather_size;         /* 分拆数据位宽, 0:16, 1:32, 2:24 */
    uint8_t u8_data_gap;            /* 需要跳过的word宽度 */
    uint8_t u8_data_ch;             /* 需要传输的通道数, 0:2, 1:4, 2:6, 3:8 */
} hal_lm_dma_sg_cfg_t;

typedef struct _hal_lm_dma_cfg {
    void *p_v_env;              /* 上层模块指针 */
    void *p_v_callback;         /* dma int callback, ref hal_lm_callback */
    void *p_v_cfg;              /* pass hal_lm_dma_normal_cfg_t or hal_lm_dma_sg_cfg_t according to e_id */
    hal_lm_dma_id_enum_t e_id;  /* ref hal_lm_dma_id_enum_t */
    uint16_t u16_num;           /* proc num */
    uint8_t u8_err_code;        /* ref hal_lm_err_enum_t */
} hal_lm_dma_cfg_t;

/*
 * GLOBAL VARIABLE DECLARATIONS
 **************************************************************************************************
 */

/*
 * EXPORTED FUNCTION DECLARATIONS
 **************************************************************************************************
 */
hal_lm_err_enum_t hal_lm_fifo_mem_need(uint8_t u8_track_num, uint16_t u16_int_num, uint32_t *p_u32_size);

hal_lm_err_enum_t hal_lm_fifo_unused_group_get(uint8_t u8_io_sel, uint8_t *p_u8_group, void *p_v_mem_malloc);

hal_lm_err_enum_t hal_lm_fifo_used_group_release(uint8_t u8_io_sel, uint8_t u8_group, void *p_v_mem_free);

hal_lm_err_enum_t hal_lm_fifo_used_group_en_cfg(uint8_t u8_io_sel, uint8_t u8_group, uint8_t u8_en);

hal_lm_err_enum_t hal_lm_fifo_en_cfg(hal_lm_fifo_en_cfg_t *p_st_en, uint8_t u8_en);

void *hal_lm_fifo_init(hal_lm_fifo_cfg_t *p_st_cfg, void *p_v_mem_malloc);

hal_lm_err_enum_t hal_lm_fifo_deinit(void *p_v_handle, void *p_v_mem_free);

hal_lm_err_enum_t hal_lm_fifo_enable(void *p_v_handle);

hal_lm_err_enum_t hal_lm_fifo_disable(void *p_v_handle);

hal_lm_err_enum_t hal_lm_fifo_in_which_group_get(void *p_v_handle, uint8_t *p_u8_group);

hal_lm_err_enum_t hal_lm_fifo_int_config(void *p_v_handle, hal_lm_fifo_int_type_enum_t e_int, void *p_v_callback, uint16_t u16_int_num);

hal_lm_err_enum_t hal_lm_fifo_int_clear(void *p_v_handle);

hal_lm_err_enum_t hal_lm_fifo_start_addr_get(void *p_v_handle, uint32_t *p_u32_addr);

hal_lm_err_enum_t hal_lm_fifo_int_num_get(void *p_v_handle, uint16_t *p_u16_int_num);

hal_lm_err_enum_t hal_lm_fifo_rw_ptr_get(void *p_v_handle, uint16_t *p_u16_ptr);

hal_lm_err_enum_t hal_lm_fifo_rw_ptr_set(void *p_v_handle, uint16_t u16_ptr);

hal_lm_err_enum_t hal_lm_fifo_ro_ptr_get(void *p_v_handle, uint16_t *p_u16_ptr);

hal_lm_err_enum_t hal_lm_fifo_valid_num_get(void *p_v_handle, uint16_t *p_u16_valid_num);

hal_lm_err_enum_t hal_lm_fifo_proc_addr_get(void *p_v_handle, uint32_t *p_u32_proc_addr, uint8_t u8_tdm_sel);

hal_lm_err_enum_t hal_lm_fifo_proc_addr_set(void *p_v_handle, uint32_t u32_proc_addr, uint16_t u16_proc_num, uint8_t u8_tdm_sel);

hal_lm_err_enum_t hal_lm_fifo_total_len_get(void *p_v_handle, uint16_t *p_u16_total_len);

hal_lm_err_enum_t hal_lm_fifo_read_s32(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_err_enum_t hal_lm_fifo_read_s24(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_err_enum_t hal_lm_fifo_read_s16(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_err_enum_t hal_lm_fifo_read_s08(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_err_enum_t hal_lm_fifo_read_null(void *p_v_handle, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_err_enum_t hal_lm_fifo_write_s32(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_err_enum_t hal_lm_fifo_write_s24(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_err_enum_t hal_lm_fifo_write_s16(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_err_enum_t hal_lm_fifo_write_s08(void *p_v_handle, void *p_v_buf, uint16_t u16_data_num, uint8_t u8_tdm_sel);

hal_lm_err_enum_t hal_lm_fifo_write_zero(void *p_v_handle, uint16_t u16_data_num, uint8_t u8_tdm_sel);

#endif /* __HAL_LM_H__ */
