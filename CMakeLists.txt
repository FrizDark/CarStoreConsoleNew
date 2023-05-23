cmake_minimum_required(VERSION 3.25)
project(CarStore)

set(CMAKE_CXX_STANDARD 23)

include_directories(/usr/local/Cellar/boost/1.81.0_1/include)
link_directories(/usr/local/Cellar/boost/1.81.0_1/lib)

add_executable(CarStore main.cpp model/common/Includes.h model/object/Object.cpp model/object/Object.h model/table/BasicSerializable.cpp model/table/BasicSerializable.h model/table/Serializable.h model/table/View.cpp model/table/View.h model/common/Extensions.h model/ui/UserInterface.cpp model/ui/UserInterface.h)

add_subdirectory(Tests)
