include_geo_sim_sdk()
link_geo_sim_sdk()
file(GLOB header *.h)
file(GLOB source *.cpp)
add_library(is-property SHARED ${header} ${source})

install(DIRECTORY ./
  DESTINATION include/sxxlib/is_mesh/property
  FILES_MATCHING PATTERN "*.h" REGEX ".svn.*" EXCLUDE)

geo_sim_sdk_install_cxx(TARGETS is-property)
