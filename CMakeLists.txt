cmake_minimum_required(VERSION 4.1)
project(p2p_chat)

set(CMAKE_CXX_STANDARD 20)

set(WINDOWS_LIBS ws2_32 ole32 oleaut32)

add_executable(p2p_chat main.cpp)

target_link_libraries(p2p_chat $ {WINDOWS_LIBS})
