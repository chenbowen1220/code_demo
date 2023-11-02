/*
 * @Author: chenbw 1069381755@qq.com
 * @Date: 2023-11-02 23:16:13
 * @LastEditors: chenbw 1069381755@qq.com
 * @LastEditTime: 2023-11-02 23:56:25
 * @FilePath: \code_demo\platform\chip\hal\include\hal_def.h
 * @Description: 
 * 
 * Copyright (c) 2023 by chenbw, All Rights Reserved. 
 */
#ifndef __HAL_DEF_H__
#define __HAL_DEF_H__

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;
typedef char                int8_t;
typedef short               int16_t;
typedef int                 int32_t;
typedef long long           int64_t;

typedef void*(hal_mem_malloc)(uint32_t u32_size);
typedef void(hal_mem_free)(void *p_v_handle);
typedef void(hal_callback)(void *p_v_handle);

#ifndef ALIGN
#define ALIGN(val, exp)     (((val) + ((exp) - 1)) & ~((exp) - 1))
#endif

#ifndef BITSET
#define BITSET
#define BIT0                (0x01ul)
#define BIT1                (0x02ul)
#define BIT2                (0x04ul)
#define BIT3                (0x08ul)
#define BIT4                (0x10ul)
#define BIT5                (0x20ul)
#define BIT6                (0x40ul)
#define BIT7                (0x80ul)

#define BIT8                (0x0100ul)
#define BIT9                (0x0200ul)
#define BIT10               (0x0400ul)
#define BIT11               (0x0800ul)
#define BIT12               (0x1000ul)
#define BIT13               (0x2000ul)
#define BIT14               (0x4000ul)
#define BIT15               (0x8000ul)

#define BIT16               (0x010000ul)
#define BIT17               (0x020000ul)
#define BIT18               (0x040000ul)
#define BIT19               (0x080000ul)
#define BIT20               (0x100000ul)
#define BIT21               (0x200000ul)
#define BIT22               (0x400000ul)
#define BIT23               (0x800000ul)

#define BIT24               (0x01000000ul)
#define BIT25               (0x02000000ul)
#define BIT26               (0x04000000ul)
#define BIT27               (0x08000000ul)
#define BIT28               (0x10000000ul)
#define BIT29               (0x20000000ul)
#define BIT30               (0x40000000ul)
#define BIT31               (0x80000000ul)

#define CO_BIT(pos)         (1UL << (pos))
#endif

#ifndef NULL
#define NULL                0
#endif

#define KB                  (1024)
#define MB                  (1024 * 1024)
#define GB                  (1024 * 1024 * 1024)

#endif /* __HAL_DEF_H__ */