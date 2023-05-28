add_library(imgui-node-editor
  imgui-node-editor/crude_json.cpp
  imgui-node-editor/imgui_bezier_math.inl
  imgui-node-editor/imgui_canvas.cpp
  imgui-node-editor/imgui_canvas.cpp
  imgui-node-editor/imgui_extra_math.inl
  imgui-node-editor/imgui_node_editor_api.cpp
  imgui-node-editor/imgui_node_editor_internal.inl
  imgui-node-editor/imgui_node_editor.cpp
  imgui-node-editor/misc/imgui_node_editor.natvis
)

target_include_directories(imgui-node-editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui-node-editor)

# target_include_directories(imgui-node-editor PUBLIC
#                            "${PROJECT_SOURCE_DIR}/imgui/imgui"
#                            )

target_link_libraries(imgui-node-editor imgui)
