cmake_minimum_required(VERSION 3.16)

project(Cortex-M_Startup)

add_library(cortex-m_startup STATIC
  src/startup.cpp)

target_compile_features(cortex-m_startup
  PUBLIC
    cxx_std_14)

target_compile_options(cortex-m_startup
  PRIVATE
    -Wall
    -Wextra
    -Werror)

target_include_directories(cortex-m_startup
  PUBLIC
    inc)
target_compile_options(cortex-m_startup
  PRIVATE
    -Os)
target_link_options(cortex-m_startup 
  INTERFACE
    "SHELL:-Wl,-T ${CMAKE_CURRENT_SOURCE_DIR}/cortex-m.ld")
set_target_properties(cortex-m_startup
  PROPERTIES
  INTERFACE_LINK_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/cortex-m.ld)

