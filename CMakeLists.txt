cmake_minimum_required(VERSION 3.15)
project(chess-ai VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/Core/Bitboard.cpp
    src/Core/Board.cpp
    src/Core/Move.cpp
    src/Core/MoveGenerator.cpp
    src/Core/Evaluation.cpp
    src/Core/AI.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Window SFML::System)