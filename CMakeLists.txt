cmake_minimum_required(VERSION 2.8)
project(crawler)
aux_source_directory(. DIR_SRCS)
add_executable(crawler ${DIR_SRCS})
target_link_libraries(crawler event)