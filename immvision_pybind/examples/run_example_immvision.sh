#!/usr/bin/env bash
THIS_DIR=`dirname "$0"`
echo "THIS_DIR=$THIS_DIR"
cd $THIS_DIR
export PYTHONPATH="/Users/pascal/dvp/OpenSource/ImGuiWork/immvision/cmake-build-debug/immvision_pybind:../src/immvision/"
python example_immvision.py
