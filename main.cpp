#include <cstdio>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_opengl3.h"

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
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
  // IMGUI_CHECKVERSION();
  // ImGui::CreateContext();
  // ImGuiIO &io = ImGui::GetIO();
  // ImGui_ImplGlfw_InitForOpenGL(window, true);
  // ImGui_ImplOpenGL3_Init()
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // Start a new ImGui frame
    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();

    // Create your ImGui UI here

    // Rendering
    // ImGui::Render();
    glViewport(0, 0, 500, 500);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // Cleanup
  // ImGui_ImplOpenGL3_Shutdown();
  // ImGui_ImplGlfw_Shutdown();
  // ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();
  // ImGui_ImplOpenGL3_Init("#version 330");
}
