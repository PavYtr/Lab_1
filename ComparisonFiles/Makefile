ROOT := $(abspath ..)
ECO_FRAMEWORK ?= $(abspath $(ROOT)/../Eco.Framework)
ifeq ($(strip $(ECO_TOOLCHAIN)),)
CC := gcc
SYSROOT :=
else
CC := $(ECO_TOOLCHAIN)/eco-x86_64-linux-gnu-gcc-13.2.0/bin/x86_64-linux-gnu-gcc
SYSROOT := --sysroot=$(ECO_TOOLCHAIN)/eco-x86_64-linux-gnu-gcc-13.2.0
endif

CPPFLAGS := -DECO_LIB -DECO_X86_64 -DUGUID_UTILITY -DECO_LINUX \
 -I$(ROOT)/SharedFiles -I$(ECO_FRAMEWORK)/Eco.Core1/SharedFiles \
 -I$(ECO_FRAMEWORK)/Eco.InterfaceBus1/SharedFiles
CFLAGS := $(SYSROOT) -std=c11 -O2 -DNDEBUG -Wall -Wextra -fno-builtin
COMPONENT := $(ROOT)/BuildFiles/Linux/x86_64/StaticRelease/libA3A5BD4760D34C1691719DF4D8D41318.a
ECO_LIBS := -L$(ECO_FRAMEWORK)/Eco.System1/BuildFiles/Linux/x86_64/StaticRelease \
 -L$(ECO_FRAMEWORK)/Eco.InterfaceBus1/BuildFiles/Linux/x86_64/StaticRelease \
 -L$(ECO_FRAMEWORK)/Eco.MemoryManager1/BuildFiles/Linux/x86_64/StaticRelease \
 -L$(ECO_FRAMEWORK)/Eco.FileSystemManagement1/BuildFiles/Linux/x86_64/StaticRelease \
 -Wl,--start-group $(COMPONENT) -l00000000000000000000000053595333 \
 -l00000000000000000000000042757331 -l0000000000000000000000004D656D31 \
 -l00000000000000000000000046534D31 -Wl,--end-group -lm -ldl -lgcc

.PHONY: all component run plots
all: build/benchmark
component:
	$(MAKE) -C $(ROOT)/AssemblyFiles/Linux/gcc_v132 -f Makefile TARGET=1 ARCH=x86_64 DEBUG=0
build/benchmark.o: benchmark.c Makefile $(ROOT)/SharedFiles/IEcoSortAndSearch.h
	@mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@
build/benchmark: build/benchmark.o component
	$(CC) $(SYSROOT) -o $@ build/benchmark.o $(ECO_LIBS)
run: all
	python3 run.py
plots:
	python3 plot.py
-include build/benchmark.d
