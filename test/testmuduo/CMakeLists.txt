# cmake_minimum_required(VERSION 2.8)
# project(main)

# #配置编译选项
# set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

set(SRC_LIST ./muduo_server.cpp)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(server ${SRC_LIST})
target_link_libraries(server muduo_net muduo_base pthread)