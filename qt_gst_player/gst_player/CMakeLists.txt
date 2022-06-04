cmake_minimum_required(VERSION 3.5)
project(gst_player LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_PROJECT_NAME gst_player)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0)
pkg_check_modules(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0)
pkg_check_modules(gobject REQUIRED IMPORTED_TARGET gobject-2.0)
pkg_check_modules(glib REQUIRED IMPORTED_TARGET glib-2.0)

# shell中编译打开以下注释，Qt5_DIR设置为QT安装路径
#set(Qt5_DIR "/home/zhy/Qt5.13.2/5.13.2/gcc_64/lib/cmake/Qt5")
#find_package (Qt5Widgets)
#find_package (Qt5Core)
#find_package (Qt5Gui)

# qt create中编译，shell下编译需注释
find_package(Qt5 REQUIRED COMPONENTS Widgets )

include_directories(${PROJECT_SOURCE_DIR}/common/)
include_directories(${PROJECT_SOURCE_DIR}/libs/)
link_directories(${PROJECT_SOURCE_DIR}/libs/)
find_library (libpath mmplayer ${PROJECT_SOURCE_DIR}/libs/)


if (${libpath} STREQUAL "libpath-NOTFOUND")
    message (STATUS "required mmplayer library but not found!")
else()
    message (STATUS "libpath library found in ${libpath}")
endif()

add_executable(gst_player
  main.cpp
  playerwindow.cpp
  playerwindow.h
  common/common.h
  common/common.cpp
  libs/mediaPlayer.h
)

target_link_libraries(gst_player PRIVATE Qt5::Widgets PkgConfig::gstreamer PkgConfig::gstreamer-video PkgConfig::gobject PkgConfig::glib mmplayer)
