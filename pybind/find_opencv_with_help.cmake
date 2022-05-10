function(find_opencv_with_help)
  if(DEFINED ENV{OpenCV_DIR})
    set(OpenCV_DIR $ENV{OpenCV_DIR})
    message(WARNING "Got OpenCV_DIR from enviroment: ${OpenCV_DIR}")
  endif()

  #
  # Add OpenCv via conan, if not found on the machine
  #
  set(opencv_install_help "
  Could not find OpenCV. Please install it first.

  1. Either via a global installation
  Under ubuntu, you can install it with apt:
      sudo apt-get install libopencv-dev

  2. Or via a local installation, using conan (https://conan.io/):
  First, install conan:
      pip install conan
      conan profile new default --detect

  If (and only if) you are using gcc, it is also recommended to run:
      conan profile update settings.compiler.libcxx=libstdc++11 default

  Then, install OpenCV for this package via:
      mkdir -p /tmp/foo
      cd /tmp/foo
      # For linux, run:
      conan install ${THIS_DIR}/conanfile_linux.txt --build=missing
      # For other platforms, run:
      conan install ${THIS_DIR}/conanfile.txt --build=missing
  ")

  find_package(OpenCV) # test if opencv can be found
  if (NOT OpenCV_FOUND)
    set(IMMVISION_PYBIND_USE_CONAN ON)
    message(WARNING "Did not find a global OpenCV installation. Will try to install it via conan")
  endif()
  if (IMMVISION_PYBIND_USE_CONAN)
    set(conan_folder ${CMAKE_CURRENT_BINARY_DIR}/conan_third)
    file(MAKE_DIRECTORY ${conan_folder})
    if(UNIX AND NOT APPLE)
      set(conanfile "conanfile_linux.txt")
    else()
      set(conanfile "conanfile.txt")
    endif()

    execute_process(COMMAND
        conan install ${PROJECT_SOURCE_DIR}/pybind/${conanfile}
        WORKING_DIRECTORY ${conan_folder}
        RESULT_VARIABLE conan_install_result
        )
    if (NOT ${conan_install_result} EQUAL "0")
      message(WARNING "conan_install_result=${conan_install_result}")
      message(FATAL_ERROR ${opencv_install_help})
    endif()
    # For conan, add binary dir to module search path
    list(APPEND CMAKE_MODULE_PATH ${conan_folder})
  endif()

  find_package(OpenCV)
  if (NOT OpenCV_FOUND)
    message(FATAL_ERROR ${opencv_install_help})
  endif()
endfunction()