TARGET ?= Project.exe
SRC_DIR ?= src/
BUILD_DIR ?= build/
EXT:=cpp
DEBUG_FLAGS = -Og
RELEASE_FLAGS = -O2 # -mwindows

# https://github.com/LaurentTreguier/Makefile/blob/master/Makefile
search-dir=$(filter-out $1,$(dir $(wildcard $1*/)))
# Returns the list of subdirectories in Arg1 directory
search-dir-all=$(strip $(call search-dir,$1) $(foreach DIR,$(call search-dir,$1),$(call search-dir-all,$(DIR))))
# Returns the list of directories and subdirectories in Arg1, including the original directory.
search-file=$(foreach DIR,$1,$(wildcard $(DIR)*.$(EXT)))
# Returns the list of files in Arg1 directories and their subdirectories.

ALL_SRC_DIRS := $(call search-dir-all,$(SRC_DIR)) $(SRC_DIR)
SRCS := $(call search-file,$(ALL_SRC_DIRS))
OBJS := $(addsuffix .o,$(basename $(patsubst $(SRC_DIR)%,$(BUILD_DIR)%,$(SRCS))))
DEPS := $(OBJS:.o=.d)

SFMLDIR= libs/SFML-2.5.1-MinGW-W64-x86_64-posix-seh-gcc10.2.0
INC_FLAGS := -I$(SFMLDIR)/include 
LDFLAGS = -L$(SFMLDIR)/lib -L$(SFMLDIR)/extlibs/libs-mingw/x64  

SFML_DEPENDENCIES = -lopengl32 -lfreetype -lwinmm -lgdi32 -lws2_32 -lopenal32 -lflac -lvorbisenc -lvorbisfile -lvorbis -logg 
SFML_MODULES = -lsfml-main -lsfml-network-s -lsfml-audio-s -lsfml-graphics-s -lsfml-window-s  -lsfml-system-s 
SFML_DEBUG_MODULES = -lsfml-main-d -lsfml-network-s-d -lsfml-audio-s-d -lsfml-graphics-s-d -lsfml-window-s-d  -lsfml-system-s-d 

CXXFLAGS_BARE = -std=c++17 -static -DSFML_STATIC -Wall
CXXFLAGS = $(CXXFLAGS_BARE)

.PHONY: debug release clean

# debug configuration, no optimizations, console application, debug modules
debug: CXXFLAGS := $(CXXFLAGS) $(DEBUG_FLAGS)
debug: LDLIBS = $(SFML_DEBUG_MODULES) $(SFML_DEPENDENCIES)
debug: $(TARGET)

# release configuration, no asserts, with optimizations, windows application
release: CXXFLAGS := $(CXXFLAGS) $(RELEASE_FLAGS)
release: LDLIBS = $(SFML_MODULES) $(SFML_DEPENDENCIES)
release: $(TARGET)

$(TARGET): $(OBJS)
	@echo %TIME% Building program.
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)
	@echo %TIME% Program built. 

$(BUILD_DIR)%.o: $(SRC_DIR)%.cpp
	@if not exist $(subst /,\,$(dir $@)) (mkdir $(subst /,\,$(dir $@)))
	@$(CXX) $(OBJ_GENERATION_FLAGS) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@
	@echo %TIME% Created $@ 

clean:
	@if exist $(TARGET) (del $(TARGET) && echo Deleted old build. $(TARGET))
	@if exist $(subst /,\,$(BUILD_DIR)) (echo Will delete: && rd $(subst /,\,$(BUILD_DIR)) /S && echo Deleted build folder $(BUILD_DIR))

-include $(DEPS)
