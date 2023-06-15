##
# widgets testing
#
# @file
# @version 0.1

IMGUI_DIR = ./imgui
IMPLOT_DIR = ./implot
NODE_EDITOR_DIR = ./imgui-node-editor
CXXFLAGS = -std=c++11 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(IMPLOT_DIR) -I$(NODE_EDITOR_DIR)
CXXFLAGS += -g -Wall -Wformat

# main: main.cpp
# 	g++ -I./imgui main.cpp -o main -lglfw -lGL

# main: main.cpp $(IMGUI_DIR)
# 	g++ main.cpp -o main \
# $(IMGUI_DIR)/imgui*.cpp $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp \
# -I $(IMGUI_DIR) -I $(IMGUI_DIR)/backends \
# -lglfw -lGL

# OBJS = $(IMGUI_DIR)/imgui*.o $(IMGUI_DIR)/backends/imgui_impl_glfw.o $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp \
#  -I$(IMGUI_DIR) -I $(IMGUI_DIR)/backends

SOURCES = main.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
# adding implot to sources
SOURCES += $(IMPLOT_DIR)/implot.cpp $(IMPLOT_DIR)/implot_demo.cpp $(IMPLOT_DIR)/implot_items.cpp
SOURCES += $(NODE_EDITOR_DIR)/imgui_node_editor.cpp \
$(NODE_EDITOR_DIR)/imgui_extra_math.inl $(NODE_EDITOR_DIR)/crude_json.cpp \
$(NODE_EDITOR_DIR)/imgui_node_editor_api.cpp $(NODE_EDITOR_DIR)/imgui_canvas.cpp \
$(NODE_EDITOR_DIR)/imgui_bezier_math.inl #$(NODE_EDITOR_DIR)/imgui_canvas.cpp

OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

ECHO_MESSAGE = "Linux"
LIBS += $(LINUX_GL_LIBS) `pkg-config --static --libs glfw3`
LIBS = -lglfw -lGL

CXXFLAGS += `pkg-config --cflags glfw3`
CFLAGS = $(CXXFLAGS)

%.o:%.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/%.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

%.o:$(IMPLOT_DIR)/%.cpp
	g++ $(CXXFLAGS) -c -o $@ $<


%.o:$(NODE_EDITOR_DIR)/%.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

# %.o: $(IMGUI_DIR)/backends/imgui_imp_glfw.cpp $(IMGUI_DIR)/backends/imgui_imp_opengl3.cpp
# 	g++ $(CXXFLAGS) -c -o $@ $<
%.o:$(IMGUI_DIR)/backends/%.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

main: $(OBJS)
	g++ -o $@ $^ $(CXXFLAGS) $(LIBS)
# g++ main.cpp -o main

clean:
	rm -f main $(OBJS)
# end
