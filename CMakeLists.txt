cmake_minimum_required(VERSION 3.6)
project(Nineworlds)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        main.cpp
        3ds.cpp
        3ds.h
        BinTree.cpp
        BinTree.h
        bitmap.cpp
        bitmap.h
        Camera.cpp
        Camera.h
        DiamondSquare.cpp
        DiamondSquare.h
        LinMaths.cpp
        LinMaths.h
        Node.h
        SurfaceCreator.cpp
        SurfaceCreator.h
        Terrain.cpp
        Terrain.h
        Triangle.h
        vector.h)
add_executable(Nineworlds ${SOURCE_FILES})