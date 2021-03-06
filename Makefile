#==================================================
# Makefile version: 1.0
# Setting time: 2013/04/30
#==================================================
.PHONY : clean run
VPATH=library
ver=debug

#==================================================
# Platform Detect MACRO
#==================================================
OSUPPER = $(shell uname -s 2>/dev/null | tr [:lower:] [:upper:])
OSLOWER = $(shell uname -s 2>/dev/null | tr [:upper:] [:lower:])

#Flags to detect 32-bit or 64-bit OS platform
OS_SIZE = $(shell uname -m | sed -e "s/i.86/32/" -e "s/x86_64/64/")
OS_ARCH = $(shell uname -m | sed -e "s/i386/i686/")

# These flags will override any settings
ifeq ($(i386),1)
	OS_SIZE = 32
	OS_ARCH = i686
endif
ifeq ($(x86_64),1)
	OS_SIZE = 64
	OS_ARCH = x86_64
endif

ifeq ($(OS_SIZE), 32)
	mflags = -m32
else
	mflags = -m64
endif

#==================================================
# Command Define
#==================================================
CC            = gcc
CXX           = g++
PRE_PROCESSOR = ${CXX}
LINKER        = ${CXX}
DEFINES       = 
DEL_FILE      = rm -f
DEL_DIR       = rm -rf
MKDIR         = mkdir -p
MOVE          = mv -u
ARG           = 
STDIN         = 
SHELL         = /bin/sh

#==================================================
# CUDA related
#==================================================
CUDA_PATH	?= /opt/cuda
ifeq ($(OS_SIZE), 32)
	CUDA_LIB	?= $(CUDA_PATH)/lib
else
	CUDA_LIB	?= $(CUDA_PATH)/lib64
endif
CUDA_INC	?= $(CUDA_PATH)/include
CUDA_BIN	?= $(CUDA_PATH)/bin
CULDFLAGS   = $(mflags) -gencode arch=compute_30,code=sm_30 -gencode arch=compute_35,code=sm_35
NVCC		= $(CUDA_BIN)/nvcc

#==================================================
# Flag Define -- build version dependant
#==================================================

ifeq ($(ver),debug)
	CFLAGS   = -O1 -g3 -Wall -Ddebug -pthread ${DEFINES} 
	CXXFLAGS = -O1 -g3 -Wall -Ddebug ${DEFINES} 
else
	CFLAGS   = -O3 -Wall -pthread ${DEFINES} 
	CXXFLAGS = -O3 -Wall ${DEFINES} 
endif
LIBS     = -L. -L$(CUDA_LIB) -lncurses -pthread -lSDL -lSDL_gfx -lSDL_ttf -lGL -lcuda -lcudart
INCLUDE  = -I. `sdl-config --cflags --libs` -I$(CUDA_INC)

#==================================================
# Source, library, bin Define
#==================================================
TARGET = FDTD
BIN_DIR     = bin
LIBRARY_DIR = library
SOURCE_FILE = SDL_gfxPrimitives.c FDTD2D.cpp main.cpp
OBJECT_FILE += $(addsuffix .o, $(basename $(SOURCE_FILE))) solver.o

#==================================================
# Compile implicit rules.
#==================================================

%.o:%.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)
	@$(MOVE) $@ $(LIBRARY_DIR)

%.o:%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDE)
	@$(MOVE) $@ $(LIBRARY_DIR)

#==================================================
# Compile rules
#==================================================

default: new_dir $(TARGET)

new_dir:
	$(MKDIR) $(LIBRARY_DIR) $(BIN_DIR)

$(TARGET): $(OBJECT_FILE) solver.cu
	cd $(LIBRARY_DIR); \
	$(LINKER) -o $@ $(OBJECT_FILE) $(LIBS); \
	cd ..
	@$(MOVE) $(LIBRARY_DIR)/$@ $(BIN_DIR)

solver.o: solver.cu
	$(NVCC) -c $< -o $@ $(CULDFLAGS) $(CUINCLUDE)
	@$(MOVE) $@ $(LIBRARY_DIR)

run: 
	./$(BIN_DIR)/$(TARGET) $(ARG)

clean: 
	$(DEL_FILE) $(LIBRARY_DIR)/*.o
	$(DEL_FILE) $(BIN_DIR)/$(TARGET)

clean_all: 
	${DEL_DIR} ${LIBRARY_DIR} ${BIN_DIR} 
