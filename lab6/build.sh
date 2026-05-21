#!/bin/bash
echo "Buildin project.."
mkdir -p build
cd build
cmake ..
make
echo "Build completed"
