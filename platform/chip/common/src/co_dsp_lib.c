/*
 * INCLUDE FILES
 **************************************************************************************************
 */
/* hal includes */

/* reg includes */

/* other includes */

/* self includes */
#include "co_dsp_lib.h"

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

/*
 * LOCAL FUNCTION DEFINITIONS
 **************************************************************************************************
 */

/*
 * EXPORTED FUNCTION DEFINITIONS
 **************************************************************************************************
 */
void co_dsp_dup_q31(int32_t *p_s32_src, int32_t *p_s32_dst, uint16_t u16_data_num)
{
#if (defined (D25))

#elif (defined (D10))

#elif (defined (N7))

#elif (defined (BX1))

#else
    for (int i = 0; i < u16_data_num; i++) {
        p_s32_dst[i] = p_s32_src[i];
    }
#endif
}

void co_dsp_dup_q15(int16_t *p_s16_src, int16_t *p_s16_dst, uint16_t u16_data_num)
{
#if (defined (D25))

#elif (defined (D10))

#elif (defined (N7))

#elif (defined (BX1))

#else
    for (int i = 0; i < u16_data_num; i++) {
        p_s16_dst[i] = p_s16_src[i];
    }
#endif
}

void co_dsp_dup_q7(int8_t *p_s8_src, int8_t *p_s8_dst, uint16_t u16_data_num)
{
#if (defined (D25))

#elif (defined (D10))

#elif (defined (N7))

#elif (defined (BX1))

#else
    for (int i = 0; i < u16_data_num; i++) {
        p_s8_dst[i] = p_s8_src[i];
    }
#endif
}

void co_dsp_set_q31(int32_t s32_value, int32_t *p_s32_dst, uint16_t u16_data_num)
{
#if (defined (D25))

#elif (defined (D10))

#elif (defined (N7))

#elif (defined (BX1))

#else
    for (int i = 0; i < u16_data_num; i++) {
        p_s32_dst[i] = s32_value;
    }
#endif
}

void co_dsp_set_q15(int16_t s16_value, int16_t *p_s16_dst, uint16_t u16_data_num)
{
#if (defined (D25))

#elif (defined (D10))

#elif (defined (N7))

#elif (defined (BX1))

#else
    for (int i = 0; i < u16_data_num; i++) {
        p_s16_dst[i] = s16_value;
    }
#endif
}

void co_dsp_set_q7(int8_t s8_value, int8_t *p_s8_dst, uint16_t u16_data_num)
{
#if (defined (D25))

#elif (defined (D10))

#elif (defined (N7))

#elif (defined (BX1))

#else
    for (int i = 0; i < u16_data_num; i++) {
        p_s8_dst[i] = s8_value;
    }
#endif
}

void co_dsp_convert_q31_q23(int32_t *p_s32_src, int32_t *p_s32_dst, uint16_t u16_data_num)
{
    for (int i = 0; i < u16_data_num; i++) {
        p_s32_dst[i] = (int32_t)(p_s32_src[i] >> 8);
    }
}

void co_dsp_convert_q31_q15(int32_t *p_s32_src, int16_t *p_s16_dst, uint16_t u16_data_num)
{
    for (int i = 0; i < u16_data_num; i++) {
        p_s16_dst[i] = (int16_t)(p_s32_src[i] >> 16);
    }
}

void co_dsp_convert_q31_q7(int32_t *p_s32_src, int8_t *p_s8_dst, uint16_t u16_data_num)
{
    for (int i = 0; i < u16_data_num; i++) {
        p_s8_dst[i] = (int8_t)(p_s32_src[i] >> 24);
    }
}

void co_dsp_convert_q23_q31(int32_t *p_s32_src, int32_t *p_s32_dst, uint16_t u16_data_num)
{
    for (int i = (u16_data_num - 1); i >= 0; i--) {
        p_s32_dst[i] = ((int32_t)p_s32_src[i]) << 8;
    }
}

void co_dsp_convert_q23_q15(int32_t *p_s32_src, int16_t *p_s16_dst, uint16_t u16_data_num)
{
    for (int i = 0; i < u16_data_num; i++) {
        p_s16_dst[i] = (int16_t)(p_s32_src[i] >> 8);
    }
}

void co_dsp_convert_q23_q7(int32_t *p_s32_src, int8_t *p_s8_dst, uint16_t u16_data_num)
{
    for (int i = 0; i < u16_data_num; i++) {
        p_s8_dst[i] = (int8_t)(p_s32_src[i] >> 16);
    }
}

void co_dsp_convert_q15_q31(int16_t *p_s16_src, int32_t *p_s32_dst, uint16_t u16_data_num)
{
    for (int i = (u16_data_num - 1); i >= 0; i--) {
        p_s32_dst[i] = ((int32_t)p_s16_src[i]) << 16;
    }
}

void co_dsp_convert_q15_q23(int16_t *p_s16_src, int32_t *p_s32_dst, uint16_t u16_data_num)
{
    for (int i = (u16_data_num - 1); i >= 0; i--) {
        p_s32_dst[i] = ((int32_t)p_s16_src[i]) << 8;
    }
}

void co_dsp_convert_q15_q7(int16_t *p_s16_src, int8_t *p_s8_dst, uint16_t u16_data_num)
{
    for (int i = 0; i < u16_data_num; i++) {
        p_s8_dst[i] = (int8_t)(p_s16_src[i] >> 8);
    }
}

void co_dsp_convert_q7_q31(int8_t *p_s8_src, int32_t *p_s32_dst, uint16_t u16_data_num)
{
    for (int i = (u16_data_num - 1); i >= 0; i--) {
        p_s32_dst[i] = ((int32_t)p_s8_src[i]) << 24;
    }
}

void co_dsp_convert_q7_q23(int8_t *p_s8_src, int32_t *p_s32_dst, uint16_t u16_data_num)
{
    for (int i = (u16_data_num - 1); i >= 0; i--) {
        p_s32_dst[i] = ((int32_t)p_s8_src[i]) << 16;
    }
}

void co_dsp_convert_q7_q15(int8_t *p_s8_src, int16_t *p_s16_dst, uint16_t u16_data_num)
{
    for (int i = (u16_data_num - 1); i >= 0; i--) {
        p_s16_dst[i] = ((int16_t)p_s8_src[i]) << 8;
    }
}
