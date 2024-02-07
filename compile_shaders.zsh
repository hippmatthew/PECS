#!/bin/zsh

VERT_DIR=shaders/vertex
FRAG_DIR=shaders/fragment
OUT_DIR=samples/triangle

glslc $VERT_DIR/triangle.vert -o $OUT_DIR/triangle.vert.spv
glslc $FRAG_DIR/triangle.frag -o $OUT_DIR/triangle.frag.spv
glslc $VERT_DIR/triangle2.vert -o $OUT_DIR/triangle2.vert.spv