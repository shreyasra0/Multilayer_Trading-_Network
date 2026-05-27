cmake_minimum_required(VERSION 3.16)
project(HFT_Engine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -ffast-math -flto -DNDEBUG")

include_directories(include)

add_executable(hft_engine 
    src/main.cpp 
    src/network/kernel_bypass.cpp
    src/core/order_book.cpp
    src/protocols/ouch_encoder.cpp
    src/protocols/fix_session.cpp
)

target_link_libraries(hft_engine PRIVATE pthread)
