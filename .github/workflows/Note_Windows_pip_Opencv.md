For pip, Opencv was precompiled, using the following commands:

```
mkdir pm
cd pm
git clone https://github.com/opencv/opencv.git
cd opencv
git checkout 4.5.5
cd ..
mkdir opencv_build
cd opencv_build

cmake -S ../opencv -B . `
-DCMAKE_INSTALL_PREFIX="../opencv_install_windows_pip" `
-DCMAKE_BUILD_TYPE=Release `
-DBUILD_SHARED_LIBS=OFF -DWITH_1394=OFF -DWITH_ADE=OFF `
-DWITH_VTK=OFF -DWITH_EIGEN=OFF -DWITH_FFMPEG=OFF -DWITH_GSTREAMER=OFF `
-DWITH_GTK=OFF -DWITH_GTK_2_X=OFF -DWITH_IPP=OFF -DWITH_JASPER=OFF     `
-DWITH_JPEG=ON -DWITH_WEBP=ON -DWITH_OPENEXR=OFF -DWITH_OPENGL=OFF `
-DWITH_OPENVX=OFF -DWITH_OPENNI=OFF -DWITH_OPENNI2=OFF -DWITH_PNG=ON `
-DWITH_TBB=OFF -DWITH_TIFF=OFF -DWITH_V4L=OFF -DWITH_OPENCL=OFF `
-DWITH_OPENCL_SVM=OFF -DWITH_OPENCLAMDFFT=OFF -DWITH_OPENCLAMDBLAS=OFF `
-DWITH_GPHOTO2=OFF -DWITH_LAPACK=OFF -DWITH_ITT=OFF -DWITH_QUIRC=ON `
-DBUILD_ZLIB=ON `
-DBUILD_opencv_apps=OFF -DBUILD_opencv_calib3d=ON -DBUILD_opencv_dnn=ON `
-DBUILD_opencv_features2d=ON -DBUILD_opencv_flann=ON -DBUILD_opencv_gapi=OFF `
-DBUILD_opencv_ml=OFF -DBUILD_opencv_photo=ON -DBUILD_opencv_imgcodecs=ON `
-DBUILD_opencv_shape=OFF -DBUILD_opencv_videoio=ON -DBUILD_opencv_videostab=OFF `
-DBUILD_opencv_highgui=ON -DBUILD_opencv_superres=OFF -DBUILD_opencv_stitching=OFF `
-DBUILD_opencv_java=OFF -DBUILD_opencv_js=OFF -DBUILD_opencv_python2=OFF `
-DBUILD_opencv_python3=OFF -DBUILD_EXAMPLES=OFF -DBUILD_PACKAGE=OFF `
`
-DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_DOCS=OFF -DWITH_PTHREADS_PF=OFF


cmake --build . --config Release --parallel 6
cmake --build . --target install --config Release
cd ..

7z a -t7z opencv_install_windows_pip.7z opencv_install_windows_pip
scp  opencv_install_windows_pip.7z pascal@traineq.org:HTML/ImmvisionGithubFiles/

```


And it can be used like this:
Note: 
        During development, don't forget to temporarily disable conan in pybind_cmake.cmake:
        set(IMMVISION_PYBIND_USE_CONAN OFF)
                instead of 
        set(IMMVISION_PYBIND_USE_CONAN ON)

```
python -m venv venv
 .\venv\Scripts\activate
$OpenCV_DIR=(pwd).Path + "/opencv_install_windows_pip"
$OpenCV_DIR=$OpenCV_DIR -replace "\\","/"
$env:OpenCV_DIR = $OpenCV_DIR
pip install -v -e /path/to/immvision_folder
```
