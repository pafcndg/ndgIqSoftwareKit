# -*- mode: Makefile; tab-width: 4; indent-tabs-mode: t; -*-

################################################################################
# This file is part of liblzg.
#
# Copyright (c) 2010 Marcus Geelnard
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
CFLAGS = -c -O3 -funroll-loops -W -Wall
AR = ar
ARFLAGS = -rcs
RM = rm -f

# Files
STATIC_LIB = $(INTERMEDIATES)/liblzg.a
OBJS = $(INTERMEDIATES)/encode.o \
       $(INTERMEDIATES)/decode.o \
       $(INTERMEDIATES)/checksum.o \
       $(INTERMEDIATES)/version.o

# Master rule
all: $(STATIC_LIB)

# Clean rule
clean:
	$(RM) $(STATIC_LIB) $(OBJS)

# Library build rule
$(STATIC_LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

# Object files build rules
$(INTERMEDIATES)/encode.o: encode.c internal.h ../include/lzg.h
	$(CC) $(CFLAGS) -o $@ $<

$(INTERMEDIATES)/decode.o: decode.c internal.h ../include/lzg.h
	$(CC) $(CFLAGS) -o $@ $<

$(INTERMEDIATES)/checksum.o: checksum.c internal.h ../include/lzg.h
	$(CC) $(CFLAGS) -o $@ $<

$(INTERMEDIATES)/version.o: version.c internal.h ../include/lzg.h
	$(CC) $(CFLAGS) -o $@ $<

