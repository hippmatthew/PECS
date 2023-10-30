include .env

CPP = clang++
SC = glslc

CPPFLAGS = -O2 -std=c++20 `pkg-config --cflags vulkan glfw3`
LDFLAGS = `pkg-config --libs vulkan glfw3` -L$(LOCAL_LIB) -lpecs

LDFLAGS += -rpath /usr/local/lib

# Files
SRCS = $(wildcard $(LOCAL_SRC)/*.cpp)
BINS = $(patsubst $(LOCAL_SRC)/%.cpp, $(LOCAL_BIN)/%.o, $(SRCS))
LIB = $(LOCAL_LIB)/libpecs.a

VERTS = $(wildcard $(LOCAL_SHDRS)/vert/*.vert)
FRAGS = $(wildcard $(LOCAL_SHDRS)/frag/*.frag)
SPVS = $(patsubst $(LOCAL_SHDRS)/vert/%.vert, $(LOCAL_SHDRS)/%.vert.spv, $(VERTS)) $(patsubst $(LOCAL_SHDRS)/frag/%.frag, $(LOCAL_SHDRS)/%.frag.spv, $(FRAGS))

# Functions
archive: CPPFLAGS += -I$(LOCAL_SRC)/include
archive: $(LIB)

shaders: $(SPVS)

sample: CPPFLAGS += -I$(LOCAL_MASTER_INCLUDE)
sample: $(LOCAL_SMPL)/main

$(LOCAL_LIB)/libpecs-debug.a: $(BINS)
	ar -rcs $@ $^

$(LOCAL_LIB)/libpecs.a: $(BINS)
	ar -rcs $@ $^

$(LOCAL_BIN)/%.o: $(LOCAL_SRC)/%.cpp
	$(CPP) -o $@ $(CPPFLAGS) -c $< $(LD_FLAGS)

$(LOCAL_SHDR)/%.vert.spv: $(LOCAL_SHDR)/vert/%.vert
	$(SC) -o $@ $<

$(LOCAL_SHDR)/%.frag.spv: $(LOCAL_SHDR)/frag/%.frag
	$(SC) -o $@ $<

$(LOCAL_SMPL)/main: $(LOCAL_SMPL)/main.cpp
	$(CPP) -o $@ $(CPPFLAGS) -I$(LOCAL_MASTER_INCLUDE) $^ $(LDFLAGS)

clean:
	rm -f $(LOCAL_BIN)/*  $(LOCAL_SHDR)/*.spv rm $(LOCAL_LIB)/* rm $(LOCAL_SMPL)/main

clean_lib:
	rm -f $(LOCAL_LIB)/*

clean_sample:
	rm -f $(LOCAL_SMPL)/main

clean_spvs:
	rm -f $(LOCAL_SHDR)/*.spv
