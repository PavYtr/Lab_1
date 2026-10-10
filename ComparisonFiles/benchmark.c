/* Separate reference experiment: libc is intentionally used only in this folder.
 * The tested Heapsort comes from the production Eco archive and public interface. */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "IEcoSystem1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoHeapSort.h"
#include "IEcoSortAndSearch.h"

#define SAMPLES 9
#define COUNT(a) (sizeof(a) / sizeof((a)[0]))

typedef struct Record {
    int key;
    uint32_t id;
    unsigned char payload[24];
} Record;

#define COMPARATOR(type, name) \
static int name(const void* left, const void* right) { \
    type a = *(const type*)left; \
    type b = *(const type*)right; \
    return (a > b) - (a < b); \
}
COMPARATOR(int, CompareInt)
COMPARATOR(long, CompareLong)
COMPARATOR(float, CompareFloat)
COMPARATOR(double, CompareDouble)
COMPARATOR(long double, CompareLongDouble)

static int CompareRecord(const void* left, const void* right) {
    int a = ((const Record*)left)->key;
    int b = ((const Record*)right)->key;
    return (a > b) - (a < b);
}

typedef struct TypeSpec {
    const char* name;
    size_t size;
    EcoSortCompare_t compare;
} TypeSpec;

static const TypeSpec types[] = {
    {"int", sizeof(int), CompareInt},
    {"long", sizeof(long), CompareLong},
    {"float", sizeof(float), CompareFloat},
    {"double", sizeof(double), CompareDouble},
    {"long_double", sizeof(long double), CompareLongDouble},
    {"record", sizeof(Record), CompareRecord}
};
static const char* patterns[] = {"random", "sorted", "reversed", "duplicates", "equal"};
static const size_t sizes[] = {256, 1024, 4096, 16384, 65536, 262144, 1048576};

static uint64_t Random(uint64_t* state) {
    *state ^= *state << 13;
    *state ^= *state >> 7;
    *state ^= *state << 17;
    return *state;
}

static void Generate(void* base, size_t count, size_t type, size_t pattern, uint64_t seed) {
    memset(base, 0, count * types[type].size);
    for (size_t i = 0; i < count; ++i) {
        int key;
        if (pattern == 0) key = (int)(Random(&seed) % 2000001) - 1000000;
        else if (pattern == 1) key = (int)i - (int)(count / 2);
        else if (pattern == 2) key = (int)(count / 2) - (int)i;
        else if (pattern == 3) key = (int)(Random(&seed) % 17) - 8;
        else key = 7;
        switch (type) {
            case 0: ((int*)base)[i] = key; break;
            case 1: ((long*)base)[i] = key; break;
            case 2: ((float*)base)[i] = (float)key / 4.0f; break;
            case 3: ((double*)base)[i] = (double)key / 4.0; break;
            case 4: ((long double*)base)[i] = (long double)key / 4.0L; break;
            default: {
                Record* record = &((Record*)base)[i];
                record->key = key;
                record->id = (uint32_t)i;
                for (size_t j = 0; j < sizeof(record->payload); ++j)
                    record->payload[j] = (unsigned char)((i * 17 + j) % 251);
                break;
            }
        }
    }
}

static uint64_t Now(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) != 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    return (uint64_t)ts.tv_sec * UINT64_C(1000000000) + (uint64_t)ts.tv_nsec;
}

/* Record keys can be equal. Check membership by unique id, not by stable order. */
static int Validate(const void* data, const void* source, const void* reference,
                    size_t count, size_t type, unsigned char* seen) {
    const unsigned char* bytes = data;
    size_t size = types[type].size;
    for (size_t i = 1; i < count; ++i)
        if (types[type].compare(bytes + (i - 1) * size, bytes + i * size) > 0) return 0;
    if (type != 5) return memcmp(data, reference, count * size) == 0;
    memset(seen, 0, count);
    for (size_t i = 0; i < count; ++i) {
        const Record* record = &((const Record*)data)[i];
        if (record->id >= count || seen[record->id] ||
            memcmp(record, &((const Record*)source)[record->id], sizeof(Record)) != 0 ||
            record->key != ((const Record*)reference)[i].key) return 0;
        seen[record->id] = 1;
    }
    return 1;
}

static int RunCase(IEcoSortAndSearch* sort, size_t type, size_t pattern,
                   size_t count, size_t caseIndex) {
    size_t size = types[type].size;
    size_t bytes = count * size;
    size_t batch = count < 8192 ? 8192 / count : 1;
    uint64_t seed = UINT64_C(20261010) + type * UINT64_C(1000003) + count;
    void* source = malloc(bytes);
    void* reference = malloc(bytes);
    unsigned char* work = malloc(bytes * batch);
    unsigned char* seen = malloc(count);
    int ok = 0;
    if (!source || !reference || !work || !seen) {
        fprintf(stderr, "Allocation failed\n");
        goto Cleanup;
    }
    Generate(source, count, type, pattern, seed);
    memcpy(reference, source, bytes);
    qsort(reference, count, size, types[type].compare);
    if (!Validate(reference, source, reference, count, type, seen)) goto Cleanup;

    /* One untimed warmup for both algorithms, then nine paired measurements.
     * Alternate which algorithm goes first; each sort receives a fresh copy. */
    for (int sample = -1; sample < SAMPLES; ++sample) {
        uint64_t elapsed[2] = {0, 0};
        int first = (int)((caseIndex + (size_t)(sample + 1)) % 2);
        for (int order = 0; order < 2; ++order) {
            int algorithm = (first + order) % 2;
            for (size_t b = 0; b < batch; ++b) memcpy(work + b * bytes, source, bytes);
            uint64_t start = Now();
            for (size_t b = 0; b < batch; ++b) {
                if (algorithm == 0)
                    sort->pVTbl->Sort(sort, work + b * bytes, count, size, types[type].compare);
                else
                    qsort(work + b * bytes, count, size, types[type].compare);
            }
            elapsed[algorithm] = Now() - start;
            for (size_t b = 0; b < batch; ++b) {
                if (!Validate(work + b * bytes, source, reference, count, type, seen)) {
                    fprintf(stderr, "Validation failed: %s/%s/%zu algorithm=%d\n",
                            types[type].name, patterns[pattern], count, algorithm);
                    goto Cleanup;
                }
            }
        }
        if (sample >= 0 && printf("%s,%s,%zu,%zu,%zu,%d,%llu,%llu,%d,%llu\n",
                types[type].name, patterns[pattern], count, size, batch, sample,
                (unsigned long long)elapsed[0], (unsigned long long)elapsed[1],
                first, (unsigned long long)seed) < 0) goto Cleanup;
    }
    ok = 1;
Cleanup:
    free(seen);
    free(work);
    free(reference);
    free(source);
    return ok;
}

int16_t EcoMain(IEcoUnknown* unknown) {
    IEcoSystem1* system = 0;
    IEcoInterfaceBus1* bus = 0;
    IEcoSortAndSearch* sort = 0;
    int16_t result = ERR_ECO_FAIL;
    size_t caseIndex = 0;
    if (!unknown || unknown->pVTbl->QueryInterface(unknown, &GID_IEcoSystem, (void**)&system) != 0 || !system)
        goto Cleanup;
    if (system->pVTbl->QueryInterface(system, &IID_IEcoInterfaceBus1, (void**)&bus) != 0 || !bus)
        goto Cleanup;
    if (bus->pVTbl->RegisterComponent(bus, &CID_EcoHeapSort,
            (IEcoUnknown*)GetIEcoComponentFactoryPtr_A3A5BD4760D34C1691719DF4D8D41318) != 0)
        goto Cleanup;
    if (bus->pVTbl->QueryComponent(bus, &CID_EcoHeapSort, 0,
            &IID_IEcoSortAndSearch, (void**)&sort) != 0 || !sort)
        goto Cleanup;
    struct timespec resolution;
    if (clock_getres(CLOCK_MONOTONIC_RAW, &resolution) != 0) goto Cleanup;
    fprintf(stderr, "timer=CLOCK_MONOTONIC_RAW resolution_ns=%llu samples=%d\n",
            (unsigned long long)resolution.tv_sec * 1000000000ULL + resolution.tv_nsec, SAMPLES);
    puts("type,pattern,count,element_bytes,batch,sample,heap_batch_ns,qsort_batch_ns,first_algorithm,seed");
    for (size_t type = 0; type < COUNT(types); ++type) {
        for (size_t pattern = 0; pattern < COUNT(patterns); ++pattern) {
            for (size_t n = 0; n < COUNT(sizes); ++n) {
                if (type != 0 && sizes[n] > 262144) continue;
                if (!RunCase(sort, type, pattern, sizes[n], caseIndex++)) goto Cleanup;
            }
            fprintf(stderr, "Validated %s / %s\n", types[type].name, patterns[pattern]);
            fflush(stdout);
        }
    }
    if (fflush(stdout) != 0 || ferror(stdout)) goto Cleanup;
    result = ERR_ECO_SUCCESS;
Cleanup:
    if (sort) sort->pVTbl->Release(sort);
    if (bus) bus->pVTbl->Release(bus);
    if (system) system->pVTbl->Release(system);
    return result;
}
