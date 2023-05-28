add_library(implot
  implot/implot.cpp
  implot/implot_demo.cpp
  implot/implot_items.cpp)
# add_library(implot implot.cpp) # implot_demo.cpp implot_items.cpp)

target_include_directories(implot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/implot)

# target_include_directories(implot PUBLIC
#                            "${PROJECT_SOURCE_DIR}/imgui"
#                            )

# find_package(OpenGL REQUIRED)
target_link_libraries(implot imgui)
# find_package(OpenGL REQUIRED)
# target_link_libraries(implot PUBLIC OpenGL::GL)
