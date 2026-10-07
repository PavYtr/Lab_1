/*
 * <character encoding>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </character encoding>
 *
 * <summary>
 *   IdEcoHeapSort
 * </summary>
 *
 * <description>
 *   This header describes the interface IdEcoHeapSort
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

#ifndef __ID_ECOHEAPSORT_H__
#define __ID_ECOHEAPSORT_H__

#include "IEcoBase1.h"
#include "IEcoHeapSort.h"

/* EcoHeapSort CID = {A3A5BD47-60D3-4C16-9171-9DF4D8D41318} */
#ifndef __CID_EcoHeapSort
static const UGUID CID_EcoHeapSort = {0x01, 0x10, {0xA3, 0xA5, 0xBD, 0x47, 0x60, 0xD3, 0x4C, 0x16, 0x91, 0x71, 0x9D, 0xF4, 0xD8, 0xD4, 0x13, 0x18}};
#endif /* __CID_EcoHeapSort */

/* Component factory for dynamic and static layout */
#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr();
#elif ECO_LIB
extern IEcoComponentFactory* GetIEcoComponentFactoryPtr_A3A5BD4760D34C1691719DF4D8D41318;
#endif

#endif /* __ID_ECOHEAPSORT_H__ */

