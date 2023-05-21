#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <cstdio>

#include <iostream>
#include <random>
#include <string>
#include <vector>

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void new_frames() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

struct Point {
  float x, y, z;
  // Overload the << operator for Person struct
  friend std::ostream &operator<<(std::ostream &os, const Point &p) {
    os << "{ x: " << p.x << ", y: " << p.y << ", z: " << p.z << " }";
    return os;
  }
};

typedef struct Mesh {
  const char *string;
  std::vector<Point> points;

  friend std::ostream &operator<<(std::ostream &os, const Mesh &m) {
    os << "{ name: " << m.string << ", \n";
    for (const auto &point : m.points) {
      os << "point: " << point << ",\n";
    }
    os << "}\n";
    return os;
  }
} Mesh;

Mesh *create_random_mesh(const char *s, int nb_points) {
  Mesh *m = new Mesh;
  m->string = s;
  std::random_device rd;
  std::mt19937 gen(rd());
  // Define the range of random numbers
  std::uniform_real_distribution<float> distribution(1, 100);
  // Generate a random number
  // int randomNum = distribution(gen);
  Point p;
  for (int i = 0; i < nb_points; i++) {
    p.x = distribution(gen);
    p.y = distribution(gen);
    p.z = distribution(gen);
    (m->points).emplace_back(p);
  }
  return m;
};

// void inspector_window


void my_window(Mesh **meshes, int len) {
  // fix window to left corner
  static bool object_inspector_active = false;
  float size_y = ImGui::GetIO().DisplaySize.y;
  if (object_inspector_active) size_y = size_y/2;
  std::cout << size_y << std::endl;
  ImGui::SetNextWindowPos(ImVec2(0, 0)); // left corner
  ImGui::SetNextWindowSize(ImVec2(300, size_y));
  // std::cout << ImGui::GetIO().DisplaySize.x << " " <<
  // ImGui::GetIO().DisplaySize.y << "\n"; bool open = true;
  int flags = 0;
  flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
  flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
  ImGui::Begin("MyWindow", nullptr, flags);

  // ImGui::Text("Hello, world %d", 123);
  if (ImGui::TreeNode("test")) {
    ImGui::Text("heeeellllo");
    ImGui::TreePop();
  }
  ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                  ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                  ImGuiTreeNodeFlags_SpanAvailWidth;
  static int selection_mask = 0; //(1 << 2);
  int node_clicked = -1;

  for (int i = 0; i < len; i++) {
    Mesh *m = meshes[i];
    ImGuiTreeNodeFlags node_flags = base_flags;
    const bool is_selected = (selection_mask & (1 << i)) != 0;
    if (is_selected)
      node_flags |= ImGuiTreeNodeFlags_Selected;
    bool node_open =
        ImGui::TreeNodeEx((void *)(intptr_t)i, node_flags, "Mesh %d", i);
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
      node_clicked = i;
    if (node_open) {
      object_inspector_active = true;
      ImGui::SetNextWindowPos(ImVec2(0, size_y));
      ImGui::SetNextWindowSize(ImVec2(300, size_y));
      if (ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        // if (ImGui::BeginTable("table1", 3))
        int j = 0;
        for (const auto &point : m->points) {
          // if (ImGui::TreeNode(&point, "point")) {
          //   ImGui::Text("{ x: %d, y: %d, z: %d }", point.x, point.y, point.z);
          //   ImGui::TreePop();
          // }
          // if (ImGui::TreeNode(&point, "point")) {
            // static float vec4f[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
            char label[64];
            sprintf(label, "##point: %d", j);// static int vec3i[3] = { 10, 20, 30 };
            ImGui::InputScalarN(label, ImGuiDataType_Float, (void *)&point, 3, NULL, NULL, NULL, ImGuiInputTextFlags_ReadOnly);
            // ImGui::Text("{ x: %d, y: %d, z: %d }", point.x, point.y, point.z);
            // ImGui::TreePop();
          // }
          j++;
        }
        ImGui::End();
      }
    }
    if (node_open) {
      // ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnArrow", &base_flags,
      // ImGuiTreeNodeFlags_OpenOnArrow);
      // ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnDoubleClick",
      // &base_flags, ImGuiTreeNodeFlags_OpenOnDoubleClick);
      // ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanAvailWidth", &base_flags,
      // ImGuiTreeNodeFlags_SpanAvailWidth); ImGui::SameLine();
      // HelpMarker("Extend hit area to all available width instead of allowing
      // more items to be laid out after the node.");
      // ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanFullWidth", &base_flags,
      // ImGuiTreeNodeFlags_SpanFullWidth);
      // for each point create sub node
      // if
      for (const auto &point : m->points) {
        if (ImGui::TreeNode(&point, "point")) {
          ImGui::Text("{ x: %f, y: %f, z: %f }", point.x, point.y, point.z);
          ImGui::TreePop();
        }
      }
      // }
      ImGui::TreePop();
    }
  }
  if (node_clicked != -1) {
    // Update selection state
    // (process outside of tree loop to avoid visual inconsistencies during the
    // clicking frame) if (ImGui::GetIO().KeyCtrl)
    //     selection_mask ^= (1 << node_clicked);          // CTRL+click to
    //     toggle
    // else //if (!(selection_mask & (1 << node_clicked))) // Depending on
    // selection behavior you want, may want to preserve selection when clicking
    // on item that is part of the selection
    selection_mask = (1 << node_clicked); // Click to single-select
  }

  // std::cout << open << "\n";
  ImGui::End();
}
// Main code
int main(int, char **) {
  // create some random meshes
  Mesh *meshes[10];
  for (int i = 0; i < 10; i++) {
    meshes[i] = create_random_mesh("Mesh", 5);
  }
  // int n= 5;
  // std::cout << "Mesh" + std::string(n) ;

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
    my_window(meshes, 10);
    ImGui::ShowDemoWindow();

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
