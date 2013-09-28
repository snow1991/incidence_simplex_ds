file(GLOB header *.h)
file(GLOB source *.cpp)
add_library(simplex SHARED ${header} ${source})
