#!/usr/bin/env bash

THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
EXTERNAL_DIR=$THIS_DIR/../external
cd $EXTERNAL_DIR
git clone https://github.com/pthom/imgui.git
cd imgui
git checkout docking_powersave

