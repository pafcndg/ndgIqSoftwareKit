# -*- mode: Makefile; tab-width: 4; indent-tabs-mode: t; -*-

################################################################################
# This file is part of liblzg.
#
# Copyright (c) 2010-2011 Marcus Geelnard
#
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would
#    be appreciated but is not required.
#
# 2. Altered source versions must be plainly marked as such, and must not
#    be misrepresented as being the original software.
#
# 3. This notice may not be removed or altered from any source
#    distribution.
################################################################################

# Compiler and linker settings
INTERMEDIATES = $(OUT)/tools/intermediates
BINARIES = $(OUT)/tools/bin

CC = gcc
CFLAGS = -c -O3 -W -Wall -I../include
LFLAGS = -L$(OUT)/tools/intermediates
LIBS = -llzg
RM = rm -f

# Benchmark configuration
USE_ZLIB = NO
USE_BZ2 = NO
USE_LZO = NO

BM_CFLAGS = $(CFLAGS)
BM_LIBS = $(LIBS)
ifeq ($(USE_ZLIB),YES)
BM_CFLAGS += -DUSE_ZLIB
BM_LIBS += -lz
endif
ifeq ($(USE_BZ2),YES)
BM_CFLAGS += -DUSE_BZ2
BM_LIBS += -lbz2
endif
ifeq ($(USE_LZO),YES)
BM_CFLAGS += -DUSE_LZO
BM_LIBS += -llzo2
endif
BM_CFLAGS += -L$(OUT)/tools/intermediates

# Files
LZG = $(BINARIES)/lzg
LZG_OBJS = $(INTERMEDIATES)/lzg.o
LZG_CRC = $(BINARIES)/lzg_crc
LZG_CRC_OBJS = $(INTERMEDIATES)/lzg_crc.o
UNLZG = $(BINARIES)/unlzg
UNLZG_OBJS = $(INTERMEDIATES)/unlzg.o
BENCHMARK = $(BINARIES)/benchmark
BENCHMARK_OBJS = $(INTERMEDIATES)/benchmark.o
STATIC_LIB = $(INTERMEDIATES)/liblzg.a

.PHONY: all clean

# Master rule
all: $(LZG) $(LZG_CRC) $(UNLZG) $(BENCHMARK)

# Clean rule
clean:
	$(RM) $(LZG) $(LZG_OBJS) $(UNLZG) $(UNLZG_OBJS) \
	      $(BENCHMARK) $(BENCHMARK_OBJS)

# Program build rules
$(LZG): $(LZG_OBJS) $(STATIC_LIB)
	$(CC) $(LFLAGS) -o $@ $(LZG_OBJS) $(LIBS)

$(LZG_CRC): $(LZG_CRC_OBJS) $(STATIC_LIB)
	$(CC) $(LFLAGS) -o $@ $(LZG_CRC_OBJS) $(LIBS)

$(UNLZG): $(UNLZG_OBJS) $(STATIC_LIB)
	$(CC) $(LFLAGS) -o $@ $(UNLZG_OBJS) $(LIBS)

$(BENCHMARK): $(BENCHMARK_OBJS) $(STATIC_LIB)
	$(CC) $(LFLAGS) -o $@ $(BENCHMARK_OBJS) $(BM_LIBS)

# Object files build rules
$(LZG_OBJS): lzg.c ../include/lzg.h
	$(CC) $(CFLAGS) -o $@ $<

$(LZG_CRC_OBJS): lzg_crc.c ../include/lzg.h
	$(CC) $(CFLAGS) -o $@ $<

$(UNLZG_OBJS): unlzg.c ../include/lzg.h
	$(CC) $(CFLAGS) -o $@ $<

$(BENCHMARK_OBJS): benchmark.c ../include/lzg.h
	$(CC) $(BM_CFLAGS) -o $@ $<

