#ifndef GRAPH_LAYOUT_H_
#define GRAPH_LAYOUT_H_
#include <unordered_map>
#include <vector>
#include <string>

typedef struct graph_t {
  std::unordered_map<int, std::vector<int>> nodes;
  std::vector<const char *> labels;
  int initial_size;

    graph_t(const char * labels[], int n) : labels(labels, labels + n), initial_size(n) {
        for (int i = 0; i < n; i++) {
            nodes[i] = std::vector<int>();
        }

    }
  size_t size() {return nodes.size();}
  bool isInvisible(int nodeNum) {return initial_size <= nodeNum; }

  const char * label(int node) { return labels[node];}
  std::unordered_map<int, std::vector<int>>& get() {
    return nodes;
  }

} graph_t;

std::unordered_map<int, std::vector<int>> compute_layout(graph_t& graph);
void print_vec(std::vector<int>& vec);



#endif // GRAPH_LAYOUT_H_
