#include <sxxlib/is_mesh/topology_operation/topology_operation.h>
#include <sxxlib/is_mesh/io/io.h>
#include <jtflib/mesh/io.h>

#define IS_TET_MESH 0

int main(int argc, char **argv)
{
  if(argc < 3)
    {
      std::cerr << "the arguments are less" << std::endl;
      return 1;
    }

  zjucad::matrix::matrix<double> nodes;
  zjucad::matrix::matrix<size_t> cells;

  /********************   read mesh   **********************/
#if IS_TET_MESH
  is_mesh::io::tet_mesh_read_from_zjumat(argv[1], &nodes, &cells);
#else
  jtf::mesh::load_obj(argv[1], cells, nodes);
#endif

  is_mesh::mesh cur_mesh;
  is_mesh::io::read_mesh(nodes, cells, cur_mesh);
  is_mesh::simplex_handle sh;
  const size_t top_dim = cur_mesh.top_dim();

  /*********************  query example   ********************/
  for(size_t dim = 0; dim <= top_dim; ++dim)
    {
      sh.set_dim(dim);
      size_t n_ele = cur_mesh.n_elements(dim);
      std::cout << "***   the num of dim " << dim
                << " is: " << n_ele << "   ***" << std::endl;
      if(dim == 0)
        continue;
      for(size_t j = 0; j < n_ele; ++j)
        {
          std::vector<is_mesh::simplex_handle> bounds;
          sh.set_id(j);
          if(cur_mesh.is_simplex_deleted(sh))
            continue;
          cur_mesh.get_k_boundary_simplex(sh, 0, bounds);
          std::cout << "dim: " << dim << "  id: " << j << ":   ";
          for(size_t k = 0; k < bounds.size(); ++k)
            std::cout << bounds[k].id() << " ";
          std::cout << std::endl;
        }
    }

  /*******************   topology operation example   **************/
  is_mesh::topology_operation top_operation(cur_mesh);
  const is_mesh::simplex_dim edge_dim = 1;
  const size_t num = cur_mesh.n_elements(edge_dim);
  sh.set_dim(edge_dim);
  for(size_t i = 0; i < num; ++i)
    {
      sh.set_id(i);
      if(cur_mesh.is_simplex_deleted(sh))
        continue;
      is_mesh::coord_type coord(zjucad::matrix::zeros(3, 1));
      std::vector<is_mesh::simplex_handle> bounds;
      cur_mesh.get_k_boundary_simplex(sh, 0, bounds);
      for(size_t j = 0; j < bounds.size(); ++j)
        coord += cur_mesh.get_coord(bounds[j]);
      coord /= bounds.size();
      top_operation.insert_vertex(sh, coord);
    }


  /********************  output mesh  ************************/
  zjucad::matrix::matrix<double> out_nodes;
  zjucad::matrix::matrix<size_t> out_cells;
  is_mesh::io::write_mesh(out_nodes, out_cells, cur_mesh);

#if IS_TET_MESH
  io::tet_mesh_write_to_zjumat(argv[2], &out_nodes, &out_cells);
#else
  jtf::mesh::save_obj(argv[2], out_cells, out_nodes);
#endif

  return 0;
}
