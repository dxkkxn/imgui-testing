#define IMGUI_DEFINE_MATH_OPERATORS
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "imgui_node_editor.h"
#include "imgui_node_editor_internal.h"
#include "implot.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <cstdio>

#include "icons.h"
#include <array>
#include <cstring>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "graph_layout.h"
#include <boost/range/combine.hpp>

#define WINDOW_SIZE 300

namespace ed = ax::NodeEditor;

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
  std::string info_logs;
  std::string warning_logs;
  std::string error_logs;
  friend std::ostream &operator<<(std::ostream &os, const Mesh &m) {
    os << "{ name: " << m.string << ", \n";
    for (const auto &point : m.points) {
      os << "point: " << point << ",\n";
    }
    os << "}\n";
    return os;
  }
} Mesh;

// ImGui::Begin("Custom Tree Node");

typedef unsigned long long ctime_t;
struct TaskExecInfo
{
    using TimeInterval = std::pair<ctime_t,ctime_t>;
    // using Color = ;

    int          execThreadIndex;
    TimeInterval execTime;
    ImVec4 color;
    std::string  name;

    inline int getExecThreadIndex() const { return execThreadIndex; }
    inline const TimeInterval& getExecTime() const { return execTime; }
    inline ctime_t getExecDuration() const { return execTime.second - execTime.first; }
    // inline const Color& getColor() const { return color; }
    inline const std::string& getName() const { return name; }
};
std::array<std::vector<TaskExecInfo>, 8> thread_pool;

std::string generateLoremIpsum(int numWords) {
  std::vector<std::string> words = {
      "Lorem",       "ipsum",      "dolor",      "sit",   "amet",
      "consectetur", "adipiscing", "elit",       "sed",   "do",
      "eiusmod",     "tempor",     "incididunt", "ut",    "labore",
      "et",          "dolore",     "magna",      "aliqua"};

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, words.size() - 1);

  std::stringstream ss;
  for (int i = 0; i < numWords; ++i) {
    if (i > 0)
      ss << ' ';
    ss << words[dis(gen)];
  }

  return ss.str();
}
void init_thread_pool() {
  for (int i = 0; i < thread_pool.size(); i++) {
    auto& thread = thread_pool[i];
    std::random_device rd;
    std::mt19937 gen(rd());
    // Define the range of random numbers
    std::uniform_int_distribution<int> dis(1, 5);
    int random = dis(gen);
    // thread.resize(random);
    int last = 0;
    for(int j = 0; j < random; j++) {
      std::uniform_int_distribution<int> dis(1, 100000);
      std::uniform_real_distribution<float> float_dis(0.0, 1.0);
      int sleep_time = dis(gen); // sleep time between the last taks and the current one
      int duration = dis(gen); // duration of the task;
      ImVec4 random_vec = ImVec4(float_dis(gen), float_dis(gen), float_dis(gen), 1);
      TaskExecInfo task = {i, {last + sleep_time, last+sleep_time+duration}, random_vec, generateLoremIpsum(4)};
      thread.emplace_back(task);
      last = last+sleep_time+duration;
    }
  }

  // for (auto task: thread_pool[0]) {
  //   std::cout << ">>>task name: " << task.name << std::endl;
  // }
}
bool MyTreeNode(const char *label) {
  const ImGuiStyle &style = ImGui::GetStyle();
  ImGuiStorage *storage = ImGui::GetStateStorage();

  ImU32 id = ImGui::GetID(label);
  int opened = storage->GetInt(id, 0);
  float x = ImGui::GetCursorPosX();
  ImGui::BeginGroup();
  if (ImGui::InvisibleButton(label,
                             ImVec2(-1, 13 + style.FramePadding.y * 2))) {
    int *p_opened = storage->GetIntRef(id, 0);
    opened = *p_opened = !*p_opened;
  }
  bool hovered = ImGui::IsItemHovered();
  bool active = ImGui::IsItemActive();
  if (hovered || active)
    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
        ImColor(ImGui::GetStyle().Colors[active ? ImGuiCol_HeaderActive
                                                : ImGuiCol_HeaderHovered]));

  // Icon, text
  ImGui::SameLine(x);
  ImGui::ColorButton("buttton",
                     opened ? ImColor(255, 0, 0) : ImColor(0, 255, 0));
  ImGui::SameLine();
  ImGui::Text("%s", label);
  ImGui::EndGroup();
  if (opened)
    ImGui::TreePush(label);
  return opened != 0;
};

bool **create_random_graph(const int n) {
  bool **graph = new bool *[n];
  for (int i = 0; i < n; i++) {
    graph[i] = new bool[n];
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  // Define the range of random numbers
  std::bernoulli_distribution distribution(0.5);

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      graph[i][j] = distribution(gen);
    }
  }
  return graph;
}

graph_t example_wiki_graph() {
  const char * labels[] = {"DC++",
                         "LinuxDC++",

                         "BCDC++",
                         "FreeDC++",
                         "StrongDC++",
                         "BMDC++",
                         "EiskaltDC++",

                         "AirDC++",
                         "zK++",
                         "ApexDC++",
                         "TkDC++",
                         "RSX++",

                         "StrongDC++ SQLite",
                         "ApexDC++ Speed-Mod",
                         "DiCe!++",
                         "FlylinkDC++ \n ver <= 4xx",
                         "GreylinkDC++",

                         "FlylinkDC++ \n ver >= 5xx",
                         "AvaLink",
                         "RayLinkDC++",
                         "SparkDC++",
                         "PeLink"};

  graph_t new_graph(labels, 22);
  // new_graph.labels = std::vector<std::string>(node_labels, node_labels+22);
  // graph[0][1] = graph[0][6] = graph[0][2] = graph[0][4] = true;
  new_graph.nodes[0] = {1,2,4,6};
  // graph[1][3] = true;
  new_graph.nodes[1] = {3};
  // graph[3][5] = graph[3][6] = true;
  new_graph.nodes[3] = {5, 6};
  // graph[2][4] = true;
  new_graph.nodes[2] = {4};
  // graph[4][7] = graph[4][8] = graph[4][9] = graph[4][10] = graph[4][11] = graph[4][12] = true;
  new_graph.nodes[4] = {7, 8, 9, 10, 11, 12};
  // graph[9][13] = graph[9][14] = graph[9][15] = true;
  new_graph.nodes[9] = {13, 14, 15};
  // graph[12][17] = true;
  new_graph.nodes[12] = {17};
  // graph[13][15] = graph[13][16] = true;
  new_graph.nodes[13] = {15, 16};
  // graph[15][17] = graph[15][18] = true;
  new_graph.nodes[15] = {17, 18};
  // graph[16][18] = graph[16][19] = graph[16][20] = graph[16][21] = true;
  new_graph.nodes[16] = {18, 19, 20, 21};
  return new_graph;
}


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
  std::string logs[] = {m->info_logs, m->warning_logs, m->error_logs};
  std::uniform_int_distribution<int> logs_nb(0, 10);
  std::uniform_int_distribution<int> nb_words(1, 30);
  std::uniform_int_distribution<int> ms(0, 10'000);
  for (int i = 0; i < 3; i++) {
    int nb_logs = logs_nb(gen);
    for (int j = 0; j < nb_logs; j++) {
      logs[i] += generateLoremIpsum(nb_words(gen));
      char time[64];
      sprintf(time, " {t=%d}\n", ms(gen));
      logs[i] += time;
    }
  }
  m->info_logs = logs[0];
  m->warning_logs = logs[1];
  m->error_logs = logs[2];
  return m;
};

float random_points[1001];

void fill_random_points_arr() {
  std::random_device rd;
  std::mt19937 gen(rd());
  // Define the range of random numbers
  std::uniform_real_distribution<float> distribution(1, 100);
  for (int i = 0; i < 1001; i++) {
    random_points[i] = distribution(gen);
  }
}

float vg(void *data, int idx) {
  float *fdata = (float *)data;
  return sinf(idx + (float)ImGui::GetTime());
}


std::vector<std::string> split_log(std::string log) {
  std::vector<std::string> tokens;
  std::istringstream iss(log);
  std::string token;

  while (std::getline(iss, token, '\n')) {
    tokens.push_back(token);
  }

  return tokens;
}

void inspector_properties(Mesh *m) {
  ImGuiTableFlags flags =
      ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg |
      ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable |
      ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
  if (ImGui::BeginTable("table1", 3, flags)) {
    ImGui::TableSetupColumn("x", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("y", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("z", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();
    int j = 0;
    for (const auto &point : m->points) {
      // if (ImGui::TreeNode(&point, "point")) {
      //   ImGui::Text("{ x: %d, y: %d, z: %d }", point.x, point.y,
      //   point.z); ImGui::TreePop();
      // }
      // if (ImGui::TreeNode(&point, "point")) {
      // static float vec4f[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
      // char label[64];
      // sprintf(label, "##point: %d", j);// static int vec3i[3] = { 10,
      // 20, 30 };

      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("%.3f", point.x);
      ImGui::TableNextColumn();
      ImGui::Text("%.3f", point.y);
      ImGui::TableNextColumn();
      ImGui::Text("%.3f", point.z);
      // ImGui::InputScalarN(label, ImGuiDataType_Float, (void *)&point,
      // 3, NULL, NULL, NULL, ImGuiInputTextFlags_ReadOnly);
      // ImGui::TableNextColumn();
      // ImGui::TableNextColumn();
      // ImGui::Text("{ x: %d, y: %d, z: %d }", point.x, point.y,
      // point.z); ImGui::TreePop();
      // }
      j++;
    }

    for (const auto &point : m->points) {
      char label[64];
      sprintf(label, "##point: %d",
              j); // static int vec3i[3] = { 10, 20, 30 };
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::InputFloat(label, (float *)&(point.x));
      ImGui::TableNextColumn();
      ImGui::InputFloat(label, (float *)&(point.y));
      ImGui::TableNextColumn();
      ImGui::InputFloat(label, (float *)&(point.z));
    }
    ImGui::EndTable();
  }

  static bool check = true;
  ImGui::Checkbox("checkbox", &check);

  static int e = 0;
  ImGui::RadioButton("Apple", &e, 0);
  ImGui::SameLine();
  ImGui::RadioButton("Banana", &e, 1);
  ImGui::SameLine();
  ImGui::RadioButton("Cherry", &e, 2);

  const char *items[] = {"Apple",     "Banana",     "Cherry",
                         "Kiwi",      "Mango",      "Orange",
                         "Pineapple", "Strawberry", "Watermelon"};
  static int item_current = 1;
  ImGui::Combo("##listbox", &item_current, items, IM_ARRAYSIZE(items), 4);
}

typedef enum { ERROR = 0, WARNING = 1, INFO = 2 } log_type_t;
typedef struct log {
  char message[512];
  int time;
  log_type_t type;
} log_t;

bool compare_log_t(const log_t &a, const log_t &b) {
  return a.time < b.time;
}

std::vector<log_t> get_formatted_sorted_logs(Mesh *m) {
  std::string logs[] = {m->error_logs, m->warning_logs, m->info_logs};
  std::vector<log_t> formatted_logs;
  for (int i = 0; i < 3; i++) {
    std::vector<std::string> messages = split_log(logs[i]);
    for (std::string string : messages) {
      log_t new_log;
      sscanf(string.c_str(), "%[^{] {t=%d}", new_log.message, &(new_log.time));
      new_log.type = (log_type_t)i;
      formatted_logs.emplace_back(new_log);
    }
  }
  std::sort(formatted_logs.begin(), formatted_logs.end(), compare_log_t);
  return formatted_logs;
}

void inspector_logs(Mesh *m) {
  static const char *labels[] = {"ERROR", "WARNING", "INFO"};
  static ImVec4 colors[] = {ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                     ImVec4(1.0f, 0.5f, 0.0f, 1.0f),
                     ImVec4(0.0f, 1.0f, 0.0f, 1.0f)};
  static bool checkboxes[] = {true, true, true};
  for (int i = 0; i < 3; i++) {
    ImGui::TextColored(colors[i], labels[i]); ImGui::SameLine();
    char label[64];
    sprintf(label, "## %s checkbox", labels[i]);
    ImGui::Checkbox(label, &checkboxes[i]); ImGui::SameLine();
  }
  ImGui::NewLine();
  ImGuiTableFlags flags =
      ImGuiTableFlags_SizingFixedFit
    | ImGuiTableFlags_RowBg |
      ImGuiTableFlags_Borders |
      ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable ;
  if (ImGui::BeginTable("logs table", 4, flags)) {
    ImGui::TableSetupColumn("##treenode");
    ImGui::TableSetupColumn("time");
    ImGui::TableSetupColumn("type");
    ImGui::TableSetupColumn("message");
    ImGui::TableHeadersRow();
  }
  std::vector<log_t> logs = get_formatted_sorted_logs(m);

  int i = 0;
  for (log_t log : logs) {
    if (!checkboxes[log.type])
      continue;
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    char label[64];
    sprintf(label, "##test %d", i++);
    bool is_open = ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_NoTreePushOnOpen);
    ImGui::TableNextColumn();
    ImGui::Text("%d", log.time);
    ImGui::TableNextColumn();
    ImGui::TextColored(colors[log.type], labels[log.type]);
    ImGui::TableNextColumn();
    ImGui::Text(log.message);
    if (is_open) {
      ImGui::EndTable();
      if (ImGui::BeginTable("message table", 1, ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("##message", ImGuiTableColumnFlags_WidthStretch);
        // ImGui::TableSetupColumn("##message");
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextWrapped(log.message);
        ImGui::EndTable();
      }
    sprintf(label, "##logs table %d", i-1);
      ImGui::BeginTable(label, 4, flags);
      continue;
    }
    // if (ImGui::TreeNodeEx(log.message, ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
    //   ImGui::TextWrapped(log.message);
    // }
    // ImGui::TableNextColumn();
  }
  ImGui::EndTable();
}

void my_window(Mesh **meshes, int len) {
  // fix window to left corner
  static bool object_inspector_active = false;
  float size_y = ImGui::GetIO().DisplaySize.y;
  if (object_inspector_active)
    size_y = size_y / 2;
  ImGui::SetNextWindowPos(ImVec2(0, 0)); // left corner
  ImGui::SetNextWindowSize(ImVec2(WINDOW_SIZE, size_y));
  // std::cout << ImGui::GetIO().DisplaySize.x << " " <<
  // ImGui::GetIO().DisplaySize.y << "\n"; bool open = true;
  int flags = 0;
  flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
  flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
  ImGui::Begin("MyWindow", nullptr, flags);

  // ImGui::Text("Hello, world %d", 123);
  if (ImGui::TreeNodeEx(ICON_FA_GEAR " test")) {
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
    bool node_open = ImGui::TreeNodeEx((void *)(intptr_t)i, node_flags,
                                       ICON_FA_FUTBOL " Mesh %d", i);
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
      node_clicked = i;
    if (is_selected) {
      // create new window
      object_inspector_active = true;
      ImGui::SetNextWindowPos(ImVec2(0, size_y));
      ImGui::SetNextWindowSize(ImVec2(WINDOW_SIZE, size_y));
      if (ImGui::Begin("Inspector", nullptr,
                       ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::BeginTabBar("TabBarTest", 0)) {
          if (ImGui::BeginTabItem("Properties")) {
            inspector_properties(m);
            ImGui::EndTabItem();
          }
          if (ImGui::BeginTabItem("Logs")) {
            inspector_logs(m);
            ImGui::EndTabItem();
          }
          ImGui::EndTabBar();
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
        if (ImGui::TreeNodeEx(&point, ImGuiTreeNodeFlags_AllowItemOverlap,
                              "point")) {
          ImGui::SameLine(0, 0);
          ImGui::Text(ICON_FA_BED);
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

  if (MyTreeNode("Hello")) {
    ImGui::Text("blah");
    if (MyTreeNode("blah")) {
      ImGui::Text("Hello");
      ImGui::TreePop();
    }
    ImGui::Text("blah");
    ImGui::TreePop();
  }

  // std::cout << open << "\n";
  ImGui::End();

  // bottom window

  size_y = ImGui::GetIO().DisplaySize.y;
  float size_x = ImGui::GetIO().DisplaySize.x;
  ImGui::SetNextWindowPos(
      ImVec2(WINDOW_SIZE, size_y - WINDOW_SIZE)); // left corner
  ImGui::SetNextWindowSize(ImVec2(size_x - WINDOW_SIZE, WINDOW_SIZE));
  if (ImGui::Begin("bottom", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

    static ImPlotColormap colormap= -1;
    static std::vector<ctime_t> values;
    static std::vector<std::string> labels_graph;
    int n = thread_pool.size();
    if (colormap == -1) {
      std::vector<ImVec4> colors = { ImVec4(1, 0, 0, 1)}; // inf line color
      for (int i = 0; i < n; i++) {
        const std::vector<TaskExecInfo> &tasks = thread_pool[i];
        ctime_t last = 0;
        for (const auto &task : tasks) {
          std::cout << "first: " << task.execTime.first << std::endl;
          std::cout << "second: " << task.execTime.second << std::endl;
          values.insert(values.end(), i, 0);
          values.push_back(task.execTime.first - last);
          values.insert(values.end(), n - i - 1, 0);

          values.insert(values.end(), i, 0);
          values.push_back(task.execTime.second - task.execTime.first);
          values.insert(values.end(), n - i - 1, 0);

          last = task.execTime.second;
          colors.push_back(ImVec4(0, 0, 0, 0));
          colors.push_back(task.color);
          labels_graph.push_back("###" + task.name + "_sleep_before");
          labels_graph.push_back(task.name);
          std::cout << "task: > " << task.name.c_str() << std::endl;
        }
      }
      colormap = ImPlot::AddColormap("scheduler", colors.data(), colors.size());
    }
    print_vec(values);
    std::cout << "values size: "<< values.size() << " labels size: " << labels_graph.size() << std::endl;
    ImPlot::PushColormap(colormap);
    if (ImPlot::BeginPlot("Scheduler", ImVec2(1920 - WINDOW_SIZE - 100, 300), ImPlotFlags_CanvasOnly)) {
      std::vector<std::string> thread_labels_strings;
      for (int i = 0; i < thread_pool.size(); i++) {
        std::string label = "Thread " + std::to_string(i);
        thread_labels_strings.push_back(label);
      }
      std::vector<const char *> thread_labels;
      for (auto &ref :thread_labels_strings) {
        thread_labels.push_back(ref.c_str());
        std::cout << ref.c_str() << std::endl;;
      }
      std::cout << " n: " << n << " size: " << thread_labels.size() << std::endl;

      ImPlot::SetupAxes("ms",nullptr,ImPlotAxisFlags_LockMin, 0);
      ImPlot::SetupAxisTicks(ImAxis_Y1,0,n-1,n,thread_labels.data(),false);

      int  val[] = {200000};
      // static double vals[] = {0.25, 0.5, 0.75};
      ImPlot::PlotInfLines("20ms", val, 1);
      static std::vector<const char *> labels;
      for (auto &ref : labels_graph) {
        labels.push_back(ref.c_str());
      }
      ImPlot::PlotBarGroups(
          labels.data(), values.data(), values.size()/n, n, 0.67, 0,
          ImPlotBarGroupsFlags_Stacked | ImPlotBarGroupsFlags_Horizontal);
      auto mouse_pos = ImPlot::GetPlotMousePos();

      for (int i = 0; i < thread_pool.size(); i++) {
        for (const auto &task : thread_pool[i]) {
          ctime_t bar_start = task.execTime.first;
          ctime_t bar_end = task.execTime.second;
          if (ImPlot::IsPlotHovered() && i+-0.335 < mouse_pos.y &&
              mouse_pos.y < i+0.335 && bar_start < mouse_pos.x &&
              mouse_pos.x < bar_end && ImGui::BeginTooltip()) {
            ImGui::Text("Thread %d", task.execThreadIndex);
            ImGui::Text("%s", task.name.c_str());
            ImGui::Text("start time: %llull ms", task.execTime.first);
            ImGui::Text("end time: %llull ms", task.execTime.second);
            ImGui::EndTooltip();
          }
        }
      }

      // Data arrays for the bars
      // static double data_prev[] = {1, 2, 3, 4};    // Previous bars
      // static double data_stacked[] = {5, 6, 7, 8}; // Stacked bars

      // // Plotting the previous bars
      // ImPlot::PlotBars("Previous", data_prev, 4, 0.2f);

      // // Creating a space (empty bars)
      // static double empty_data[] = {0, 0, 0, 0}; // Empty bars data
      // ImPlot::PlotBars("", empty_data, 4, 0.2f);

      // // Plotting the stacked bars
      // ImPlot::PlotBars("Stacked", data_stacked, 4, 0.2f);

      // Plot the second set of bars on top of the first set
      // ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0, 0, 0, 0));
      // ImPlot::PlotBars("labels", values2, 3, 0.4, 1, ImPlotBarsFlags_Horizontal);
      // ImPlot::PopStyleColor();

      ImPlot::EndPlot();
    }
    ImPlot::PopColormap();
      static float xs1[1001], ys1[1001];
      static float g[1001];
      for (int i = 0; i < 1001; ++i) {
        xs1[i] = i * 0.01f;
        ys1[i] = sinf(xs1[i] + (float)ImGui::GetTime());
        g[i] = exp(-pow(fmod(xs1[i] + (float)ImGui::GetTime(), 10) - 4, 2));
        // g[i] = exp(xs1[i]);
        // std::cout << exp(i) << " " << i << std::endl;
      }
      memmove(random_points, random_points + 1, sizeof(int) * 1000);

      std::random_device rd;
      std::mt19937 gen(rd());
      // Define the range of random numbers
      std::uniform_real_distribution<float> distribution(1, 100);
      random_points[1000] = distribution(gen);

      ImGui::PlotLines("##teeest", ys1, IM_ARRAYSIZE(ys1), 0, "forces");
      ImGui::SameLine();
      ImGui::PlotLines("##values_getter_test", vg, xs1, IM_ARRAYSIZE(xs1), 0,
                       "teeest");

      if (ImPlot::BeginPlot("Force x", ImVec2(500, 200))) {
        ImPlot::SetupAxes("time", "force");
        ImPlot::PlotLine("##f(x)", xs1, ys1, 1001);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
        ImPlot::EndPlot();
      }
      ImGui::SameLine();

      if (ImPlot::BeginPlot("Force y", ImVec2(500, 200))) {

        ImPlot::SetupAxes("x", "y");

        // ImPlot::PlotLineG("##f(x)", ImPlotGetter getter, void* data, int
        // count, ImPlotLineFlags flags=0);
        ImPlot::PlotLine("##f(x)", random_points, 1001);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
        ImPlot::EndPlot();
      }
      ImGui::SameLine();
      if (ImPlot::BeginPlot("Force z", ImVec2(500, 200))) {
        ImPlot::SetupAxes("x", "y");
        ImPlot::PlotLine("##f(x)", xs1, g, 1001);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
        ImPlot::EndPlot();
      }

      ImGui::End();
    }
}


#define MARGIN_X 50
#define MARGIN_Y 20
#define INVISIBLE_NODE_SIZE 200
std::unordered_map<int, ImVec2>  compute_coords(std::unordered_map<int, std::vector<int>> map, graph_t& graph) {
  std::vector<int> layers_sizes(map.size() + 1); // llayers start at 1
  int max_size_layer = 0;
  int max_layer = 0;
  for (const auto& pair : map) {
    int res = 0;
    for (int node: pair.second) {
      // if (graph.isInvisible(node)) {
      //   res += INVISIBLE_NODE_SIZE + MARGIN_Y;
      //   continue;
      // }
      ImVec2 size = ed::GetNodeSize(node*3+1);
      res += size.y + MARGIN_Y;
    }
    layers_sizes.at(pair.first) = res;
    if (res > max_size_layer) {
      max_size_layer = res;
      max_layer = pair.first;
    }
  }


  std::unordered_map<int, ImVec2> coords;
  int x = 0;
  for (int layer = 1; layer < map.size()+1; layer++) {
    int size_y = layers_sizes.at(layer);
    int y = (max_size_layer - size_y) / 2; // center the y coord of the current layer
    int max_x_node = 0;
    for (const int& node: map.at(layer)) {
      // if (graph.isInvisible(node)) {
      //   ImVec2 size = ImVec2(200, 100);
      // } else {
        ImVec2 size = ed::GetNodeSize(node*3+1);
      // }
      coords[node] = ImVec2(x, y);
      y += size.y + MARGIN_Y;
      if (size.x > max_x_node)
        max_x_node = size.x;
    }
    x += max_x_node + MARGIN_X;
  }

  // std::unordered_map<int, int> layer_x_coord;
  // int prev_size = max_size_layer;
  // layer_x_coord[max_layer] = 0;
  // for (const auto& pair : layers_sizes) {
  //   int layer = pair.first;
  //   int size = pair.second;
  //   layer_x_coord[layer] = (max_size_layer - layers_sizes[layer]) / 2;
  // }
  // for (int i = max_layer - 1; i >= 1; i--) {
  //   std::cout << "prev_size: " << prev_size << std::endl;
  //   std::cout << "layers_sizes[i]: " <<  layers_sizes[i] << " i: "<< i << std::endl;
  //   std::cout << "res: " << (prev_size - layers_sizes[i]) / 2 << std::endl;
  //   layer_x_coord[i] = (prev_size - layers_sizes[i]) / 2 + layer_x_coord[i+1];
  //   prev_size = layers_sizes[i];
  // }
  // prev_size = max_size_layer;
  // for (int i = max_layer + 1; i < map.size() + 1; i++) {
  //   layer_x_coord[i] = (prev_size - layers_sizes[i]) / 2;
  //   prev_size = layers_sizes[i];
  // }
  return coords;
}

ed::EditorContext *ed_context;


void node_editor(graph_t& graph) {
  float size_y = ImGui::GetIO().DisplaySize.y;
  float size_x = ImGui::GetIO().DisplaySize.x;
  float x = (size_x - WINDOW_SIZE) / 2;
  float y = size_y - WINDOW_SIZE;
  ImGui::SetNextWindowPos(ImVec2(WINDOW_SIZE + x, 0));
  ImGui::SetNextWindowSize(ImVec2(x, y));
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  if (ImGui::Begin("graph window", nullptr, flags)) {
    auto &io = ImGui::GetIO();

    ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate,
                io.Framerate ? 1000.0f / io.Framerate : 0.0f);

    ImGui::Separator();

    ed::SetCurrentEditor(ed_context);

    ed::Begin("My Editor", ImVec2(0.0, 0.0f));
    int uniqueId = 1;
    static ImVec2 * positions;
    static bool first_loop = true;
    std::unordered_map<int, std::vector<int>> layers;
    if (first_loop) {
      std::cout << "here: ";
      print_vec(graph.nodes.at(0));
      layers = compute_layout(graph);
      std::cout << "end here: ";
      print_vec(graph.nodes.at(0));
    }

    for (int i = 0; i < graph.size(); i++) {
      int node = i;
      ed::BeginNode(uniqueId++);
      if (graph.isInvisible(node)) {
        // ed::SetInvisible(uniqueId-1);
        ImGui::Text("im invisible");
        // ImGui::Text("-> In");
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
        // ImGui::Text("Out ->");
        ed::EndPin();
        ed::EndNode();
        continue;
      }
      ImGui::Text("%s", graph.labels.at(node));
      ed::BeginPin(uniqueId++, ed::PinKind::Input);
      ImGui::Text("-> In");
      ed::EndPin();
      ImGui::SameLine();
      ed::BeginPin(uniqueId++, ed::PinKind::Output);
      ImGui::Text("Out ->");
      ed::EndPin();
      ed::EndNode();
    }

    std::for_each(graph.nodes.begin(), graph.nodes.end(),
      [&uniqueId, &graph](const auto &pair) {
        int start;
        std::vector<int> neighbours;
        boost::tie(start, neighbours) = pair;
        int startId = 3 * start + 1 + 2;// +1 bc uniqueId starts at 1 because inid is neighid + 2
        // if (graph.isInvisible(start)) {
        //   startId = 3 * start + 1 + 1;// +1 bc uniqueId starts at 1 because inid is neighid + 2
        // }
        std::for_each(neighbours.begin(), neighbours.end(),
          [&startId, &uniqueId](const int neigh) {
            int endId = 3 * neigh + 1 + 1; // +1 1 bc uniqueId starts at 1 because inid is neighid + 1
            if (endId == 2 || startId == 2) exit(0);
            ed::Link(uniqueId++, startId, endId);
        });
      });
    // ed::BeginNode(uniqueId++);
    // ed::SetInvisible(uniqueId-1);
    // ed::BeginPin(uniqueId++, ed::PinKind::Input);
    // ed::EndPin();
    // ed::EndNode();
    // ed::Link(uniqueId, 2, uniqueId-1);
    // uniqueId++;

    if (first_loop) {
      std::unordered_map<int, ImVec2> coords = compute_coords(layers, graph);
      for (const auto &[node, coord]: coords) {
        ed::SetNodePosition(node*3+1, coord);
      }
      first_loop=false;
    }

    ed::End();
    ed::SetCurrentEditor(nullptr);

    // ImGui::ShowMetricsWindow();
    ImGui::End();
  }
}



// Main code
int main(int, char **) {
  // create some random meshes
  Mesh *meshes[10];
  for (int i = 0; i < 10; i++) {
    meshes[i] = create_random_mesh("Mesh", 5);
  }
  fill_random_points_arr();
  init_thread_pool();
  // int n= 5;
  // std::cout << "Mesh" + std::string(n) ;

  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;
  // Initialize GLFW and create a window
  glfwInit();
  GLFWwindow *window =
      glfwCreateWindow(1920, 1080, "ImGui Window", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  // Initialize ImGui for GLFW and OpenGL
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ed_context = ed::CreateEditor(nullptr);
  ImGui::StyleColorsDark();
  // Merge icons into default tool font
  ImGuiIO &io = ImGui::GetIO();
  io.Fonts->AddFontDefault();

  ImFontConfig config;
  config.MergeMode = true;
  config.OversampleH = 5;
  config.OversampleV = 5;
  // config.GlyphExtraSpacing.x = 1.0f;
  // config.GlyphMinAdvanceX =
  //     13.0f; // Use if you want to make the icon monospaced
  static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
  // io.Fonts->AddFontFromFileTTF("fa-regular-400.ttf", 13.0f, &config,
  // icon_ranges);
  io.Fonts->AddFontFromFileTTF("../fa-solid-900.ttf", 13.0f, &config,
                               icon_ranges);
  // Example of loading an icon font
  // ImGuiIO& io = ImGui::GetIO();
  // io.Fonts->AddFontFromFileTTF("fa-solid-900.ttf", 13);
  // io.Fonts->AddFontFromFileTTF("./imgui/misc/fonts/Roboto-Medium.ttf", 13);
  // io.Fonts->Build();

  // ImGuiIO &io = ImGui::GetIO();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  // bool **graph = create_random_graph(10);
  graph_t graph = example_wiki_graph();
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // Start a new ImGui frame
    new_frames();

    // Create your ImGui UI here
    my_window(meshes, 10);
    node_editor(graph);
    ImGui::ShowDemoWindow();
    ImPlot::ShowDemoWindow();

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
  ImPlot::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();
  // ImGui_ImplOpenGL3_Init("#version 330");
}
