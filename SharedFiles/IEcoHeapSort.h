/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   IEcoHeapSort
 * </summary>
 *
 * <description>
 *   This header describes the interface IEcoHeapSort
 * </description>
 *
 * <reference>
 *
 * </reference>
 *
 * <author>
 *   Copyright (c) 2026 PavYtr. All rights reserved.
 * </author>
 *
 */

#ifndef __I_ECOHEAPSORT_H__
#define __I_ECOHEAPSORT_H__

#include "IEcoBase1.h"

/* IEcoHeapSort IID = {2245125D-B011-4035-9FCA-43A4406E50B5} */
#ifndef __IID_IEcoHeapSort
static const UGUID IID_IEcoHeapSort = {0x01, 0x10, {0x22, 0x45, 0x12, 0x5D, 0xB0, 0x11, 0x40, 0x35, 0x9F, 0xCA, 0x43, 0xA4, 0x40, 0x6E, 0x50, 0xB5}};
#endif /* __IID_IEcoHeapSort */

typedef struct IEcoHeapSort* IEcoHeapSortPtr_t;

typedef struct IEcoHeapSortVTbl {

    /* IEcoUnknown */
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoHeapSortPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoHeapSortPtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoHeapSortPtr_t me);

    /* IEcoHeapSort */
    int16_t (ECOCALLMETHOD *MyFunction)(/* in */ IEcoHeapSortPtr_t me, /* in */ char_t* Name, /* out */ char_t** CopyName);

} IEcoHeapSortVTbl, *IEcoHeapSortVTblPtr_t;

interface IEcoHeapSort {
    struct IEcoHeapSortVTbl *pVTbl;
} IEcoHeapSort;


#endif /* __I_ECOHEAPSORT_H__ */

