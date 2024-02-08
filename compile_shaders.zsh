#!/bin/zsh

VERT_DIR=shaders/vertex
FRAG_DIR=shaders/fragment
OUT_DIR=samples/triangle

glslc $VERT_DIR/triangle1.vert -o $OUT_DIR/triangle1.vert.spv
glslc $VERT_DIR/triangle2.vert -o $OUT_DIR/triangle2.vert.spv
glslc $FRAG_DIR/triangle.frag -o $OUT_DIR/triangle.frag.spv