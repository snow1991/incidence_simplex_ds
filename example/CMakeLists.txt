cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{HOME}/usr/share/cmake/Modules/")
include($ENV{HOME}/usr/share/cmake/Modules/geo_sim_sdk.cmake)
link_directories($ENV{HOME}/usr/lib)

include_geo_sim_sdk()
link_geo_sim_sdk()

file(GLOB header *.h)
file(GLOB source *.cpp)

add_executable(example ${header} ${source})

target_link_libraries(example
is-mesh-topology-operation
is-mesh-io
is-mesh
is-property
is-simplex
is-common
jtf-mesh)
