cmake_minimum_required(VERSION 3.24)
project(DigitPixels VERSION 0.1.0 LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Use an out-of-source build: cmake -S . -B build/debug")
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV 4 REQUIRED CONFIG
    COMPONENTS core imgproc imgcodecs highgui ml)
if(NOT OpenCV_VERSION VERSION_LESS "5.0")
    message(FATAL_ERROR "This setup targets OpenCV 4.x")
endif()
message(STATUS "Using OpenCV ${OpenCV_VERSION} from ${OpenCV_DIR}")

add_library(digitpixels STATIC
    src/DigitPixels.cpp
    src/Preprocessing.cpp
)
target_compile_features(digitpixels PUBLIC cxx_std_17)
target_include_directories(digitpixels
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_include_directories(digitpixels SYSTEM PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(digitpixels PRIVATE ${OpenCV_LIBS})

add_executable(DigitRecognizer app/DigitRecognizer.cpp)
target_link_libraries(DigitRecognizer PRIVATE digitpixels)

foreach(target IN ITEMS digitpixels DigitRecognizer)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

