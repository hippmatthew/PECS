#!/bin/zsh

VERT_DIR=shaders/vertex
FRAG_DIR=shaders/fragment
OUT_DIR=samples/spv

glslc $VERT_DIR/fallingbox.vert -o $OUT_DIR/fallingbox.vert.spv
glslc $FRAG_DIR/fallingbox.frag -o $OUT_DIR/fallingbox.frag.spv