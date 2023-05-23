GIT_HASH=`git describe --always --dirty`
COMPILE_TIME=`date -u +'%Y-%m-%d %H:%M:%S UTC'`
COMPILE_TS=`date -u +'%Y%m%d%H%M'`
GIT_BRANCH=`git branch | grep "^\*" | sed 's/^..//'`
export VERSION_FLAGS=-DGIT_HASH="\"$(GIT_HASH)\"" -DCOMPILE_TIME="\"$(COMPILE_TIME)\"" -DGIT_BRANCH="\"$(GIT_BRANCH)\""


# Folder structure:
OBJ := obj
OBJARM := objarm
SRC := ./**/
INC := ./**/
EXE := exe

# Compilers
CXX = g++
CC = gcc
CROSS_COMPILE = arm-linux-gnueabihf
CCARM = $(CROSS_COMPILE)-g++
LDARM = $(CROSS_COMPILE)-g++

UNAME_S := $(shell uname -s)

# DE10 specific:
ALT_DEVICE_FAMILY ?= soc_cv_av
#SOCEDS_DEST_ROOT = /home/depa/intelFPGA/20.1/embedded
HWLIBS_ROOT = $(SOCEDS_DEST_ROOT)/ip/altera/hps/altera_hps/hwlib

# Flags and includes:
INCLUDE := -I$(INC)
INCLUDEARM += $(INCLUDE)

DEBUGFLAGS := #-g -fsanitize=address -fstack-protector
CFLAGS := -Wall -Wextra -pthread -std=c++11 $(DEBUGFLAGS)
LDFLAGS := -Wall -Wextra -pthread $(DEBUGFLAGS)

CPPFLAGS := $(CFLAGS) $(INCLUDE)
CFLAGSARM := $(CFLAGS) $(INCLUDEARM) -I$(HWLIBS_ROOT)/include -I$(HWLIBS_ROOT)/include/$(ALT_DEVICE_FAMILY) -D$(ALT_DEVICE_FAMILY)

OPTFLA := -g
HPSOPTFLAG := -g
# OPTFLAG := -O3
# HPSOPTFLAG := -O2

# Objects and sources:
OBJECTS=$(OBJ)/elettroforo.o $(OBJ)/NewHV.o $(OBJ)/ADC101CS021.o $(OBJ)/LTC1669.o

OBJECTSHPS := $(OBJARM)/elettroforo.o $(OBJARM)/NewHV.o $(OBJARM)/ADC101CS021.o $(OBJARM)/LTC1669.o

# Executables:
ELETTROFORO := $(EXE)/EFORO

# Rules:
all: $(ELETTROFORO)
elettroforo: $(ELETTROFORO)


$(OCADAQ): $(OBJECTS)
	@echo Linking $^ to $@
	@mkdir -pv $(EXE)
	$(CXX) $(CPPFLAGS) $^ -o $@

$(PAPERO): $(OBJECTSHPS)
ifeq ($(UNAME_S),Darwin)
	@echo Compilation under MacOs not possibile
else
	@echo Linking $^ to $@
	@mkdir -pv $(EXE)
	$(LDARM) $(LDFLAGS) $^ -o $(PAPERO)
endif



$(OBJ)/%.o: $(SRC)/%.cpp
	@echo Compiling $< ...
	@mkdir -pv $(OBJ)
	$(CXX) $(CPPFLAGS) $(OPTFLAG) $(VERSION_FLAGS) -c -o $@ $<

$(OBJARM)/%.o: $(SRC)/%.cpp
ifeq ($(UNAME_S),Darwin)
	@echo Compilation under MacOs not possibile
else
	@echo Compiling $< ...
	@mkdir -pv $(OBJARM)
	$(CCARM) $(CFLAGSARM) $(HPSOPTFLAG) $(VERSION_FLAGS) -c -o $@ $<
endif

clean:
	@echo " Cleaning all..."
	@$(RM) -Rfv $(OBJ)
	@$(RM) -Rfv $(OBJARM)
	@$(RM) -Rfv $(EXE)

.PHONY: clean
