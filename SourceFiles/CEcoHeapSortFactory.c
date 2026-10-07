/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   CEcoHeapSort_D8D41318Factory
 * </summary>
 *
 * <description>
 *   This header describes the implementation of the factory for the component
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
#include "CEcoHeapSortFactory.h"

extern CEcoHeapSort_D8D41318 g_xCEcoHeapSort_D8D41318;

/*
 *
 * <summary>
 *   QueryInterface function
 * </summary>
 *
 * <description>
 *   The function returns a pointer to the interface
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CEcoHeapSort_D8D41318Factory_QueryInterface(IEcoComponentFactory* me, const UGUID* riid, void** ppv) {
    if (ppv == 0) {
        return ERR_ECO_POINTER;
    }
    *ppv = 0;
    if (me == 0 || riid == 0) {
        return ERR_ECO_POINTER;
    }
    if ( IsEqualUGUID(riid, &IID_IEcoUnknown) || IsEqualUGUID(riid, &IID_IEcoComponentFactory) ) {
        *ppv = me;
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }
    ((IEcoUnknown*)(*ppv))->pVTbl->AddRef((IEcoUnknown*)*ppv);

    return ERR_ECO_SUCCESS;
}

/*
 *
 * <summary>
 *   AddRef function
 * </summary>
 *
 * <description>
 *   The function increments the reference count for the interface
 * </description>
 *
 */
static uint32_t ECOCALLMETHOD CEcoHeapSort_D8D41318Factory_AddRef(/* in */ IEcoComponentFactory* me) {
    CEcoHeapSort_D8D41318Factory* pCMe = (CEcoHeapSort_D8D41318Factory*)me;

    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    return ++pCMe->m_cRef;
}

/*
 *
 * <summary>
 *   Release function
 * </summary>
 *
 * <description>
 *   The function decrements the reference count for the interface
 * </description>
 *
 */
static uint32_t ECOCALLMETHOD CEcoHeapSort_D8D41318Factory_Release(/* in */ IEcoComponentFactory* me) {
    CEcoHeapSort_D8D41318Factory* pCMe = (CEcoHeapSort_D8D41318Factory*)me;

    if (me == 0 ) {
        return -1; /* ERR_ECO_POINTER */
    }

    /* Decrementing the component reference counter */
    --pCMe->m_cRef;

    /* If the counter is zeroed, free the instance data */
    if ( pCMe->m_cRef == 0 ) {
        return 0;
    }
    return pCMe->m_cRef;
}

/*
 *
 * <summary>
 *   Init function
 * </summary>
 *
 * <description>
 *   The function initializes the component with parameters
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CEcoHeapSort_D8D41318Factory_Init(/* in */ struct IEcoComponentFactory* me, /* in */ struct IEcoUnknown *pIUnkSystem, /* in */ void* pv) {
    int16_t result = ERR_ECO_POINTER;

    if (me == 0 ) {
        return result;
    }

    /* Initializing the component with parameters */

    return result;
}

/*
 *
 * <summary>
 *   Alloc function
 * </summary>
 *
 * <description>
 *   The function creates a component
 * </description>
 *
 */
static int16_t ECOCALLMETHOD CEcoHeapSort_D8D41318Factory_Alloc(/* in */ struct IEcoComponentFactory* me, /* in */ struct IEcoUnknown *pISystem, /* in */ struct IEcoUnknown *pIUnknownOuter, /* in */ const UGUID* riid, /* out */ void** ppv) {
    IEcoUnknown* pIUnk = 0;
    int16_t result = ERR_ECO_POINTER;
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoInterfaceBus1MemExt* pIMemExt = 0;
    IEcoMemoryAllocator1* pIMem = 0;
    CEcoHeapSort_D8D41318* pCObj = 0;
    UGUID* rcid = (UGUID*)&CID_EcoMemoryManager1;

    if (ppv == 0) {
        return ERR_ECO_POINTER;
    }
    *ppv = 0;
    if (me == 0 || pISystem == 0 || riid == 0) {
        return result; /* ERR_ECO_POINTER */
    }

    /* This preparation-stage component does not implement aggregation. */
    if (pIUnknownOuter != 0) {
        /* aggregation not supported */
        return ERR_ECO_NOAGGREGATION;
    }

    /* Getting the application system interface */
    result = pISystem->pVTbl->QueryInterface(pISystem, &GID_IEcoSystem, (void **)&pISys);
    /* Check */
    if (result != 0 || pISys == 0) {
        return ERR_ECO_NOSYSTEM;
    }

    /* Getting the interface for working with the interface bus */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    /* Check */
    if (result != 0 || pIBus == 0) {
        pISys->pVTbl->Release(pISys);
        return ERR_ECO_NOBUS;
    }

    /* Getting the component ID for memory operations */
    result = pIBus->pVTbl->QueryInterface(pIBus, &IID_IEcoInterfaceBus1MemExt, (void**)&pIMemExt);
    if (result == 0 && pIMemExt != 0) {
        rcid = (UGUID*)pIMemExt->pVTbl->get_Manager(pIMemExt);
        pIMemExt->pVTbl->Release(pIMemExt);
    }

    /* Getting the memory allocator interface */
    result = pIBus->pVTbl->QueryComponent(pIBus, rcid, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);
    /* Check */
    if (result != 0 || pIMem == 0) {
        /* Freeing in case of an error */
        pIBus->pVTbl->Release(pIBus);
        pISys->pVTbl->Release(pISys);
        return ERR_ECO_GET_MEMORY_ALLOCATOR;
    }

    /* Allocating memory for instance data */
    pCObj = (CEcoHeapSort_D8D41318*)pIMem->pVTbl->Alloc(pIMem, sizeof(CEcoHeapSort_D8D41318));
    if (pCObj == 0) {
        pIMem->pVTbl->Release(pIMem);
        /* Freeing in case of an error */
        pIBus->pVTbl->Release(pIBus);
        pISys->pVTbl->Release(pISys);
        return ERR_ECO_OUTOFMEMORY;
    }

    /* Forming instance data */
    pCObj = (CEcoHeapSort_D8D41318*)pIMem->pVTbl->Copy(pIMem, pCObj, &g_xCEcoHeapSort_D8D41318, sizeof(CEcoHeapSort_D8D41318));

    /* Component creation */
    pCObj->Create(pCObj, pISystem, pIUnknownOuter);

    /* Component initialization */
    result = pCObj->Init(pCObj, pISystem);
    if (result != 0) {
        if (pCObj->m_pISys != 0) {
            pCObj->m_pISys->pVTbl->Release(pCObj->m_pISys);
        }
        if (pCObj->m_pIMem != 0) {
            pCObj->m_pIMem->pVTbl->Release(pCObj->m_pIMem);
        }
        pIMem->pVTbl->Free(pIMem, pCObj);
        pIMem->pVTbl->Release(pIMem);
        pIBus->pVTbl->Release(pIBus);
        pISys->pVTbl->Release(pISys);
        return result;
    }
    pIMem->pVTbl->Release(pIMem);
    pIBus->pVTbl->Release(pIBus);
    pISys->pVTbl->Release(pISys);


    /* Getting a pointer to the interface */
    pIUnk = (IEcoUnknown*)pCObj;
    result = pIUnk->pVTbl->QueryInterface(pIUnk, riid, ppv);

    /* Decrementing the reference requested by the Component Factory */
    pIUnk->pVTbl->Release(pIUnk);

    return result;
}
/*
 *
 * <summary>
 *   get_Name function
 * </summary>
 *
 * <description>
 *   The function returns the name of the component
 * </description>
 *
 */
static char_t* ECOCALLMETHOD CEcoHeapSort_D8D41318Factory_get_Name(/* in */ struct IEcoComponentFactory* me) {
    CEcoHeapSort_D8D41318Factory* pCMe = (CEcoHeapSort_D8D41318Factory*)me;

    if (me == 0 ) {
        return 0; /* ERR_ECO_POINTER */
    }

    return pCMe->m_Name;
}

/*
 *
 * <summary>
 *   get_Version function
 * </summary>
 *
 * <description>
 *   The function returns the version of the component
 * </description>
 *
 */
static char_t* ECOCALLMETHOD CEcoHeapSort_D8D41318Factory_get_Version(/* in */ struct IEcoComponentFactory* me) {
    CEcoHeapSort_D8D41318Factory* pCMe = (CEcoHeapSort_D8D41318Factory*)me;

    if (me == 0 ) {
        return 0; /* ERR_ECO_POINTER */
    }

    return pCMe->m_Version;
}

/*
 *
 * <summary>
 *   get_Manufacturer function
 * </summary>
 *
 * <description>
 *   The function returns the name of the component manufacturer
 * </description>
 *
 */
static char_t* ECOCALLMETHOD CEcoHeapSort_D8D41318Factory_get_Manufacturer(/* in */ struct IEcoComponentFactory* me) {
    CEcoHeapSort_D8D41318Factory* pCMe = (CEcoHeapSort_D8D41318Factory*)me;

    if (me == 0 ) {
        return 0; /* ERR_ECO_POINTER */
    }

    return pCMe->m_Manufacturer;
}

/* Create Virtual Table */
IEcoComponentFactoryVTbl g_xA3A5BD4760D34C1691719DF4D8D41318FactoryVTbl = {
    CEcoHeapSort_D8D41318Factory_QueryInterface,
    CEcoHeapSort_D8D41318Factory_AddRef,
    CEcoHeapSort_D8D41318Factory_Release,
    CEcoHeapSort_D8D41318Factory_Alloc,
    CEcoHeapSort_D8D41318Factory_Init,
    CEcoHeapSort_D8D41318Factory_get_Name,
    CEcoHeapSort_D8D41318Factory_get_Version,
    CEcoHeapSort_D8D41318Factory_get_Manufacturer
};

/*
 *
 * <summary>
 *   Create function
 * </summary>
 *
 * <description>
 *   The function 
 * </description>
 *
 */
CEcoHeapSort_D8D41318Factory g_xA3A5BD4760D34C1691719DF4D8D41318Factory = {
    &g_xA3A5BD4760D34C1691719DF4D8D41318FactoryVTbl,
    0,
    "EcoHeapSort\0",
    "1.0.0.0\0",
    "\0"
};

#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr() {
    return (IEcoComponentFactory*)&g_xA3A5BD4760D34C1691719DF4D8D41318Factory;
};
#elif ECO_LIB
IEcoComponentFactory* GetIEcoComponentFactoryPtr_A3A5BD4760D34C1691719DF4D8D41318 = (IEcoComponentFactory*)&g_xA3A5BD4760D34C1691719DF4D8D41318Factory;
#endif
