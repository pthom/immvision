FROM ubuntu:22.04

#ADD install_scripts/ /install_scripts/

# Windows / git annoyance : convert shell file to unix LF (not CRLF)
#RUN apt-get update && apt-get install -y dos2unix
#RUN dos2unix /install_scripts/*

#install_essentials
RUN apt-get update
RUN DEBIAN_FRONTEND="noninteractive" apt install -y \
   xz-utils \
   build-essential \
   cmake ccache default-jdk git \
   ninja-build python3 \
   curl wget \
   tree nano vim p7zip htop tmux

#install_cmake
RUN cd /tmp && \
    wget https://cmake.org/files/v3.23/cmake-3.23.0-rc1-linux-x86_64.sh &&\
    sh cmake-3.23.0-rc1-linux-x86_64.sh --skip-license --prefix=/usr/local &&\
    rm cmake-3.23.0-rc1-linux-x86_64.sh


RUN apt-get install -y curl zip unzip tar bison pkg-config python3-distutils gperf libx11-dev libxft-dev libxft-dev x11proto-xext-dev libxext-dev  libegl-dev psmisc


# install_x11vnc
RUN DEBIAN_FRONTEND="noninteractive"  apt-get install -y x11vnc xvfb icewm
RUN mkdir ~/.vnc &&\
    x11vnc -storepasswd xxx ~/.vnc/passwd &&\
    echo 'export DISPLAY=:0' >> ~/.bashrc
# /start_x_vnc.sh will start a "X server + vnc server + window manager" accessible via VNC
# -> you can connect to it via VNC on the port 5900 of the localhost
RUN echo "Xvfb -screen 0 1280x1024x16 & sleep 1 && icewm & x11vnc -forever  -ncache 10" > /start_x_vnc.sh &&\
    chmod +x /start_x_vnc.sh
RUN apt-get install -y libglapi-mesa

RUN apt-get update
RUN apt-get install -y libopencv-dev libsdl2-dev python3-pip python3-venv
RUN touch /IMMVISION_DOCKER_BUILDER


# Clear package cache
#RUN rm -rf /var/lib/apt/lists/*
