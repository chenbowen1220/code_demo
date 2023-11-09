/*
 * @Author: chenbw 1069381755@qq.com
 * @Date: 2023-11-09 22:16:20
 * @LastEditors: chenbw 1069381755@qq.com
 * @LastEditTime: 2023-11-09 22:20:48
 * @FilePath: \code_demo\platform\module\fml\include\fml_misc.h
 * @Description: 
 * 
 * Copyright (c) 2023 by chenbw, All Rights Reserved. 
 */
#ifndef __FML_MISC_H__
#define __FML_MISC_H__

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
void fml_misc_dsd_to_pcm(uint8_t *p_u8_dsd_data, uint16_t u16_dsd_size, int16_t *p_s16_pcm_out, uint16_t u16_pcm_size);

#endif /* __FML_MISC_H__ */
