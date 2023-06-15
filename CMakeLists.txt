cmake_minimum_required(VERSION 3.20)

# set the project name and version
project(imgui-testing VERSION 1.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# specify the C++ standard
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED True)


set(CMAKE_CXX_STANDARD            17)
set(CMAKE_CXX_STANDARD_REQUIRED   YES)

add_subdirectory(external/imgui)
add_subdirectory(external/implot)
add_subdirectory(external/imgui-node-editor)
list(APPEND LIBS imgui)
list(APPEND LIBS implot)
list(APPEND LIBS imgui-node-editor)
# list(APPEND INCLUDES "${PROJECT_SOURCE_DIR}/implot/implot")
# list(APPEND INCLUDES "${PROJECT_SOURCE_DIR}/imgui/imgui")
# list(APPEND INCLUDES "${PROJECT_SOURCE_DIR}/imgui-node-editor/imgui-node-editor")

add_executable(main main.cpp graph_layout.cpp)

# target_include_directories(main PUBLIC "${INCLUDES}")

target_link_libraries(main "${LIBS}")

# target_link_libraries(main imgui)
# target_link_libraries(main implot)
