file(GLOB header *.h)
file(GLOB source *.cpp)
add_library(is-simplex SHARED ${header} ${source})


install(DIRECTORY ./
  DESTINATION include/sxxlib/is_mesh/simplex
  FILES_MATCHING PATTERN "*.h" REGEX ".svn.*" EXCLUDE)

geo_sim_sdk_install_cxx(TARGETS is-simplex)
