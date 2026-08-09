cmake_minimum_required(VERSION 3.10)
project(DetectNotesChord)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(DetectNotesChord
    main.cpp
    detect_notes.cpp
)

target_link_libraries(DetectNotesChord Qt6::Widgets)
