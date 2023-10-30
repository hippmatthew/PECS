include .env

# Includes
INCLUDES = -I$(M_INC_DIR) -I$(CORE_DIR)/include

# Files
CORE_SRCS = $(wildcard $(CORE_DIR)/*.cpp)
CORE_BINS = $(patsubst $(CORE_DIR)/%.cpp, $(BIN_DIR)/%.core.o, $(CORE_SRCS))

LIB = $(LIB_DIR)/libpecs.a
BINS = $(CORE_BINS)

VERTS = $(wildcard $(SHDR_DIR)/vert/*.vert)
FRAGS = $(wildcard $(SHDR_DIR)/frag/*.frag)
COMPS = $(wildcard $(SHDR_DIR)/comp/*.comp)
SPVS = $(patsubst $(SHDR_DIR)/vert/%.vert, $(SHDR_DIR)/%.vert.spv, $(VERTS)) $(patsubst $(SHDR_DIR)/frag/%.frag, $(SHDR_DIR)/%.frag.spv, $(FRAGS)) $(patsubst $(SHDR_DIR)/comp/%.comp, $(SHDR_DIR)/%.comp.spv, $(COMPS))

# Functions
lib: $(LIB)
shaders: $(SPVS)

core: $(CORE_BINS)

sample: $(SMPL_DIR)/main

$(LIB_DIR)/libpecs.a: $(BINS)
	ar -rcs $@ $^

$(BIN_DIR)/%.core.o: $(CORE_DIR)/%.cpp
	$(CPP) -o $@ $(CPPFLAGS) $(INCLUDES) -c $<

$(SHDR_DIR)/%.vert.spv: $(SHDR_DIR)/vert/%.vert
	$(SC) -o $@ $<

$(SHDR_DIR)/%.frag.spv: $(SHDR_DIR)/frag/%.frag
	$(SC) -o $@ $<

$(SHDR_DIR)/%.comp.spv: $(SHDR_DIR)/comp/%.comp
	$(SC) -o $@ $<

$(SMPL_DIR)/main: $(SMPL_DIR)/main.cpp
	$(CPP) -o $@ $(CPPFLAGS) $(INCLUDES) $^ $(LDFLAGS)

clean:
	rm -f $(BIN_DIR)/*  $(SHDR_DIR)/*.spv rm $(LIB_DIR)/* rm $(SMPL_DIR)/main

clean_lib:
	rm -f $(LIB_DIR)/*

clean_spvs:
	rm -f $(SHDR_DIR)/*.spv

clean_sample:
	rm -f $(SMPL_DIR)/main