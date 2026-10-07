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
 *   This source code describes the implementation of the interfaces for CEcoHeapSort_D8D41318
 * </description>
 *
 * <author>
 *   Copyright (c) 2026 PavYtr. All rights reserved.
 * </author>
 *
 */


#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoHeapSort.h"
#include <stddef.h>
/*
 *
 * <summary>
 *   QueryInterface Function
 * </summary>
 *
 * <description>
 *   QueryInterface function for the IEcoHeapSort interface
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CEcoHeapSort_D8D41318_QueryInterface(/* in */ IEcoHeapSortPtr_t me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoHeapSort_D8D41318* pCMe = (CEcoHeapSort_D8D41318*)me;

    /* Pointer Validation */
    if (ppv == 0) {
        return ERR_ECO_POINTER;
    }
    *ppv = 0;
    if (me == 0 || riid == 0) {
        return ERR_ECO_POINTER;
    }

    /* Validate and retrieve requested interface */
    if ( IsEqualUGUID(riid, &IID_IEcoHeapSort) ) {
        *ppv = &pCMe->m_pVTblIEcoHeapSort;
        pCMe->m_pVTblIEcoHeapSort->AddRef((IEcoHeapSort*)pCMe);
    }
	
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblIEcoHeapSort;
        pCMe->m_pVTblIEcoHeapSort->AddRef((IEcoHeapSort*)pCMe);
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }
    return ERR_ECO_SUCCESS;
}

/*
 *
 * <summary>
 *   AddRef Function
 * </summary>
 *
 * <description>
 *   AddRef function for the IEcoHeapSort interface
 * </description>
 *
 */
static uint32_t ECOCALLMETHOD CEcoHeapSort_D8D41318_AddRef(/* in */ IEcoHeapSortPtr_t me) {
    CEcoHeapSort_D8D41318* pCMe = (CEcoHeapSort_D8D41318*)me;

    /* Pointer Validation */
    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    return ++pCMe->m_cRef;
}

/*
 *
 * <summary>
 *   Release Function
 * </summary>
 *
 * <description>
 *   Release function for the IEcoHeapSort interface
 * </description>
 *
 */
static uint32_t ECOCALLMETHOD CEcoHeapSort_D8D41318_Release(/* in */ IEcoHeapSortPtr_t me) {
    CEcoHeapSort_D8D41318* pCMe = (CEcoHeapSort_D8D41318*)me;

    /* Pointer Validation */
    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    /* Decrementing the component's reference count */
    --pCMe->m_cRef;
    /* If the count is zero, free the instance data */
    if ( pCMe->m_cRef == 0 ) {
        pCMe->Delete(pCMe);
		
        return 0;
    }
    return pCMe->m_cRef;
}

/*
 *
 * <summary>
 *   MyFunction Function
 * </summary>
 *
 * <description>
 *   Function
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CEcoHeapSort_D8D41318_MyFunction(/* in */ IEcoHeapSortPtr_t me, /* in */ char_t* Name, /* out */ char_t** copyName) {
    CEcoHeapSort_D8D41318* pCMe = (CEcoHeapSort_D8D41318*)me;
    size_t index = 0;
    char_t* newName = 0;

    /* Pointer Validation */
    if (me == 0 || Name == 0 || copyName == 0) {
        return ERR_ECO_POINTER;
    }

    *copyName = 0;
    while (Name[index] != 0) {
        /* Eco allocator accepts a 32-bit size, including the terminator. */
        if (index == (size_t)((uint32_t)-1) - 1) {
            return ERR_ECO_INVALIDARG;
        }
        ++index;
    }
    newName = (char_t*)pCMe->m_pIMem->pVTbl->Alloc(pCMe->m_pIMem, (uint32_t)(index + 1));
    if (newName == 0) {
        return ERR_ECO_OUTOFMEMORY;
    }
    index = 0;
    while (Name[index] != 0) {
        newName[index] = Name[index];
        ++index;
    }
    newName[index] = 0;
    /* Copy first: Name may point to the previous component-owned copy. */
    if (pCMe->m_Name != 0) {
        pCMe->m_pIMem->pVTbl->Free(pCMe->m_pIMem, pCMe->m_Name);
    }
    pCMe->m_Name = newName;
    *copyName = newName;

    return ERR_ECO_SUCCESS;
}




/*
 *
 * <summary>
 *   Init Function
 * </summary>
 *
 * <description>
 *   Instance initialization function
 * </description>
 *
 */
static int16_t ECOCALLMETHOD initCEcoHeapSort_D8D41318(/*in*/ CEcoHeapSort_D8D41318Ptr_t me, /* in */ IEcoUnknownPtr_t pIUnkSystem) {
    CEcoHeapSort_D8D41318* pCMe = (CEcoHeapSort_D8D41318*)me;
    IEcoInterfaceBus1* pIBus = 0;

    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    int16_t result = ERR_ECO_POINTER;
    UGUID* rcid = (UGUID*)&CID_EcoMemoryManager1;	

    /* Pointer Validation */
    if (me == 0 || pIUnkSystem == 0) {
        return result;
    }

    /* Storing the pointer to the system interface */
    result = pIUnkSystem->pVTbl->QueryInterface(pIUnkSystem, &GID_IEcoSystem, (void**)&pCMe->m_pISys);
    if (result != 0 || pCMe->m_pISys == 0) {
        return ERR_ECO_NOSYSTEM;
    }

    /* Getting the interface for working with the interface bus */
    result = pCMe->m_pISys->pVTbl->QueryInterface(pCMe->m_pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
        return ERR_ECO_NOBUS;
    }

    /* Getting the component ID for working with memory */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        rcid = (UGUID*)pIMemExt->pVTbl->get_Manager(pIMemExt);
        pIMemExt->pVTbl->Release(pIMemExt);
    }

    /* Getting the memory allocator interface */
    result = pIBus->pVTbl->QueryComponent(pIBus, rcid, 0, &IID_IEcoMemoryAllocator1, (void**) &pCMe->m_pIMem);
    /* Check */
    if (result != 0 || pCMe->m_pIMem == 0) {
        result = ERR_ECO_GET_MEMORY_ALLOCATOR;
    }



    /* Freeing */
    pIBus->pVTbl->Release(pIBus);

    return result;
}

/*
 *
 * <summary>
 *   Create Function
 * </summary>
 *
 * <description>
 *   Instance creation function
 * </description>
 *
 */
static int16_t ECOCALLMETHOD createCEcoHeapSort_D8D41318(/* in */ CEcoHeapSort_D8D41318Ptr_t pCMe, /* in */ IEcoUnknownPtr_t pIUnkSystem, /* in */ IEcoUnknownPtr_t pIUnkOuter) {
    int16_t result = ERR_ECO_POINTER;

    /* Pointer Validation */
    if (pCMe == 0) {
        return result; /* ERR_ECO_POINTER */
    }



    return ERR_ECO_SUCCESS;
}

/*
 *
 * <summary>
 *   Delete Function
 * </summary>
 *
 * <description>
 *   Instance freeing function
 * </description>
 *
 */
static void ECOCALLMETHOD deleteCEcoHeapSort_D8D41318(/* in */ CEcoHeapSort_D8D41318Ptr_t pCMe) {
    IEcoMemoryAllocator1* pIMem = 0;

    if (pCMe != 0 ) {
        pIMem = pCMe->m_pIMem;
        /* Freeing */
        if ( pCMe->m_Name != 0 ) {
            pIMem->pVTbl->Free(pIMem, pCMe->m_Name);
        }
        if ( pCMe->m_pISys != 0 ) {
            pCMe->m_pISys->pVTbl->Release(pCMe->m_pISys);
        }
        pIMem->pVTbl->Free(pIMem, pCMe);
        pIMem->pVTbl->Release(pIMem);
    }
}

/* IEcoHeapSort Virtual Table */
IEcoHeapSortVTbl g_x2245125DB01140359FCA43A4406E50B5VTbl_D8D41318 = {
    CEcoHeapSort_D8D41318_QueryInterface,
    CEcoHeapSort_D8D41318_AddRef,
    CEcoHeapSort_D8D41318_Release,
    CEcoHeapSort_D8D41318_MyFunction
};



/* Object Instance */
CEcoHeapSort_D8D41318 g_xCEcoHeapSort_D8D41318 = {
    &g_x2245125DB01140359FCA43A4406E50B5VTbl_D8D41318,
   
    initCEcoHeapSort_D8D41318,
    createCEcoHeapSort_D8D41318,
    deleteCEcoHeapSort_D8D41318,
    1, /* m_cRef */
    0, /* m_pISys */
    0, /* m_pISys */
    0  /* m_Name */
};
