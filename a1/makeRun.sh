#!/bin/bash
make
CENT=$(ls | grep -i "cube_centered.ply")
NORM=$(ls | grep -i "cube_normalized.ply")
if [ -n $CENT ];then
  rm cube_centered.ply
fi
if [ -n $NORM ];then
  rm cube_normalized.ply
fi
./geonorm ../models/cube.ply
