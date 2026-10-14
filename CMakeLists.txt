cmake_minimum_required(VERSION 3.14)
project(HospitalManagementSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

file(GLOB LIB_SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_library(hms_core STATIC ${LIB_SOURCES})
target_include_directories(hms_core PUBLIC include)

add_executable(hms src/main.cpp)
target_link_libraries(hms PRIVATE hms_core)

enable_testing()
add_executable(hms_tests tests/test_hospital.cpp)
target_link_libraries(hms_tests PRIVATE hms_core)
add_test(NAME unit_tests COMMAND hms_tests WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
