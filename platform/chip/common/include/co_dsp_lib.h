#ifndef __CO_DSP_LIB_H__
#define __CO_DSP_LIB_H__

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

/*
 * STRUCTURE DEFINES
 **************************************************************************************************
 */

/*
 * GLOBAL VARIABLE DECLARATIONS
 **************************************************************************************************
 */

/*
 * EXPORTED FUNCTION DECLARATIONS
 **************************************************************************************************
 */
void co_dsp_dup_q31(int32_t *p_s32_src, int32_t *p_s32_dst, uint16_t u16_data_num);
void co_dsp_dup_q15(int16_t *p_s16_src, int16_t *p_s16_dst, uint16_t u16_data_num);
void co_dsp_dup_q7(int8_t *p_s8_src, int8_t *p_s8_dst, uint16_t u16_data_num);

void co_dsp_set_q31(int32_t s32_value, int32_t *p_s32_dst, uint16_t u16_data_num);
void co_dsp_set_q15(int16_t s16_value, int16_t *p_s16_dst, uint16_t u16_data_num);
void co_dsp_set_q7(int8_t s8_value, int8_t *p_s8_dst, uint16_t u16_data_num);

void co_dsp_convert_q31_q23(int32_t *p_s32_src, int32_t *p_s32_dst, uint16_t u16_data_num);
void co_dsp_convert_q31_q15(int32_t *p_s32_src, int16_t *p_s16_dst, uint16_t u16_data_num);
void co_dsp_convert_q31_q7(int32_t *p_s32_src, int8_t *p_s8_dst, uint16_t u16_data_num);

void co_dsp_convert_q23_q31(int32_t *p_s32_src, int32_t *p_s32_dst, uint16_t u16_data_num);
void co_dsp_convert_q23_q15(int32_t *p_s32_src, int16_t *p_s16_dst, uint16_t u16_data_num);
void co_dsp_convert_q23_q7(int32_t *p_s32_src, int8_t *p_s8_dst, uint16_t u16_data_num);

void co_dsp_convert_q15_q31(int16_t *p_s16_src, int32_t *p_s32_dst, uint16_t u16_data_num);
void co_dsp_convert_q15_q23(int16_t *p_s16_src, int32_t *p_s32_dst, uint16_t u16_data_num);
void co_dsp_convert_q15_q7(int16_t *p_s16_src, int8_t *p_s8_dst, uint16_t u16_data_num);

void co_dsp_convert_q7_q31(int8_t *p_s8_src, int32_t *p_s32_dst, uint16_t u16_data_num);
void co_dsp_convert_q7_q23(int8_t *p_s8_src, int32_t *p_s32_dst, uint16_t u16_data_num);
void co_dsp_convert_q7_q15(int8_t *p_s8_src, int16_t *p_s16_dst, uint16_t u16_data_num);

#endif /* __CO_DSP_LIB_H__ */
