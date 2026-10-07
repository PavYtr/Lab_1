/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   CEcoHeapSort_D8D41318
 * </summary>
 *
 * <description>
 *   This header describes the implementation of the CEcoHeapSort_D8D41318 component
 * </description>
 *
 * <author>
 *   Copyright (c) 2018 PavYtr. All rights reserved.
 * </author>
 *
 */

#ifndef __C_ECOHEAPSORT_H__
#define __C_ECOHEAPSORT_H__

#include "IEcoHeapSort.h"
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"

typedef struct CEcoHeapSort_D8D41318* CEcoHeapSort_D8D41318Ptr_t;

typedef struct CEcoHeapSort_D8D41318 {

    /* IEcoHeapSort interface function table */
    IEcoHeapSortVTbl* m_pVTblIEcoHeapSort;


    /* Instance initialization */
    int16_t (ECOCALLMETHOD *Init)(/*in*/ CEcoHeapSort_D8D41318Ptr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem);
    /* Instance creation */
    int16_t (ECOCALLMETHOD *Create)(/*in*/ CEcoHeapSort_D8D41318Ptr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter);
    /* Deletion */
    void (ECOCALLMETHOD *Delete)(/*in*/ CEcoHeapSort_D8D41318Ptr_t pCMe);


    /* Reference counter */
    uint32_t m_cRef;

    /* Interface for memory operations */
    IEcoMemoryAllocator1* m_pIMem;

    /* System interface */
    IEcoSystem1* m_pISys;

    /* Instance data */
    char_t* m_Name;

} CEcoHeapSort_D8D41318;

#endif /* __C_ECOHEAPSORT_H__ */
