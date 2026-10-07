/* Public sorting service contract for assignment 60; implementation follows later. */
#ifndef __I_ECO_SORT_AND_SEARCH_H__
#define __I_ECO_SORT_AND_SEARCH_H__

#include "IEcoBase1.h"
#include <stddef.h> /* size_t only; no libc functions are used. */

/* IEcoSortAndSearch IID = {98820B3D-4DFA-4F1E-B510-18C91A8281F4} */
static const UGUID IID_IEcoSortAndSearch = {0x01, 0x10, {0x98, 0x82, 0x0B, 0x3D, 0x4D, 0xFA, 0x4F, 0x1E, 0xB5, 0x10, 0x18, 0xC9, 0x1A, 0x82, 0x81, 0xF4}};

typedef struct IEcoSortAndSearch* IEcoSortAndSearchPtr_t;

/* Same callback type as qsort: negative / zero / positive means less / equal / greater.
 * C calling convention is intentional; the callback has no Eco 'me' argument. */
typedef int (ECOCDECLMETHOD *EcoSortCompare_t)(const void* left, const void* right);

typedef struct IEcoSortAndSearchVTbl {
    /* IEcoUnknown: these three slots must come first. */
    int16_t (ECOCALLMETHOD *QueryInterface)(IEcoSortAndSearchPtr_t me, const UGUID* riid, voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(IEcoSortAndSearchPtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(IEcoSortAndSearchPtr_t me);

    /* qsort prototype plus the required Eco receiver.
     * Sort in place; the caller owns base and supplies a consistent comparison.
     * count < 2: no access to base and no callback calls (base/compare may be 0).
     * Otherwise base/compare must be non-null, size > 0, count*size must fit size_t,
     * and base must reference a writable, suitably aligned array of that extent.
     * No stability guarantee. No retained array/callback pointers.
     * Invalid arguments: return without changing the array; void has no error code.
     * me must always be a valid acquired interface pointer. */
    void (ECOCALLMETHOD *Sort)(IEcoSortAndSearchPtr_t me, void* base,
                             size_t count, size_t size, EcoSortCompare_t compare);
} IEcoSortAndSearchVTbl, *IEcoSortAndSearchVTblPtr_t;

interface IEcoSortAndSearch {
    struct IEcoSortAndSearchVTbl* pVTbl;
} IEcoSortAndSearch;

#endif /* __I_ECO_SORT_AND_SEARCH_H__ */
