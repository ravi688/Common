#-------------------------------------------
#		Requirements
#-------------------------------------------
#	1. mcpp: http://mcpp.sourceforge.net/
#	2. dot or graphviz: https://graphviz.org/
#	3. uncrustify: https://github.com/uncrustify/uncrustify
#	4. ar
#	5. gcc
#-------------------------------------------

#-------------------------------------------
#	Platform Detection
#-------------------------------------------

PLATFORM_DETECT := $(shell uname)

# MINGW (Windows)
ifneq (,$(findstring MINGW,$(PLATFORM_DETECT)))
	PLATFORM := Windows
endif

# Linux
ifneq (,$(findstring Linux,$(PLATFORM_DETECT)))
	PLATFORM := Linux
endif

# FreeBSD
ifneq (,$(findstring FreeBSD,$(PLATFORM_DETECT)))
	PLATFORM := FreeBSD
endif

#-------------------------------------------
#		Project Configuration
#-------------------------------------------
PROJECT_NAME = Common
STATIC_LIB_NAME = common.a
DYNAMIC_LIB_NAME = common.dll
EXECUTABLE_NAME = common
MAIN_SOURCE_LANG = cpp
#The sources which needs to be included when building primary executable
MAIN_SOURCES=source/main.cpp
#The sources which needs to be included when building test executasble
TEST_SOURCES=source/test.cpp
EXTERNAL_INCLUDES =
EXTERNAL_LIBRARIES = $(shell pkg-config catch2-with-main --libs)
BUILD_DEFINES=

DEPENDENCIES = ../../../shared-dependencies/BufferLib ../../../dependencies/SafeMemory/shared-dependencies/CallTrace ../../../dependencies/MeshLib/dependencies/DiskManager
DEPENDENCY_LIBS = ../../../shared-dependencies/BufferLib/lib/bufferlib.a ../../../dependencies/SafeMemory/shared-dependencies/CallTrace/lib/calltrace.a ../../../dependencies/MeshLib/dependencies/DiskManager/lib/diskmanager.a
DEPENDENCIES_DIR = ./dependencies
SHARED_DEPENDENCIES =
SHARED_DEPENDENCY_LIBS =
SHARED_DEPENDENCIES_DIR = ./shared-dependencies


ifeq ($(VERBOSE),1)
	BUILD_DEFINES += -DCOM_VERBOSE
endif

# Windows
# ifeq ($(PLATFORM),Windows)
# 	EXTERNAL_LIBRARIES += -L${VK_SDK_PATH}/lib/ -lvulkan-1  -L./dependencies/VulkanRenderer/external-dependency-libs/win -lglfw3 -lfreetype.dll -lgdi32
# 	EXTERNAL_INCLUDES += -I${VK_SDK_PATH}/include/
# endif

# Linux
# ifeq ($(PLATFORM),Linux)
# 	EXTERNAL_LIBRARIES += -L${VULKAN_SDK}/lib/ -lvulkan -L./dependencies/VulkanRenderer/external-dependency-libs/linux -lglfw3 -lfreetype -lz -lpng -lbrotlidec -lm
# 	EXTERNAL_INCLUDES += -I${VULKAN_SDK}/include/
# endif

#-------------------------------------------

#-------------------------------------------
#		Project Initialization and Uitilty commands
#-------------------------------------------
__DEPENDENCIES = $(addprefix $(DEPENDENCIES_DIR)/, $(DEPENDENCIES))
__DEPENDENCY_LIBS = $(addprefix $(DEPENDENCIES_DIR)/, $(DEPENDENCY_LIBS))
__SHARED_DEPENDENCIES = $(addprefix $(SHARED_DEPENDENCIES_DIR)/, $(SHARED_DEPENDENCIES))
__SHARED_DEPENDENCY_LIBS = $(addprefix $(SHARED_DEPENDENCIES_DIR)/, $(SHARED_DEPENDENCY_LIBS))
ifdef COMSPEC
__EXECUTABLE_NAME = $(addsuffix .exe, $(basename $(EXECUTABLE_NAME)))
__TEST_EXECUTABLE_NAME=$(addsuffix .exe, $(join $(basename $(EXECUTABLE_NAME)),.test))
else
__EXECUTABLE_NAME = $(basename $(EXECUTABLE_NAME))
__TEST_EXECUTABLE_NAME=$(join $(basename $(EXECUTABLE_NAME)),.test)
endif
.PHONY: all
.PHONY: init
all: dgraph release

%.gv:
	echo digraph $(PROJECT_NAME) "{ $(PROJECT_NAME); }" > $@
	@echo [Log] $@ created successfully!

$(DEPENDENCIES_DIR) $(SHARED_DEPENDENCIES_DIR):
	mkdir $(subst /,\,$@)
	@echo [Log] $@ created successfully!

init: $(PROJECT_NAME).gv $(DEPENDENCIES_DIR) $(SHARED_DEPENDENCIES_DIR)
	@echo [Log] $(PROJECT_NAME) init successfully!
	$(foreach var, $(__DEPENDENCIES), $(MAKE) --directory=$(var) init;)

setup:
	git submodule update --init
	git -C ./dependencies/Common checkout main
	git -C ./dependencies/Common submodule update --init dependencies/BufferLib
	git -C ./dependencies/Common/dependencies/BufferLib checkout main
	git -C ./dependencies/Common/dependencies/BufferLib submodule update --init dependencies/CallTrace
	git -C ./dependencies/Common/dependencies/BufferLib/dependencies/CallTrace checkout main
	@echo [Log] Setup successfully!

update:
	git -C ./dependencies/Common pull origin main
	git -C ./dependencies/Common/dependencies/BufferLib pull origin main
	git -C ./dependencies/Common/dependencies/BufferLib/dependencies/CallTrace pull origin main
	git -C ./dependencies/Common push
	git -C ./dependencies/Common/dependencies/BufferLib push
	git -C ./dependencies/Common/dependencies/BufferLib/dependencies/CallTrace push
#-------------------------------------------


#-------------------------------------------
#		Dependency Graph Generation
#-------------------------------------------
DGRAPH_TARGET = ./dependency_graph/$(PROJECT_NAME).png
DGRAPH_TARGET_DIR = dependency_graph
DGRAPH_SCRIPT = $(PROJECT_NAME).gv
DGRAPH_INCLUDES = $(addprefix -I, $(__DEPENDENCIES) $(__SHARED_DEPENDENCIES))
DGRAPH_COMPILER = dot
DGRAPH_FLAGS = -Tpng

DGRAPH_PREPROCESSOR = mcpp
DGRAPH_PREPROCESSOR_FLAGS = -P


DGRAPH_PREPROCESSED_SCRIPT = $(addsuffix .i, $(DGRAPH_SCRIPT))

.PHONY: dgraph
.PHONY: dgraph-clean

%.gv.i:
	$(DGRAPH_PREPROCESSOR) $(DGRAPH_PREPROCESSOR_FLAGS) $(DGRAPH_INCLUDES) $(basename $@) -o $@

$(DGRAPH_TARGET_DIR):
	mkdir $@

dgraph: $(DGRAPH_PREPROCESSED_SCRIPT) | $(DGRAPH_TARGET_DIR)
	$(DGRAPH_COMPILER) $(DGRAPH_FLAGS) $(DGRAPH_PREPROCESSED_SCRIPT) > $(DGRAPH_TARGET)

dgraph-clean:
	del dependency_graph\$(PROJECT_NAME).png
	rmdir dependency_graph
	del $(PROJECT_NAME).gv.i
	@echo [Log] Dependency graphs cleaned successfully!
#-------------------------------------------

#-------------------------------------------
#
#-------------------------------------------



#-------------------------------------------
#		Binary Generation
#-------------------------------------------
TARGET_LIB_DIR = ./lib
TARGET_STATIC_LIB = $(join $(TARGET_LIB_DIR)/, $(STATIC_LIB_NAME))
TARGET_DYNAMIC_LIB = $(join $(TARGET_LIB_DIR)/, $(DYNAMIC_LIB_NAME))
TARGET = $(__EXECUTABLE_NAME)
TEST_TARGET=$(__TEST_EXECUTABLE_NAME)

#Dependencies
DEPENDENCY_INCLUDES = $(addsuffix /include, $(__DEPENDENCIES))
SHARED_DEPENDENCY_INCLUDES = $(addsuffix /include, $(__SHARED_DEPENDENCIES))

MAIN_OBJECT=$(addsuffix .o, $(wildcard $(MAIN_SOURCES)))
TEST_OBJECT=$(addsuffix .o, $(wildcard $(TEST_SOURCES)))
#The sources which needs to be excluded when building libraries (static or dynamic)
EXCLUDE_SOURCES=$(MAIN_SOURCES) $(TEST_SOURCES)
INCLUDES= -I./include $(EXTERNAL_INCLUDES) $(addprefix -I, $(DEPENDENCY_INCLUDES) $(SHARED_DEPENDENCY_INCLUDES))
C_SOURCES=$(filter-out $(EXCLUDE_SOURCES), $(wildcard source/*.c source/*/*.c))
CPP_SOURCES=$(filter-out $(EXCLUDE_SOURCES), $(wildcard source/*.cpp source/*/*.cpp))
SOURCES= $(C_SOURCES) $(CPP_SOURCES)
OBJECTS= $(addsuffix .o, $(SOURCES))
LIBS = $(EXTERNAL_LIBRARIES)

#Flags and Defines
DEBUG_DEFINES =  -DGLOBAL_DEBUG -DDEBUG -DLOG_DEBUG
RELEASE_DEFINES =  -DGLOBAL_RELEASE -DRELEASE -DLOG_RELEASE
DEFINES = $(BUILD_DEFINES)
ifeq ($(XDG_SESSION_TYPE),wayland)
	DEFINES+=-DWINDOW_SYSTEM_WAYLAND
endif
ifeq ($(XDG_SESSION_TYPE),x11)
	DEFINES+=-DWINDOW_SYSTEM_X11
endif

COMPILER_FLAGS= -m64
C_COMPILER_FLAGS = $(COMPILER_FLAGS)
CPP_COMPILER_FLAGS = $(COMPILER_FLAGS) -std=c++20
LINKER_FLAGS= -m64
LINKER=g++
DYNAMIC_LIBRARY_COMPILATION_FLAG = -shared
DYNAMIC_IMPORT_LIBRARY_FLAG = -Wl,--out-implib,
C_COMPILER = gcc
CPP_COMPILER = g++
ARCHIVER_FLAGS = -rc
ARCHIVER = ar

ifeq ($(STACK_PROTECT),1)
	COMPILER_FLAGS += -fstack-protector
	LINKER_FLAGS += -fstack-protector
endif

ifeq ($(ADDRESS_SANITIZE),1)
	COMPILER_FLAGS += -fsanitize=address -static-libasan
	LINKER_FLAGS += -fsanitize=address -static-libasan
endif

ifeq ($(SHADOW_CALL_STACK_SANITIZE),1)
	COMPILER_FLAGS += -fsanitize=shadow-call-stack
	LINKER_FLAGS += -fsanitize=shadow-call-stack
endif

ifeq ($(LEAK_SANITIZE),1)
	# no need to add it to COMPILER_FLAGS, as this flag only causes a library linking which overrides the malloc
	LINKER_FLAGS += -fsanitize=leak
endif

ifeq ($(PROF),1)
	COMPILER_FLAGS += -p
	LINKER_FLAGS += -p
endif

ifeq ($(GPROF),1)
	COMPILER_FLAGS += -pg
	LINKER_FLAGS += -pg
endif

ifeq ($(NOOPT),1)
	COMPILER_FLAGS += -O0
	LINKER_FLAGS += -O0
endif

ifeq ($(WALL),1)
	COMPILER_FLAGS += -Wall
endif

DEBUG_COMPILER_FLAGS= -g #-fsanitize=integer-divide-by-zero // why it is not working on windows 64 bit?
RELEASE_COMPILER_FLAGS= -O3 
DEBUG_LINKER_FLAGS= -g #-fsanitize=integer-divide-by-zero  // why it is not working on windows 64 bit?
RELEASE_LINKER_FLAGS= -flto

TARGET_DYNAMIC_IMPORT_LIB = $(addprefix $(dir $(TARGET_DYNAMIC_LIB)), $(addprefix lib, $(notdir $(TARGET_DYNAMIC_LIB).a)))

.PHONY: lib-static
.PHONY: lib-static-debug
.PHONY: lib-static-release
.PHONY: lib-dynamic
.PHONY: lib-dynamic-debug
.PHONY: lib-dynamic-release
.PHONY: lib-static-dynamic
.PHONY: lib-static-dynamic-debug
.PHONY: lib-static-dynamic-release
.PHONY: release
.PHONY: debug
.PHONY: test-release
.PHONY: test-debug
.PHONY: $(TARGET)
.PHONY: $(TEST_TARGET)
.PHONY: bin-clean
.PHONY: PRINT_MESSAGE1

all: release
lib-static: lib-static-release
lib-static-debug: DEFINES += $(DEBUG_DEFINES) -DBUILD_STATIC_LIBRARY
lib-static-debug: __STATIC_LIB_COMMAND = lib-static-debug
lib-static-debug: COMPILER_FLAGS += $(DEBUG_COMPILER_FLAGS)
lib-static-debug: LINKER_FLAGS += $(DEBUG_LINKER_FLAGS)
lib-static-debug: $(TARGET_STATIC_LIB)
lib-static-release: DEFINES += $(RELEASE_DEFINES) -DBUILD_STATIC_LIBRARY
lib-static-release: __STATIC_LIB_COMMAND = lib-static-release
lib-static-release: COMPILER_FLAGS += $(RELEASE_COMPILER_FLAGS)
lib-static-release: LINKER_FLAGS += $(RELEASE_LINKER_FLAGS)
lib-static-release: $(TARGET_STATIC_LIB)

lib-dynamic: lib-dynamic-release
lib-dynamic-debug: DEFINES += $(DEBUG_DEFINES) -DBUILD_DYNAMIC_LIBRARY
lib-dynamic-debug: __STATIC_LIB_COMMAND = lib-static-dynamic-debug
lib-dynamic-debug: COMPILER_FLAGS += $(DEBUG_COMPILER_FLAGS) -fPIC
lib-dynamic-debug: LINKER_FLAGS += $(DEBUG_LINKER_FLAGS) -fPIC
lib-dynamic-debug: $(TARGET_DYNAMIC_LIB)
lib-dynamic-release: DEFINES += $(RELEASE_DEFINES) -DBUILD_DYNAMIC_LIBRARY
lib-dynamic-release: __STATIC_LIB_COMMAND = lib-static-dynamic-release
lib-dynamic-release: COMPILER_FLAGS += $(RELEASE_COMPILER_FLAGS) -fPIC
lib-dynamic-release: LINKER_FLAGS += $(RELEASE_LINKER_FLAGS) -fPIC
lib-dynamic-release: COMPILER_FLAGS += -fPIC
lib-dynamic-release: LINKER_FLAGS += -fPIC
lib-dynamic-release: $(TARGET_DYNAMIC_LIB)

.PHONY: lib-dynamic-packed-debug
lib-dynamic-packed-debug: DEFINES += $(DEBUG_DEFINES) -DBUILD_DYNAMIC_LIBRARY
lib-dynamic-packed-debug: __STATIC_LIB_COMMAND = lib-static-dynamic-debug
lib-dynamic-packed-debug: COMPILER_FLAGS += $(DEBUG_COMPILER_FLAGS) -fPIC
lib-dynamic-packed-debug: LINKER_FLAGS += $(DEBUG_LINKER_FLAGS) -fPIC
lib-dynamic-packed-debug: $(TARGET_DYNAMIC_PACKED_LIB)

release: DEFINES += $(RELEASE_DEFINES) -DBUILD_EXECUTABLE
release: __STATIC_LIB_COMMAND = lib-static-release
release: COMPILER_FLAGS += $(RELEASE_COMPILER_FLAGS)
release: LINKER_FLAGS += $(RELEASE_LINKER_FLAGS)
release: $(TARGET)
test-release: DEFINES += $(RELEASE_DEFINES) -DBUILD_TEST
test-release: __STATIC_LIB_COMMAND = lib-static-release
test-release: COMPILER_FLAGS += $(RELEASE_COMPILER_FLAGS)
test-release: LINKER_FLAGS += $(RELEASE_LINKER_FLAGS)
test-release: $(TEST_TARGET)
debug: DEFINES += $(DEBUG_DEFINES) -DBUILD_EXECUTABLE
debug: __STATIC_LIB_COMMAND = lib-static-debug
debug: COMPILER_FLAGS += $(DEBUG_COMPILER_FLAGS)
debug: LINKER_FLAGS += $(DEBUG_LINKER_FLAGS)
debug: $(TARGET)
test-debug: DEFINES += $(DEBUG_DEFINES) -DBUILD_TEST
test-debug: __STATIC_LIB_COMMAND = lib-static-debug
test-debug: COMPILER_FLAGS += $(DEBUG_COMPILER_FLAGS)
test-debug: LINKER_FLAGS += $(DEBUG_LINKER_FLAGS)
test-debug: $(TEST_TARGET)


%.c.o : %.c
	@echo [Log] Compiling $< to $@
	$(C_COMPILER) $(C_COMPILER_FLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

%.cpp.o: %.cpp
	@echo [Log] Compiling $< to $@
	$(CPP_COMPILER) $(CPP_COMPILER_FLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

%.a:
	@echo [Log] Building $@ ...
	$(MAKE) --directory=$(subst lib/, ,$(dir $@)) $(__STATIC_LIB_COMMAND)
	@echo [Log] $@ built successfully!

$(TARGET_LIB_DIR):
	mkdir $@

PRINT_STATIC_INFO:
	@echo [Log] Building $(TARGET_STATIC_LIB) ...

PRINT_DYNAMIC_INFO:
	@echo [Log] Building $(TARGET_DYNAMIC_LIB) ...

$(TARGET_STATIC_LIB) : PRINT_STATIC_INFO $(OBJECTS) | $(TARGET_LIB_DIR)
	$(ARCHIVER) $(ARCHIVER_FLAGS) $@ $(filter-out $<, $^)
	@echo [Log] $@ built successfully!

$(TARGET_DYNAMIC_LIB) : PRINT_DYNAMIC_INFO $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS) $(OBJECTS) | $(TARGET_LIB_DIR)
	@echo [Log] Linking $@ ...
	$(LINKER) $(LINKER_FLAGS) $(DYNAMIC_LIBRARY_COMPILATION_FLAG) $(OBJECTS)  $(LIBS)\
	$(addprefix -L, $(dir $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS))) \
	$(addprefix -l:, $(notdir $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS))) \
	-o $@ $(DYNAMIC_IMPORT_LIBRARY_FLAG)$(TARGET_DYNAMIC_IMPORT_LIB)
	@echo [Log] $@ and lib$(notdir $@.a) built successfully!

build-all-static-libraries: $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS) $(TARGET_STATIC_LIB)
ALL_STATIC_LIBRARIES=$(addprefix -L, $(dir $(TARGET_STATIC_LIB) $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS))) \
	$(addprefix -l:, $(notdir $(TARGET_STATIC_LIB) $(__DEPENDENCY_LIBS) $(__SHARED_DEPENDENCY_LIBS))) $(LIBS) \

$(TARGET): build-all-static-libraries $(MAIN_OBJECT)
	@echo [Log] Linking $@ ...
	$(LINKER) $(LINKER_FLAGS) $(MAIN_OBJECT) $(ALL_STATIC_LIBRARIES) -o $@
	@echo [Log] $(PROJECT_NAME) built successfully!

$(TEST_TARGET): build-all-static-libraries $(TEST_OBJECT)
	@echo [Log] Linking $@ ...
	$(LINKER) $(LINKER_FLAGS) $(TEST_OBJECT) $(ALL_STATIC_LIBRARIES) -o $@
	@echo [Log] $(TEST_TARGET) built successfully!

RM := rm -f
RM_DIR := rm -rf

bin-clean:
	$(RM) $(OBJECTS)
	$(RM) $(MAIN_OBJECT)
	$(RM) $(TEST_OBJECT)
	$(RM) $(__EXECUTABLE_NAME)
	$(RM) $(__TEST_EXECUTABLE_NAME)
	$(RM) $(TARGET_STATIC_LIB)
	$(RM) $(TARGET_DYNAMIC_LIB)
	$(RM) $(TARGET_DYNAMIC_IMPORT_LIB)
	$(RM_DIR) $(TARGET_LIB_DIR)
	@echo [Log] Binaries cleaned successfully!
	$(MAKE) --directory=./../../shared-dependencies/BufferLib clean
	$(MAKE) --directory=./../../dependencies/SafeMemory/shared-dependencies/CallTrace clean
	$(MAKE) --directory=./../../dependencies/MeshLib/dependencies/DiskManager clean
# 	$(MAKE) --directory=../../shared-dependencies/HPML clean
#  	$(MAKE) --directory=./dependencies/tgc clean
#-------------------------------------------


#-------------------------------------------
#		Cleaning
#-------------------------------------------

.PHONY: clean-project-internal

clean-project-internal:
	$(MAKE) -f $(addsuffix .makefile, $(PROJECT_NAME)) clean

.PHONY: clean
clean: bin-clean clean-project-internal
	@echo [Log] All cleaned successfully!
#-------------------------------------------



.PHONY: build
.PHONY: build-run
.PHONY: build-release
.PHONY: build-debug
.PHONY: run

.PHONY: build-project-internal-debug
.PHONY: build-project-internal-release

build-project-internal-debug:
	$(MAKE) -f $(addsuffix .makefile, $(PROJECT_NAME)) debug

build-project-internal-release:
	$(MAKE) -f $(addsuffix .makefile, $(PROJECT_NAME)) release

build-release:
	$(MAKE) build-project-internal-release
	$(MAKE) lib-static-release
	$(MAKE) release

build-debug:
	$(MAKE) build-project-internal-debug
	$(MAKE) lib-static-debug
	$(MAKE) debug

build: build-debug

build-run: build
	$(__EXECUTABLE_NAME)

run: build-run
