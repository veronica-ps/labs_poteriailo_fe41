#!/bin/bash
echo "Buildin project.."
mkdir -p build
cp deploy.prototxt build/ 2>/dev/null
cp res10_300x300_ssd_iter_140000.caffemodel build/ 2>/dev/null
cp image6.png build/ 2>/dev/null
cd build
cmake ..
make
echo "Build completed"
