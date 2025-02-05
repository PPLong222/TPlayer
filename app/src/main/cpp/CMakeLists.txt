# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("tplayer")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.

set(ffmpeg_lib_dir ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})
set(ffmpeg_head_dir ${CMAKE_SOURCE_DIR})

include_directories(${ffmpeg_head_dir}/include)

add_library(avutil SHARED IMPORTED)
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION ${ffmpeg_lib_dir}/libavutil.so)
add_library(swresample SHARED IMPORTED)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION ${ffmpeg_lib_dir}/libswresample.so)
add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION ${ffmpeg_lib_dir}/libavcodec.so)
add_library(avfilter SHARED IMPORTED)
set_target_properties(avfilter PROPERTIES IMPORTED_LOCATION ${ffmpeg_lib_dir}/libavfilter.so)
add_library(swscale SHARED IMPORTED)
set_target_properties(swscale PROPERTIES IMPORTED_LOCATION ${ffmpeg_lib_dir}/libswscale.so)
add_library(avformat SHARED IMPORTED)
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION ${ffmpeg_lib_dir}/libavformat.so)
add_library(avdevice SHARED IMPORTED)
set_target_properties(avdevice PROPERTIES IMPORTED_LOCATION ${ffmpeg_lib_dir}/libavdevice.so)

add_library(${CMAKE_PROJECT_NAME} SHARED
        # List C/C++ source files with relative paths to this CMakeLists.txt.
        native-lib.cpp
        # util
        ${ffmpeg_head_dir}/util/logger.h
        ${ffmpeg_head_dir}/util/time.h
        # player
        ${ffmpeg_head_dir}/media/player/player.cpp
        ${ffmpeg_head_dir}/media/player/opengl_player.cpp
        # decoder
        ${ffmpeg_head_dir}/media/one_frame.h
        ${ffmpeg_head_dir}/media/decode_state.h
        ${ffmpeg_head_dir}/media/decoder/i_decoder.h
        ${ffmpeg_head_dir}/media/decoder/base_decoder.cpp
        ${ffmpeg_head_dir}/media/decoder/video/v_decoder.cpp
        ${ffmpeg_head_dir}/media/decoder/audio/a_decoder.cpp
        # render
        ${ffmpeg_head_dir}/media/render/video/video_render.h
        ${ffmpeg_head_dir}/media/render/video/native_render.cpp
        ${ffmpeg_head_dir}/media/render/video/opengl/opengl_render.cpp
        ${ffmpeg_head_dir}/media/render/video/opengl/opengl_pixel_receiver.h
        ${ffmpeg_head_dir}/media/render/audio/audio_render.h
        ${ffmpeg_head_dir}/media/render/audio/open_sl_render.cpp
        ${ffmpeg_head_dir}/test/test.cpp

        ${ffmpeg_head_dir}/opengl/egl/egl_core.cpp
        ${ffmpeg_head_dir}/opengl/egl/egl_surface.cpp

        ${ffmpeg_head_dir}/opengl/drawer/drawer.cpp
        ${ffmpeg_head_dir}/opengl/drawer/video_drawer.cpp
        ${ffmpeg_head_dir}/opengl/drawer/proxy/drawer_proxy.cpp
        ${ffmpeg_head_dir}/opengl/drawer/proxy/drawer_proxy_impl.cpp
)



# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME}
        # List libraries link to the target library
        android
        log
        avutil
        swresample
        avcodec
        avfilter
        swscale
        avformat
        avdevice
        # Really need this one?
        -landroid
        OpenSLES
        -lEGL
        -lGLESv2
)





