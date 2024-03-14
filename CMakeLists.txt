
cmake_minimum_required(VERSION 3.23)
project(webcam)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(webcam source/camera.h source/camera.cpp main.cpp)
target_link_libraries(webcam
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        )

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(webcam ${OpenCV_LIBS})
