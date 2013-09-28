file(GLOB header *.h)
file(GLOB source *.cpp)
add_library(mesh SHARED ${header} ${source})
