#ifndef IS_UTILS_H
#define IS_UTILS_H

#include <jtflib/mesh/mesh.h>
#include "../common/common.h"

namespace is_mesh
{

  int dfs_outside_face(const size_t index, std::vector<bool> &is_visited,
                       const matrixst &face,
                       const jtf::mesh::edge2cell_adjacent &edge_adj);

  bool detect_topology(const matrixst& cells);
}

#endif // UTILS_H
