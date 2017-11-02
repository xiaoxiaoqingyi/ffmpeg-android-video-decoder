

cmake_minimum_required(VERSION 3.4.1)

add_library(
             native-lib
             SHARED
             src/main/cpp/native-lib.cpp )


# FFmpeg library
add_library(avcodec SHARED IMPORTED)
set_target_properties( avcodec
              PROPERTIES IMPORTED_LOCATION
              src/main/cpp/libavcodec-57.so)

add_library(avdevice SHARED IMPORTED)
set_target_properties( avdevice
              PROPERTIES IMPORTED_LOCATION
              src/main/cpp/libavdevice-57.so)

add_library(avfilter SHARED IMPORTED)
set_target_properties( avfilter
            PROPERTIES IMPORTED_LOCATION
            src/main/cpp/libavfilter-6.so)

add_library(avformat SHARED IMPORTED)
set_target_properties( avformat
            PROPERTIES IMPORTED_LOCATION
            src/main/cpp/libavformat-57.so)

add_library(avutil SHARED IMPORTED)
set_target_properties( avutil
            PROPERTIES IMPORTED_LOCATION
            src/main/cpp/libavutil-55.so)

add_library(postproc SHARED IMPORTED)
set_target_properties( postproc
            PROPERTIES IMPORTED_LOCATION
            src/main/cpp/libpostproc-54.so)

add_library(swresample SHARED IMPORTED)
set_target_properties( swresample
            PROPERTIES IMPORTED_LOCATION
            src/main/cpp/libswresample-2.so)

add_library(swscale SHARED IMPORTED)
set_target_properties( swscale
            PROPERTIES IMPORTED_LOCATION
            src/main/cpp/libswscale-4.so)

#jpegLib
add_library(jpegbither SHARED IMPORTED)
set_target_properties( jpegbither
            PROPERTIES IMPORTED_LOCATION
            src/main/cpp/libjpegbither.so)

#inclue header file
include_directories(src/main/cpp/include)


find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )