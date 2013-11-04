#include "io.h"

namespace is_mesh
{
  namespace io
  {
    using namespace zjucad::matrix;

    int read_mesh(const matrixd& node, const matrixst& top_simplex, mesh_type &mesh)
    {
      const simplex_dim top_dim = top_simplex.size(1) - 1;
      mesh.set_dim(top_dim);
      simplex_handle sh;
      for(size_t i = 0; i < node.size(2); ++i)
        mesh.new_vert(i, node(colon(), i), sh);

      std::vector<size_t> verts(top_dim + 1);
      for(size_t i = 0; i < top_simplex.size(2); ++i)
        {
          for(size_t j = 0; j < verts.size(); ++j)
            verts[j] = top_simplex[top_simplex.size(1) * i + j];
          mesh.new_top_simplex(verts, sh);
        }
      return 0;
    }

    int write_mesh(matrixd& node, matrixst& top_simplex, const mesh_type &mesh, bool is_remove)
    {
      const simplex_manager& sm = mesh.get_simplex_manager();
      simplex_handle cur_sh;
      cur_sh.set_dim(0);
      node.resize(3, sm.n_element(0));
      for(size_t i = 0; i < node.size(2); ++i)
        {
          cur_sh.set_id(i);
          node(zjucad::matrix::colon(), i) = mesh.get_coord(cur_sh);
        }

      size_t top_simplex_num = 0;
      const size_t top_dim = mesh.top_dim();
      cur_sh.set_dim(top_dim);
      for(size_t i = 0; i < sm.n_element(top_dim); ++i)
        {
          cur_sh.set_id(i);
          if(!mesh.is_simplex_deleted(cur_sh))
            ++top_simplex_num;
        }
      top_simplex.resize(top_dim + 1, top_simplex_num);
      size_t cnt = 0;
      for(size_t i = 0; i < sm.n_element(top_dim); ++i)
        {
          cur_sh.set_id(i);
          if(!mesh.is_simplex_deleted(cur_sh))
            {
              std::vector<simplex_handle> bounds;
              mesh.get_k_boundary_simplex(cur_sh, 0, bounds);
              for(size_t j = 0; j < bounds.size(); ++j)
                {
                  top_simplex(j, cnt) = bounds[j].id();
                }
              ++cnt;
            }
        }
      if(is_remove)
        remove_extra_nodes(top_simplex, node);
      return 0;
    }

    int remove_extra_nodes(matrixst &cells, matrixd &node)
    {
      assert(cells.size(1) == 4 || cells.size() == 3);
      assert(*std::max_element(cells.begin(), cells.end()) < node.size(2));

      std::set<size_t> used_node_idx(cells.begin(), cells.end());
      matrixst used_node_mat(used_node_idx.size(), 1);
      std::copy(used_node_idx.begin(), used_node_idx.end(), used_node_mat.begin());

      std::map<size_t,size_t> p2p;

      matrixd new_node(3, used_node_mat.size());
      for(size_t pi = 0; pi < used_node_mat.size(); ++pi){
        new_node(colon(),pi) = node(colon(), used_node_mat[pi]);
        p2p[used_node_mat[pi]] = pi;
      }
      for(size_t pi = 0; pi < cells.size(); ++pi)
        cells[pi] = p2p[cells[pi]];
      node = new_node;
      return 0;
    }

    int tet_mesh_read_from_zjumat(const char *path, matrixd *node,
                                  matrixst *tet, matrixst *tri)
    {
      std::ifstream ifs(path, std::ifstream::binary);
      if(ifs.fail()) {
        std::cerr << "[info] " << "can not open file" << path << std::endl;
        return __LINE__;
      }

      matrixd node0;
      zjucad::matrix::matrix<int> tet1, tri1;
      if(!node) node = &node0;

      read_matrix(ifs, *node);
      read_matrix(ifs, tet1);
      read_matrix(ifs, tri1);
      if(tet)
        *tet = tet1;
      if(tri)
        *tri = tri1;

      if(max(*tet) >= node->size(2)){
        std::cerr << "# [error] tet index beyond node size " << std::endl;
        return __LINE__;
      }
      return 0;
    }

    int tet_mesh_write_to_zjumat(const char *path, matrixd *node,
                                 matrixst *tet,  matrixst *tri)
    {
      std::ofstream ofs(path, std::ofstream::binary);
      zjucad::matrix::matrix<int> tet1, tri1;
      if(node)
        write_matrix(ofs, *node);
      if(tet){
        tet1 = *tet;
        write_matrix(ofs, tet1);
      }
      if(tri){
        tri1 = *tri;
        write_matrix(ofs, tri1);
      }
      return 0;
    }
  }
}

