##
# widgets testing
#
# @file
# @version 0.1

IMGUI_DIR = ./imgui
# main: main.cpp
# 	g++ -I./imgui main.cpp -o main -lglfw -lGL

main: main.cpp
	g++ main.cpp -o main \
$(IMGUI_DIR)/imgui*.cpp $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp \
-I $(IMGUI_DIR) -I $(IMGUI_DIR)/backends \
-lglfw -lGL
# end
