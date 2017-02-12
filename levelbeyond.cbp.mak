#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres
RESTBED_DIR = <needs to be filled in>
RESTBED_INLUDES = -I$(RESTBED_DIR)restbed/source/corvusoft -I$(RESTBED_DIR)restbed/source
INC = $(RESTBED_INCLUDES)
CFLAGS = -Wall -std=c++14 -m64 -fpic -fexceptions $(INC)
RESINC = 
LIBDIR = /usr/lib64/
RESTBEDLIB = $(RESTBED_DIR)restbed/build/librestbed.a
LIB = $(RESTBEDLIB) $(LIBDIR)libboost_system.so $(LIBDIR)libboost_thread.so $(LIBDIR)libcrypt.so $(LIBDIR)libcrypto.so $(LIBDIR)libboost_log.so $(LIBDIR)libssl.so $(LIBDIR)libboost_regex.so $(LIBDIR)libboost_program_options.so
LIB0 = $(RESTBEDLIB) $(LIBDIR)libpthread.so $(LIBDIR)libboost_system.so $(LIBDIR)libboost_thread.so $(LIBDIR)libcrypt.so $(LIBDIR)libcrypto.so $(LIBDIR)libboost_log.so $(LIBDIR)libssl.so $(LIBDIR)libboost_regex.so $(LIBDIR)libboost_program_options.so $(RESTBEDLIB) $(LIBDIR)libpthread.so $(LIBDIR)libboost_system.so $(LIBDIR)libboost_thread.so $(LIBDIR)libcrypt.so $(LIBDIR)libcrypto.so $(LIBDIR)libboost_log.so $(LIBDIR)libssl.so $(LIBDIR)libboost_program_options.so

LDFLAGS = -m64

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -std=c++14 -m64 -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
#LIBDIR_DEBUG = $(LIBDIR)
#LIB_DEBUG = $(LIB) -l$(RESTBEDLIB) -l$(LIBDIR)libpthread.so -l$(LIBDIR)libboost_system.so -l$(LIBDIR)libboost_thread.so -l$(LIBDIR)libcrypt.so -l$(LIBDIR)libcrypto.so -l$(LIBDIR)libboost_log.so -l$(LIBDIR)libssl.so -l$(LIBDIR)libboost_program_options.so
LIB_DEBUG = $(LIB0)
LDFLAGS_DEBUG = $(LDFLAGS) -m64
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/levelbeyond

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2 -std=c++14 -m64
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
#LIBDIR_RELEASE = $(LIBDIR)
LIBDIR_RELEASE =
LIB_RELEASE = $(LIB0) 
LDFLAGS_RELEASE = $(LDFLAGS) -s -m64
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = bin/Release/levelbeyond

OBJ_DEBUG = $(OBJDIR_DEBUG)/apiNote.o $(OBJDIR_DEBUG)/jsonStore.o $(OBJDIR_DEBUG)/main.o $(OBJDIR_DEBUG)/serverlb.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/apiNote.o $(OBJDIR_RELEASE)/jsonStore.o $(OBJDIR_RELEASE)/main.o $(OBJDIR_RELEASE)/serverlb.o

all: debug release

clean: clean_debug clean_release

before_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/apiNote.o: apiNote.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c apiNote.cpp -o $(OBJDIR_DEBUG)/apiNote.o

$(OBJDIR_DEBUG)/jsonStore.o: jsonStore.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c jsonStore.cpp -o $(OBJDIR_DEBUG)/jsonStore.o

$(OBJDIR_DEBUG)/main.o: main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c main.cpp -o $(OBJDIR_DEBUG)/main.o

$(OBJDIR_DEBUG)/serverlb.o: serverlb.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c serverlb.cpp -o $(OBJDIR_DEBUG)/serverlb.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin/Debug
	rm -rf $(OBJDIR_DEBUG)

before_release: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/apiNote.o: apiNote.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c apiNote.cpp -o $(OBJDIR_RELEASE)/apiNote.o

$(OBJDIR_RELEASE)/jsonStore.o: jsonStore.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c jsonStore.cpp -o $(OBJDIR_RELEASE)/jsonStore.o

$(OBJDIR_RELEASE)/main.o: main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c main.cpp -o $(OBJDIR_RELEASE)/main.o

$(OBJDIR_RELEASE)/serverlb.o: serverlb.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c serverlb.cpp -o $(OBJDIR_RELEASE)/serverlb.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf bin/Release
	rm -rf $(OBJDIR_RELEASE)

apiNote.cpp: apiNote.h

apiNote.h: serverlb.h jsonStore.h

serverlb.h: /mnt-btrfs/home/jar/projects/restbed/source/restbed

jsonStore.cpp: jsonStore.h

main.cpp: serverlb.h apiNote.h

serverlb.cpp: serverlb.h apiNote.h

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

