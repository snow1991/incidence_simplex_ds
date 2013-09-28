file(GLOB header *.h)
file(GLOB source *.cpp)
add_library(property SHARED ${header} ${source})
