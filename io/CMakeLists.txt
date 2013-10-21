include_geo_sim_sdk()
link_geo_sim_sdk()

file(GLOB header *.h)
file(GLOB source *.cpp)

add_library(is-mesh-io SHARED ${header} ${source})
target_link_libraries(is-mesh-io
  is-mesh
  jtf-mesh
  jtf-util
)

install(DIRECTORY ./
  DESTINATION include/sxxlib/is_mesh/io
  FILES_MATCHING PATTERN "*.h" REGEX ".svn.*" EXCLUDE)


geo_sim_sdk_install_cxx(TARGETS is-mesh-io)
