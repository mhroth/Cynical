# Copyright (c) 2012 The Native Client Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

#
# GNU Make based build file.  For details on GNU Make see:
#   http://www.gnu.org/software/make/manual/make.html
#

include ../ZenGarden/src/Makefile.sourcefiles
VPATH = ../ZenGarden/src

#
# Get pepper directory for toolchain and includes.
#
# If NACL_SDK_ROOT is not set, then assume it can be found a two directories up,
# from the default example directory location.
#
THIS_MAKEFILE:=$(abspath $(lastword $(MAKEFILE_LIST)))
NACL_SDK_ROOT?=$(abspath $(dir $(THIS_MAKEFILE))../..)
CHROME_PATH?=Undefined

#
# Defaults
#
NACL_WARNINGS:=-Wno-long-long -Wall -Wswitch-enum -Werror -pedantic
NACL_CCFLAGS:=-O3 -pthread $(NACL_WARNINGS)  #-O0 -g -pthread $(NACL_WARNINGS)
NACL_CXXFLAGS:=-O3 -pthread -std=gnu++98 $(NACL_WARNINGS)  #-O0 -g -pthread -std=gnu++98 $(NACL_WARNINGS)
NACL_LDFLAGS:=-pthread -lppapi_cpp -lppapi  #-g -pthread -lppapi_cpp -lppapi

#
# Project Settings
#
VALID_TOOLCHAINS:=newlib glibc
TOOLCHAIN?=newlib

ZENGARDEN_SRC_FILEs:=$(LOCAL_SRC_FILES) # LOCAL_SRC_FILES comes from the ZenGarden Makefile.sourcefiles
GETURL_CXX:=ZgnaclInstance.cpp LightPipe.cpp zgcallback.cpp geturl_handler.cpp $(LOCAL_SRC_FILES)
GETURL_CXXFLAGS:=-I../ZenGarden/src -D__APPLE__=0 -msse -mfpmath=sse -ffast-math -w $(NACL_CXXFLAGS)
GETURL_LDFLAGS:=$(NACL_LDFLAGS)


#
# Default target
#
all: newlib/cynical.nmf glibc/cynical.nmf

#
# Alias for standard commands
#
CP:=python $(NACL_SDK_ROOT)/tools/oshelpers.py cp
MKDIR:=python $(NACL_SDK_ROOT)/tools/oshelpers.py mkdir
MV:=python $(NACL_SDK_ROOT)/tools/oshelpers.py mv


#
# Verify we selected a valid toolchain for this example
#
ifeq (,$(findstring $(TOOLCHAIN),$(VALID_TOOLCHAINS)))
$(warning Availbile choices are: $(VALID_TOOLCHAINS))
$(error Can not use TOOLCHAIN=$(TOOLCHAIN) on this example.)
endif


#
# Compute path to requested NaCl Toolchain
#
OSNAME:=$(shell python $(NACL_SDK_ROOT)/tools/getos.py)
TC_PATH:=$(abspath $(NACL_SDK_ROOT)/toolchain)


#
# Verify we have a valid NACL_SDK_ROOT by looking for the toolchain directory
#
ifeq (,$(wildcard $(TC_PATH)))
$(warning No valid NACL_SDK_ROOT at $(NACL_SDK_ROOT))
ifeq ($(origin NACL_SDK_ROOT), 'file')
$(error Override the default value via enviornment variable, or command-line.)
else
$(error Fix the NACL_SDK_ROOT specified in the environment or command-line.)
endif
endif


#
# Disable DOS PATH warning when using Cygwin based NaCl tools on Windows
#
CYGWIN ?= nodosfilewarning
export CYGWIN


#
# Defaults for TOOLS
#

NEWLIB_CC?=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-gcc -c
NEWLIB_CXX?=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-g++ -c
NEWLIB_LINK?=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-g++ -Wl,-as-needed
NEWLIB_DUMP?=$(TC_PATH)/$(OSNAME)_x86_newlib/x86_64-nacl/bin/objdump

GLIBC_CC?=$(TC_PATH)/$(OSNAME)_x86_glibc/bin/i686-nacl-gcc -c
GLIBC_CXX?=$(TC_PATH)/$(OSNAME)_x86_glibc/bin/i686-nacl-g++ -c
GLIBC_LINK?=$(TC_PATH)/$(OSNAME)_x86_glibc/bin/i686-nacl-g++ -Wl,-as-needed
GLIBC_DUMP?=$(TC_PATH)/$(OSNAME)_x86_glibc/x86_64-nacl/bin/objdump
GLIBC_PATHS:=-L $(TC_PATH)/$(OSNAME)_x86_glibc/x86_64-nacl/lib32
GLIBC_PATHS+=-L $(TC_PATH)/$(OSNAME)_x86_glibc/x86_64-nacl/lib



#
# NMF Manifiest generation
#
# Use the python script create_nmf to scan the binaries for dependencies using
# objdump.  Pass in the (-L) paths to the default library toolchains so that we
# can find those libraries and have it automatically copy the files (-s) to
# the target directory for us.
NMF:=python $(NACL_SDK_ROOT)/tools/create_nmf.py


#
# Verify we can find the Chrome executable if we need to launch it.
#
.PHONY: CHECK_FOR_CHROME
CHECK_FOR_CHROME:
ifeq (,$(wildcard $(CHROME_PATH)))
	$(warning No valid Chrome found at CHROME_PATH=$(CHROME_PATH))
	$(error Set CHROME_PATH via an environment variable, or command-line.)
else
	$(warning Using chrome at: $(CHROME_PATH))
endif


#
# Rules for newlib toolchain
#
newlib:
	$(MKDIR) newlib

NEWLIB_GETURL_x86_32_CXX_O:=$(patsubst %.cpp, newlib/%_x86_32.o,$(GETURL_CXX))
$(NEWLIB_GETURL_x86_32_CXX_O) : newlib/%_x86_32.o : %.cpp $(THIS_MAKE) | newlib
	$(NEWLIB_CXX) -o $@ $< -m32 $(GETURL_CXXFLAGS) -DTCNAME=newlib

newlib/cynical_x86_32.nexe : $(NEWLIB_GETURL_x86_32_CXX_O)
	$(NEWLIB_LINK) -o $@ $^ -m32 $(GETURL_LDFLAGS)
NEWLIB_NMF+=newlib/cynical_x86_32.nexe 

NEWLIB_GETURL_x86_64_CXX_O:=$(patsubst %.cpp, newlib/%_x86_64.o,$(GETURL_CXX))
$(NEWLIB_GETURL_x86_64_CXX_O) : newlib/%_x86_64.o : %.cpp $(THIS_MAKE) | newlib
	$(NEWLIB_CXX) -o $@ $< -m64 $(GETURL_CXXFLAGS) -DTCNAME=newlib

newlib/cynical_x86_64.nexe : $(NEWLIB_GETURL_x86_64_CXX_O)
	$(NEWLIB_LINK) -o $@ $^ -m64 $(GETURL_LDFLAGS)
NEWLIB_NMF+=newlib/cynical_x86_64.nexe 

newlib/cynical.nmf : $(NEWLIB_NMF)
	$(NMF) -D $(NEWLIB_DUMP) -o $@ $^ -t newlib -s newlib


#
# Rules for glibc toolchain
#
glibc:
	$(MKDIR) glibc

GLIBC_GETURL_x86_32_CXX_O:=$(patsubst %.cpp, glibc/%_x86_32.o,$(GETURL_CXX))
$(GLIBC_GETURL_x86_32_CXX_O) : glibc/%_x86_32.o : %.cpp $(THIS_MAKE) | glibc
	$(GLIBC_CXX) -o $@ $< -m32 $(GETURL_CXXFLAGS) -DTCNAME=glibc

glibc/cynical_x86_32.nexe : $(GLIBC_GETURL_x86_32_CXX_O)
	$(GLIBC_LINK) -o $@ $^ -m32 $(GETURL_LDFLAGS)
GLIBC_NMF+=glibc/cynical_x86_32.nexe 

GLIBC_GETURL_x86_64_CXX_O:=$(patsubst %.cpp, glibc/%_x86_64.o,$(GETURL_CXX))
$(GLIBC_GETURL_x86_64_CXX_O) : glibc/%_x86_64.o : %.cpp $(THIS_MAKE) | glibc
	$(GLIBC_CXX) -o $@ $< -m64 $(GETURL_CXXFLAGS) -DTCNAME=glibc

glibc/cynical_x86_64.nexe : $(GLIBC_GETURL_x86_64_CXX_O)
	$(GLIBC_LINK) -o $@ $^ -m64 $(GETURL_LDFLAGS)
GLIBC_NMF+=glibc/cynical_x86_64.nexe 

glibc/cynical.nmf : $(GLIBC_NMF)
	$(NMF) -D $(GLIBC_DUMP) -o $@ $(GLIBC_PATHS) $^ -t glibc -s glibc $(GLIBC_REMAP)





RUN: all
	python ../httpd.py

LAUNCH_NEXE: CHECK_FOR_CHROME all
	$(CHROME_PATH) $(NEXE_ARGS) "localhost:5103/$(PROJECT).html?tool=$(TOOLCHAIN)"


