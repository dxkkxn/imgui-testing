#include "graph_layout.h"
#include <algorithm>
#include <assert.h>
#include <boost/range/combine.hpp>
#include <deque>
#include <iostream>
#include <unordered_map>
#include <vector>

int find_root_node(graph_t graph) {
  int root_node;
  bool found_root;
  for (const auto &[possible_root, _] : graph.nodes) {
    found_root = true;
    for (auto it = graph.nodes.begin(); it != graph.nodes.end() && found_root;
         ++it) {
      int node;
      std::vector<int> succesors;
      boost::tie(node, succesors) = *it;
      if (node == possible_root)
        continue;
      for (auto succIt = succesors.begin();
           succIt != succesors.end() && found_root; ++succIt) {
        int succesor = *succIt;
        if (succesor == possible_root)
          found_root = false;
      }
    }
    std::cout << "node : " << possible_root << " value: " << found_root
              << std::endl;
    if (found_root) {
      root_node = possible_root;
      break;
    }
  }
  assert(found_root && "No root node found");
  std::cout << "root node: " << graph.labels.at(root_node) << std::endl;
  return root_node;
}

std::vector<int> layer_assignment(graph_t graph) {
  int n = graph.nodes.size();
  std::vector<int> assignment(n);
  std::vector<int> next_layer;
  bool *visited_nodes =
      new bool[n]; // just to throw an error if a cycle is detected
  for (int i = 0; i < n; i++) {
    visited_nodes[i] = false;
  }
  int root = find_root_node(graph);
  std::vector<int> current_layer;
  current_layer.push_back(root);
  int layer = 1;
  while (!current_layer.empty()) {
    // for each node of the current_layer assign the layer number
    for (int node : current_layer) {
      assignment[node] = layer;
      visited_nodes[node] = true;
      // for each one of the succesors of node push it in next_layer
      std::vector<int> succesors = graph.nodes.at(node);
      next_layer.insert(next_layer.end(), succesors.begin(), succesors.end());
    }
    current_layer = next_layer;
    next_layer.clear();
    layer++;
  }
  delete[] visited_nodes;
  return assignment;
}

void print_vec(std::vector<int> &vec) {
  for (const auto &element : vec) {
    std::cout << element << " ";
  }
  std::cout << std::endl;
}

void insert_invisible_nodes(graph_t &graph,
                            std::unordered_map<int, std::vector<int>> &layer,
                            std::vector<int> assignement) {
  assert(layer.at(1).size() == 1);
  int root_node = layer.at(1).at(0);
  std::deque<int> deque = {root_node};
  while (!deque.empty()) {
    int current = deque.front();
    deque.pop_front();
    assert(assignement.size() > current);
    int current_layer = assignement.at(current);
    std::vector<int> new_succesors = graph.nodes.at(current);
    for (const int &succ : graph.nodes.at(current)) {
      assert(assignement.size() > succ);
      assert(assignement.at(succ) >= current_layer + 1);
      if (assignement.at(succ) > current_layer + 1) {
        // std::cout << "current node: " << graph.label(current) << std::endl;
        // std::cout << "succ node: " << graph.label(succ) << std::endl;
        int newNode = graph.size();
        assert(graph.nodes.find(newNode) == graph.nodes.end());
        new_succesors.push_back(newNode);
        std::vector<int> vec = {succ};
        graph.nodes[newNode] = vec;
        layer.at(current_layer + 1).push_back(newNode);
        assert(assignement.size() == newNode);
        assignement.push_back(current_layer + 1);

        new_succesors.erase(
            std::remove(new_succesors.begin(), new_succesors.end(), succ),
            new_succesors.end());
        std::cout << "added node" << std::endl;
      }
    }
    print_vec(graph.nodes.at(current));
    graph.nodes.at(current) = new_succesors;
    print_vec(graph.nodes.at(current));
    deque.insert(deque.end(), new_succesors.begin(), new_succesors.end());
  }
  std::cout << "end func" << std::endl;
}


// std::vector<std::vector<int>> permuations(std::vector<int> vec) {

// }

int compute_crossing_edges(int layer,
                           std::unordered_map<int, std::vector<int>> layers, graph_t graph);

std::unordered_map<int, std::vector<int>> compute_layout(graph_t &graph) {
  // find_root_node(graph);
  std::vector<int> assignment = layer_assignment(graph);
  std::unordered_map<int, std::vector<int>> map;
  for (int i = 0; i < graph.nodes.size(); i++) {
    map[assignment[i]].push_back(i);
  }
  int root = find_root_node(graph);
  insert_invisible_nodes(graph, map, assignment);
  for (int layer = 1; layer < map.size(); layer++) {
    std::vector<int>& layer_nodes = map.at(layer);
    std::sort(layer_nodes.begin(), layer_nodes.end());
    auto minVec = layer_nodes;
    int minValue = compute_crossing_edges(layer, map, graph);
    while (std::next_permutation(layer_nodes.begin(), layer_nodes.end())) {
      int currValue = compute_crossing_edges(layer, map, graph);
      if (currValue < minValue) {
        minValue = currValue;
        minVec = layer_nodes;
      }
    }
    map[layer] = minVec;
    // std::cout << "layer: " << layer
    //           << ", crossing edges: " << compute_crossing_edges(layer, map, graph)
    //           << std::endl;

  }
  return map;
}



// computes the number of crossing between layer and layer + 1;
int compute_crossing_edges(int layer,
                           std::unordered_map<int, std::vector<int>> layers, graph_t graph) {
  auto exists_edge = [&graph](int src, int dest) {
    auto it = find(graph.nodes.at(src).begin(), graph.nodes.at(src).end(), dest);
    if (it == graph.nodes.at(src).end())
      return false;
    return true; // Edge exist
  };
  assert(layer >= 1 and layers.find(layer) != layers.end());
  auto ncl = layers.at(layer); // nodes current layer (ncl)
  auto nnl = layers.at(layer + 1); // nodes next layer (nnl)
  int count = 0;
  for (int i = 0; i < ncl.size(); i++) {
    for (int j = 0; j < nnl.size(); j++) {
      // ncl[i] ->  nnl[j]
      if (!exists_edge(ncl[i], nnl[j])) continue;
      for (int k = i + 1; k < ncl.size(); k++) {
        for (int l = 0; l < j; l++) {
          if (exists_edge(ncl[k], nnl[l]))
            count++;
        }
      }
    }
  }
  return count;
}
