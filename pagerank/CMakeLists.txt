cmake_minimum_required(VERSION 2.8)
project(pagerank)
aux_source_directory(. DIR_SRCS)
add_executable(pagerank ${DIR_SRCS})