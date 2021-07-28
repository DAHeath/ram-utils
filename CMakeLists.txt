cmake_minimum_required (VERSION 3.10)
project (GORAM)
set(NAME "GORAM")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()


add_definitions(-Wfatal-errors)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-pthread -Wall -march=native -O3")

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(OpenSSL REQUIRED)

file(GLOB sources "src/*.cc")

add_library(${NAME} src/bit.cc src/prf.cc src/gate.cc)
target_include_directories(${NAME} PUBLIC src ${OPENSSL_INCLUDE_DIR})
target_link_libraries(${NAME} ${OPENSSL_LIBRARIES})

add_executable(${NAME}_main ${sources})
target_include_directories(${NAME}_main PUBLIC src ${OPENSSL_INCLUDE_DIR})
target_link_libraries(${NAME}_main ${NAME} ${OPENSSL_LIBRARIES})

add_subdirectory("test/rapidcheck")

add_executable(${NAME}_test test/bit.cc)
target_include_directories(${NAME}_test PUBLIC src ${OPENSSL_INCLUDE_DIR})
target_link_libraries(${NAME}_test ${OPENSSL_LIBRARIES} ${NAME} rapidcheck)
