#!/bin/zsh

VERT_DIR=shaders/vertex
FRAG_DIR=shaders/fragment
OUT_DIR=samples/spv

glslc $VERT_DIR/default.vert -o $OUT_DIR/default.vert.spv
glslc $FRAG_DIR/default.frag -o $OUT_DIR/default.frag.spv