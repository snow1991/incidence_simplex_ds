include_geo_sim_sdk()
link_geo_sim_sdk()

file(GLOB header *.h)
file(GLOB source *.cpp)
add_library(is-mesh SHARED ${header} ${source})

target_link_libraries(is-mesh
  is-simplex
  is-property
  is-common
)

install(DIRECTORY ./
  DESTINATION include/sxxlib/is_mesh/mesh
  FILES_MATCHING PATTERN "*.h" REGEX ".svn.*" EXCLUDE)

geo_sim_sdk_install_cxx(TARGETS is-mesh)
