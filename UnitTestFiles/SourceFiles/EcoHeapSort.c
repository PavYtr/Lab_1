/* Smoke client for the preparation stage; no sorting algorithm is tested yet. */
#include "IEcoSystem1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoHeapSort.h"
#include "IEcoSortAndSearch.h"

int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = ERR_ECO_POINTER;
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoHeapSort* pIHeap = 0;
    IEcoUnknown* pIdentity = 0;
    IEcoSortAndSearch* pISort = 0;
    char_t name[] = "EcoHeapSort";
    char_t* copy = 0;
    size_t index = 0;
    uint32_t pass = 0;

    if (pIUnk == 0) {
        return result;
    }
    result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void**)&pISys);
    if (result != 0 || pISys == 0) {
        if (result == 0) result = ERR_ECO_FAIL;
        goto Release;
    }
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void**)&pIBus);
    if (result != 0 || pIBus == 0) {
        if (result == 0) result = ERR_ECO_FAIL;
        goto Release;
    }
#ifdef ECO_LIB
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoHeapSort,
        (IEcoUnknown*)GetIEcoComponentFactoryPtr_A3A5BD4760D34C1691719DF4D8D41318);
    if (result != 0) {
        goto Release;
    }
#endif
    /* Recreate the component to exercise ownership and the factory reference count. */
    for (pass = 0; pass < 2; ++pass) {
        result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoHeapSort, 0,
                                            &IID_IEcoHeapSort, (void**)&pIHeap);
        if (result != 0 || pIHeap == 0) {
            if (result == 0) result = ERR_ECO_FAIL;
            goto Release;
        }
        result = pIHeap->pVTbl->QueryInterface(pIHeap, &IID_IEcoUnknown, (void**)&pIdentity);
        if (result != 0 || pIdentity != (IEcoUnknown*)pIHeap) {
            result = ERR_ECO_FAIL;
            goto Release;
        }
        pIdentity->pVTbl->Release(pIdentity);
        pIdentity = 0;

        /* Only the public contract exists in this stage. Do not expose a fake Sort. */
        result = pIHeap->pVTbl->QueryInterface(pIHeap, &IID_IEcoSortAndSearch, (void**)&pISort);
        if (result != ERR_ECO_NOINTERFACE || pISort != 0) {
            result = ERR_ECO_FAIL;
            goto Release;
        }
        result = pIHeap->pVTbl->MyFunction(pIHeap, name, &copy);
        if (result != 0 || copy == 0) {
            if (result == 0) result = ERR_ECO_FAIL;
            goto Release;
        }
        for (index = 0; index < sizeof(name); ++index) {
            if (copy[index] != name[index]) {
                result = ERR_ECO_FAIL;
                goto Release;
            }
        }
        result = pIHeap->pVTbl->MyFunction(pIHeap, copy, &copy);
        if (result != 0 || copy == 0) {
            if (result == 0) result = ERR_ECO_FAIL;
            goto Release;
        }
        for (index = 0; index < sizeof(name); ++index) {
            if (copy[index] != name[index]) {
                result = ERR_ECO_FAIL;
                goto Release;
            }
        }
        pIHeap->pVTbl->Release(pIHeap);
        pIHeap = 0;
    }
    result = ERR_ECO_SUCCESS;

Release:
    if (pISort != 0) {
        pISort->pVTbl->Release(pISort);
    }
    if (pIdentity != 0) {
        pIdentity->pVTbl->Release(pIdentity);
    }
    if (pIHeap != 0) {
        pIHeap->pVTbl->Release(pIHeap);
    }
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }
    return result;
}
