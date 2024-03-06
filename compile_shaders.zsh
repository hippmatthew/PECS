#!/bin/zsh

VERT_DIR=shaders/vertex
FRAG_DIR=shaders/fragment
OUT_DIR=test/spv
NAME=test

glslc $VERT_DIR/$NAME.vert -o $OUT_DIR/$NAME.vert.spv
glslc $FRAG_DIR/$NAME.frag -o $OUT_DIR/$NAME.frag.spv