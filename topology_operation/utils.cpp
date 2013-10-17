#include "utils.h"

namespace is_mesh
{
  int dfs_outside_face(const size_t index, std::vector<bool> &is_visited,
                       const matrixst &face,
                       const jtf::mesh::edge2cell_adjacent &edge_adj)
  {
    size_t face_index, edge_index;
    std::vector<size_t> edge2tri;
    if(!is_visited[index])
    {
      is_visited[index] = true;
      for(size_t i = 0; i < 3; ++i)
        for(size_t j = i+1; j < 3; ++j)
        {
          edge_index = edge_adj.get_edge_idx(face(i, index), face(j, index));
          edge2tri.clear();
          edge2tri.push_back(edge_adj.edge2cell_[edge_index].first);
          edge2tri.push_back(edge_adj.edge2cell_[edge_index].second);
          if(edge2tri.size() != 2)
            continue;
          face_index = edge2tri[0] + edge2tri[1] - index;
          dfs_outside_face(face_index, is_visited, face, edge_adj);
        }
    }
    return 0;
  }

  bool detect_topology(const matrixst& cells)
  {
    using namespace jtf::mesh;
    matrixst face;
    if(cells.size(2) == 3)
      face = cells;
    else
      {
      }
    std::auto_ptr<edge2cell_adjacent> edge_adj(edge2cell_adjacent::create(face, false));
    if(!edge_adj.get())
      return false;
    std::vector<bool> is_visited(face.size(2), false);
    dfs_outside_face(0, is_visited, face, *edge_adj);
    for(size_t i = 0; i < is_visited.size(); ++i)
      if(!is_visited[i])
      {
        std::cout << "exist a hole" << std::endl;
        return false;
      }
    return true;
  }
}
