#include <cstdio>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_opengl3.h"

#include <iostream>

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void new_frames() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void my_window() {
  // fix window to left corner
  float size_y = ImGui::GetIO().DisplaySize.y;
  ImGui::SetNextWindowPos(ImVec2(0, 0)); // left corner
  ImGui::SetNextWindowSize(ImVec2(300, size_y));
  // std::cout << ImGui::GetIO().DisplaySize.x << " " << ImGui::GetIO().DisplaySize.y << "\n";
  // bool open = true;
  ImGui::Begin("MyWindow", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
  ImGui::Text("Hello, world %d", 123);
  // std::cout << open << "\n";
  ImGui::End();
}

// Main code
int main(int, char **) {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;
  // Initialize GLFW and create a window
  glfwInit();
  GLFWwindow *window =
      glfwCreateWindow(800, 600, "ImGui Window", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  // Initialize ImGui for GLFW and OpenGL
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  // ImGuiIO &io = ImGui::GetIO();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // Start a new ImGui frame
    new_frames();

    // Create your ImGui UI here
    my_window();
    // ImGui::ShowDemoWindow();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();
  // ImGui_ImplOpenGL3_Init("#version 330");
}
