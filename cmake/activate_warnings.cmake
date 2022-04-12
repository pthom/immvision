cmake_policy(SET CMP0054 NEW)

macro(immvision_activate_warnings)
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        # using Clang
        set(warning_flags "-Wall -Werror")

        set(ignored_warnings
            old-style-cast
            covered-switch-default
            switch-enum
            global-constructors
            exit-time-destructors
            zero-as-null-pointer-constant

            undef
            newline-eof
            missing-prototypes
            reserved-id-macro
            c++98-compat
            c++98-compat-pedantic
            padded
            weak-vtables
            )
        foreach(ignored_warning ${ignored_warnings})
            set(warning_flags "${warning_flags} -Wno-${ignored_warning}")
        endforeach()

        set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${warning_flags})
        message("set(CMAKE_CXX_FLAGS ${warning_flags})")

    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # using GCC
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
        # using Intel C++
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        # using Visual Studio C++
        set(warning_flags "/W4")

        set(ignored_warnings
            # no ignored warning
            )
        foreach(ignored_warning ${ignored_warnings})
            set(warning_flags "${warning_flags} /Wd-${ignored_warning}")
        endforeach()
        # Warning as error
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")

        add_compile_options(${warning_flags})
        message("add_compile_options(${warning_flags})")
    endif()
endmacro()
