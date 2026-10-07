/* Assignment 60: integration and sorting checks, with no libc calls. */
#include "IEcoSystem1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoHeapSort.h"
#include "IEcoSortAndSearch.h"
#include <limits.h>

#define ELEMENTS(array) (sizeof(array) / sizeof((array)[0]))

static size_t comparisonCalls;

static int ECOCDECLMETHOD CompareInt(const void* left, const void* right) {
    int a = *(const int*)left;
    int b = *(const int*)right;
    ++comparisonCalls;
    return (a > b) - (a < b);
}

static int ECOCDECLMETHOD CompareIntDescending(const void* left, const void* right) {
    return CompareInt(right, left);
}

static int ECOCDECLMETHOD CompareLong(const void* left, const void* right) {
    long a = *(const long*)left;
    long b = *(const long*)right;
    return (a > b) - (a < b);
}

static int ECOCDECLMETHOD CompareFloat(const void* left, const void* right) {
    float a = *(const float*)left;
    float b = *(const float*)right;
    return (a > b) - (a < b);
}

static int ECOCDECLMETHOD CompareDouble(const void* left, const void* right) {
    double a = *(const double*)left;
    double b = *(const double*)right;
    return (a > b) - (a < b);
}

static int ECOCDECLMETHOD CompareLongDouble(const void* left, const void* right) {
    long double a = *(const long double*)left;
    long double b = *(const long double*)right;
    return (a > b) - (a < b);
}

typedef struct TestRecord {
    int key;
    long id;
    unsigned char payload[11];
} TestRecord;

static int ECOCDECLMETHOD CompareRecord(const void* left, const void* right) {
    int a = ((const TestRecord*)left)->key;
    int b = ((const TestRecord*)right)->key;
    return (a > b) - (a < b);
}

/* A three-byte element exercises sizes that are not multiples of a word. */
typedef struct TestTriplet {
    unsigned char key;
    unsigned char payload[2];
} TestTriplet;

static int ECOCDECLMETHOD CompareTriplet(const void* left, const void* right) {
    unsigned char a = ((const TestTriplet*)left)->key;
    unsigned char b = ((const TestTriplet*)right)->key;
    return (a > b) - (a < b);
}

static int SameBytes(const unsigned char* left, const unsigned char* right, size_t size) {
    size_t index;
    for (index = 0; index < size; ++index) {
        if (left[index] != right[index]) {
            return 0;
        }
    }
    return 1;
}

/* Verify order and the complete byte multiset, including duplicate multiplicity,
 * record payloads and padding. Equal keys may occur in any relative order. */
static int CheckSort(IEcoSortAndSearch* sort, const void* original, void* work,
                     size_t count, size_t size, EcoSortCompare_t compare) {
    const unsigned char* source = (const unsigned char*)original;
    unsigned char* bytes = (unsigned char*)work;
    size_t index;
    size_t other;
    size_t before;
    size_t after;
    for (index = 0; index < count * size; ++index) {
        bytes[index] = source[index];
    }
    sort->pVTbl->Sort(sort, work, count, size, compare);
    for (index = 1; index < count; ++index) {
        if (compare(bytes + (index - 1) * size, bytes + index * size) > 0) {
            return 0;
        }
    }
    for (index = 0; index < count; ++index) {
        before = 0;
        after = 0;
        for (other = 0; other < count; ++other) {
            before += SameBytes(source + index * size, source + other * size, size);
            after += SameBytes(source + index * size, bytes + other * size, size);
        }
        if (before != after) {
            return 0;
        }
    }
    return 1;
}

static int CheckArguments(IEcoSortAndSearch* sort) {
    static const int original[] = {42, -17, 9};
    int work[3];
    size_t index;
    for (index = 0; index < ELEMENTS(work); ++index) {
        work[index] = original[index];
    }
    comparisonCalls = 0;
    sort->pVTbl->Sort(sort, 0, 0, 0, 0);
    sort->pVTbl->Sort(sort, 0, 1, (size_t)-1, CompareInt);
    sort->pVTbl->Sort(sort, work, 0, sizeof(int), CompareInt);
    sort->pVTbl->Sort(sort, work, 1, sizeof(int), CompareInt);
    sort->pVTbl->Sort(sort, work, 1, 0, 0);
    sort->pVTbl->Sort(sort, work, 1, (size_t)-1, CompareInt);
    sort->pVTbl->Sort(sort, 0, 3, sizeof(int), CompareInt);
    sort->pVTbl->Sort(sort, work, 3, 0, CompareInt);
    sort->pVTbl->Sort(sort, work, 3, sizeof(int), 0);
    sort->pVTbl->Sort(sort, work, (size_t)-1 / sizeof(int) + 1,
                      sizeof(int), CompareInt);
    sort->pVTbl->Sort(sort, work, 2, (size_t)-1, CompareInt);
    if (comparisonCalls != 0) {
        return 0;
    }
    return SameBytes((const unsigned char*)original,
                     (const unsigned char*)work, sizeof(work));
}

static int CheckTypes(IEcoSortAndSearch* sort) {
    static const int mixed[] = {INT_MAX, -7, 0, 3, -7, INT_MIN, 3, 2, -1, 0};
    static const int ordered[] = {-9, -3, -3, 0, 2, 7};
    static const int reversed[] = {7, 2, 0, -3, -3, -9};
    static const int equal[] = {5, 5, 5, 5, 5};
    static const int single[] = {-42};
    static const long longs[] = {LONG_MAX, -19L, 0L, LONG_MIN, 19L, -19L};
    static const float floats[] = {3.5f, -0.0f, -2.25f, 0.0f, 3.5f, 0.125f};
    static const double doubles[] = {9.25, -7.5, 0.0, -0.0, 9.25, 0.125};
    static const long double extended[] = {3.125L, -9.5L, 0.0L, -0.0L, 3.125L, 1.25L};
    static const TestTriplet triplets[] = {{2, {9, 8}}, {0, {7, 6}},
        {2, {5, 4}}, {1, {3, 2}}, {0, {1, 0}}};
    int intsWork[10];
    long longsWork[6];
    float floatsWork[6];
    double doublesWork[6];
    long double extendedWork[6];
    TestTriplet tripletsWork[5];
    TestRecord records[9];
    TestRecord recordsWork[9];
    unsigned char* recordBytes = (unsigned char*)records;
    size_t index;
    size_t byte;
    if (!CheckSort(sort, 0, 0, 0, sizeof(int), CompareInt) ||
        !CheckSort(sort, single, intsWork, ELEMENTS(single), sizeof(int), CompareInt) ||
        !CheckSort(sort, mixed, intsWork, ELEMENTS(mixed), sizeof(int), CompareInt) ||
        !CheckSort(sort, mixed, intsWork, ELEMENTS(mixed), sizeof(int), CompareIntDescending) ||
        !CheckSort(sort, ordered, intsWork, ELEMENTS(ordered), sizeof(int), CompareInt) ||
        !CheckSort(sort, reversed, intsWork, ELEMENTS(reversed), sizeof(int), CompareInt) ||
        !CheckSort(sort, equal, intsWork, ELEMENTS(equal), sizeof(int), CompareInt) ||
        !CheckSort(sort, longs, longsWork, ELEMENTS(longs), sizeof(long), CompareLong) ||
        !CheckSort(sort, floats, floatsWork, ELEMENTS(floats), sizeof(float), CompareFloat) ||
        !CheckSort(sort, doubles, doublesWork, ELEMENTS(doubles), sizeof(double), CompareDouble) ||
        !CheckSort(sort, extended, extendedWork, ELEMENTS(extended), sizeof(long double), CompareLongDouble) ||
        !CheckSort(sort, triplets, tripletsWork, ELEMENTS(triplets), sizeof(TestTriplet), CompareTriplet)) {
        return 0;
    }
    /* Initialize padding as well as members so its preservation is observable. */
    for (byte = 0; byte < sizeof(records); ++byte) {
        recordBytes[byte] = (unsigned char)(byte % 251);
    }
    for (index = 0; index < ELEMENTS(records); ++index) {
        records[index].key = (int)(index % 3) - 1;
        records[index].id = (long)index;
        for (byte = 0; byte < sizeof(records[index].payload); ++byte) {
            records[index].payload[byte] = (unsigned char)(index * 11 + byte);
        }
    }
    return CheckSort(sort, records, recordsWork, ELEMENTS(records),
                     sizeof(TestRecord), CompareRecord);
}

static int CheckExhaustive(IEcoSortAndSearch* sort) {
    int original[7];
    int work[7];
    size_t count;
    size_t combinations = 1;
    size_t code;
    size_t value;
    size_t index;
    /* Every array of length 0..7 over {-1, 0, 1}, in both directions. */
    for (count = 0; count <= ELEMENTS(original); ++count) {
        for (code = 0; code < combinations; ++code) {
            value = code;
            for (index = 0; index < count; ++index) {
                original[index] = (int)(value % 3) - 1;
                value /= 3;
            }
            if (!CheckSort(sort, original, work, count, sizeof(int), CompareInt) ||
                !CheckSort(sort, original, work, count, sizeof(int), CompareIntDescending)) {
                return 0;
            }
        }
        combinations *= 3;
    }
    return 1;
}

static int CheckLarge(IEcoSortAndSearch* sort) {
    struct GuardedArray {
        int before;
        int values[1024];
        int after;
    } work;
    size_t histogram[97];
    size_t index;
    size_t pattern;
    int value;
    /* Heap boundaries near powers of two and five larger input patterns. */
    for (index = 2; index <= 65; ++index) {
        size_t item;
        int original[65];
        int smallWork[65];
        for (item = 0; item < index; ++item) {
            original[item] = (int)((item * 73 + index) % 19) - 9;
        }
        if (!CheckSort(sort, original, smallWork, index, sizeof(int), CompareInt)) {
            return 0;
        }
    }
    for (pattern = 0; pattern < 5; ++pattern) {
        work.before = INT_MIN;
        work.after = INT_MAX;
        for (index = 0; index < ELEMENTS(histogram); ++index) {
            histogram[index] = 0;
        }
        for (index = 0; index < ELEMENTS(work.values); ++index) {
            if (pattern == 0) value = (int)(index * 73 % 97) - 48;
            else if (pattern == 1) value = (int)(index * 97 / 1024) - 48;
            else if (pattern == 2) value = 48 - (int)(index * 97 / 1024);
            else if (pattern == 3) value = 7;
            else value = (index % 2 == 0) ? -48 : 48;
            work.values[index] = value;
            ++histogram[value + 48];
        }
        comparisonCalls = 0;
        sort->pVTbl->Sort(sort, work.values, ELEMENTS(work.values), sizeof(int), CompareInt);
        /* A generous O(n log n) bound rejects quadratic implementations. */
        if (comparisonCalls > 4 * 1024 * 11 ||
            work.before != INT_MIN || work.after != INT_MAX) {
            return 0;
        }
        for (index = 0; index < ELEMENTS(work.values); ++index) {
            value = work.values[index];
            if (value < -48 || value > 48 || histogram[value + 48] == 0 ||
                (index != 0 && work.values[index - 1] > value)) {
                return 0;
            }
            --histogram[value + 48];
        }
        for (index = 0; index < ELEMENTS(histogram); ++index) {
            if (histogram[index] != 0) return 0;
        }
    }
    return 1;
}

static int CheckInterfaces(IEcoHeapSort* heap, IEcoSortAndSearch* sort) {
    IEcoUnknown* fromHeap = 0;
    IEcoUnknown* fromSort = 0;
    IEcoHeapSort* roundHeap = 0;
    IEcoSortAndSearch* roundSort = 0;
    void* invalid = heap;
    uint32_t first;
    uint32_t second;
    uint32_t remaining;
    uint32_t restored;
    int ok = 0;
    if (heap->pVTbl->QueryInterface(heap, &IID_IEcoUnknown, (void**)&fromHeap) != 0 ||
        sort->pVTbl->QueryInterface(sort, &IID_IEcoUnknown, (void**)&fromSort) != 0 ||
        fromHeap != (IEcoUnknown*)heap || fromSort != fromHeap ||
        sort->pVTbl->QueryInterface(sort, &IID_IEcoHeapSort, (void**)&roundHeap) != 0 ||
        roundHeap != heap ||
        sort->pVTbl->QueryInterface(sort, &IID_IEcoSortAndSearch, (void**)&roundSort) != 0 ||
        roundSort != sort || (void*)heap == (void*)sort) {
        goto Release;
    }
    if (sort->pVTbl->QueryInterface(sort, &GID_IEcoSystem, &invalid) != ERR_ECO_NOINTERFACE ||
        invalid != 0) {
        goto Release;
    }
    invalid = heap;
    if (sort->pVTbl->QueryInterface(sort, 0, &invalid) != ERR_ECO_POINTER || invalid != 0 ||
        sort->pVTbl->QueryInterface(sort, &IID_IEcoUnknown, 0) != ERR_ECO_POINTER) {
        goto Release;
    }
    first = heap->pVTbl->AddRef(heap);
    second = sort->pVTbl->AddRef(sort);
    remaining = heap->pVTbl->Release(heap);
    restored = sort->pVTbl->Release(sort);
    ok = second == first + 1 && remaining == first && restored == first - 1;
Release:
    if (roundSort != 0) roundSort->pVTbl->Release(roundSort);
    if (roundHeap != 0) roundHeap->pVTbl->Release(roundHeap);
    if (fromSort != 0) fromSort->pVTbl->Release(fromSort);
    if (fromHeap != 0) fromHeap->pVTbl->Release(fromHeap);
    return ok;
}

int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = ERR_ECO_POINTER;
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoHeapSort* pIHeap = 0;
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
        if (pass == 0) {
            result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoHeapSort, 0,
                                                &IID_IEcoHeapSort, (void**)&pIHeap);
            if (result != 0 || pIHeap == 0) {
                if (result == 0) result = ERR_ECO_FAIL;
                goto Release;
            }
            result = pIHeap->pVTbl->QueryInterface(pIHeap, &IID_IEcoSortAndSearch, (void**)&pISort);
        }
        else {
            /* Request the secondary interface directly through the factory. */
            result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoHeapSort, 0,
                                                &IID_IEcoSortAndSearch, (void**)&pISort);
            if (result != 0 || pISort == 0) {
                if (result == 0) result = ERR_ECO_FAIL;
                goto Release;
            }
            result = pISort->pVTbl->QueryInterface(pISort, &IID_IEcoHeapSort, (void**)&pIHeap);
        }
        if (result != 0 || pIHeap == 0 || pISort == 0 ||
            !CheckInterfaces(pIHeap, pISort) || !CheckArguments(pISort) ||
            !CheckTypes(pISort) || !CheckExhaustive(pISort) || !CheckLarge(pISort)) {
            result = ERR_ECO_FAIL;
            goto Release;
        }
        /* On the second instance the legacy interface must survive Sort.Release. */
        if (pass == 1) {
            pISort->pVTbl->Release(pISort);
            pISort = 0;
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
        if (pISort != 0) {
            /* On the first instance Sort must survive Heap.Release and delete it. */
            if (!CheckTypes(pISort)) {
                result = ERR_ECO_FAIL;
                goto Release;
            }
            if (pISort->pVTbl->Release(pISort) != 0) {
                pISort = 0;
                result = ERR_ECO_FAIL;
                goto Release;
            }
            pISort = 0;
        }
    }
    result = ERR_ECO_SUCCESS;

Release:
    if (pISort != 0) {
        pISort->pVTbl->Release(pISort);
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
