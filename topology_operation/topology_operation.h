#ifndef IS_TOPOLOGY_OPERATION_H
#define IS_TOPOLOGY_OPERATION_H

#include "../mesh/mesh.h"

namespace is_mesh
{
  class topology_operation
  {
  public:
    topology_operation(mesh& rhs): cur_mesh_(rhs) {}

    int insert_vertex(const simplex_handle& sh, const matrixd& coord);

    int collapse_edge(const simplex_handle& sh, const matrixd& coord);

    int flip_edge(const simplex_handle& sh, std::vector<simplex_handle>& new_edge);

  protected:

    int del_top_simplex(const simplex_handle& sh,
                        std::vector<simplex_handle>& vert_shs,
                        std::vector<std::vector<simplex_handle> >& other_verts);

    int insert_top_simplex(const simplex_handle& v_sh,
                           const std::vector<simplex_handle>& vert_shs,
                           const std::vector<std::vector<simplex_handle> >& other_verts);

    int insert_snd_top_simplex(const simplex_handle& v_sh,
                               const std::vector<simplex_handle>& vert_shs,
                               const std::vector<std::vector<simplex_handle> >& other_verts);

    int insert_edge_in_tet(const simplex_handle& v_sh,
                           const std::vector<simplex_handle>& vert_shs,
                           const std::vector<std::vector<simplex_handle> >& other_verts);

    bool is_edge_collapse_ok(const simplex_handle& sh);

    bool is_edge_flip_ok(const simplex_handle& sh);

    template<typename T>
    bool is_in(const std::vector<T>& vec, const T& ele)
    {
      bool flg = false;
      for(size_t i = 0; i < vec.size(); ++i)
        if(vec[i] == ele)
          {
            flg = true;
            break;
          }
      return flg;
    }

  protected:
    mesh& cur_mesh_;
  };
}

#endif
