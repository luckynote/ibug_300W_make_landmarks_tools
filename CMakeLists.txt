cmake_minimum_required(VERSION 3.1)
project(landmarks_tools)

set(CMAKE_CXX_STANDARD 11)
find_package(OpenCV REQUIRED)

include_directories(include/)
include_directories(include/tinyXML/)

aux_source_directory("src/" SOURCE)
aux_source_directory("src/tinyXML/" TINYXML_SOURCE)

add_executable(landmarks_tools main.cpp ${SOURCE} ${TINYXML_SOURCE})

target_link_libraries(landmarks_tools ${OpenCV_LIBS})