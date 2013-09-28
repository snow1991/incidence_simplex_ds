file(GLOB header *.h)
file(GLOB source *.cpp)
add_library(common SHARED ${header} ${source})
