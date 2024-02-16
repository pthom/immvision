#!/usr/bin/env bash

if [ $(uname) = 'Linux' ]; then
  sudo apt-get update && sudo apt-get install -y xorg-dev
  sudo apt-get install libgtk2.0-dev libglfw3-dev
fi
