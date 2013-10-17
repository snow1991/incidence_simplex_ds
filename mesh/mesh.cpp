#include "mesh.h"

namespace is_mesh
{
  double mesh::get_length(const simplex_handle& sh)
  {
    using namespace zjucad::matrix;
    assert(is_valid_handle(sh));
    assert(sh.dim() == 1);
    std::vector<simplex_handle> verts;
    get_k_boundary_simplex(sh, 0, verts);
    assert(verts.size() == 2);
    return norm(get_coord(verts[0]) - get_coord(verts[1]));
  }

  double mesh::get_area(const simplex_handle& sh)
  {
    using namespace zjucad::matrix;
    assert(is_valid_handle(sh));
    assert(sh.dim() == 2);
    std::vector<simplex_handle> verts;
    get_k_boundary_simplex(sh, 0, verts);
    assert(verts.size() == 3);
    return norm(cross(get_coord(verts[1]) - get_coord(verts[0]),
                      get_coord(verts[2]) - get_coord(verts[0]))) / 2.0;
  }

  double mesh::get_volume(const simplex_handle& sh)
  {
    using namespace zjucad::matrix;
    assert(is_valid_handle(sh));
    assert(sh.dim() == 3);
    std::vector<simplex_handle> verts;
    get_k_boundary_simplex(sh, 0, verts);
    assert(verts.size() == 4);
    return dot(cross(get_coord(verts[1]) - get_coord(verts[0]),
                     get_coord(verts[2]) - get_coord(verts[0])),
               get_coord(verts[3]) - get_coord(verts[0])) / 6.0;
  }

  int mesh::get_face_normal(const simplex_handle& sh, matrixd& face_normal)
  {
    using namespace zjucad::matrix;
    assert(is_valid_handle(sh));
    assert(sh.dim() == 2);
    std::vector<simplex_handle> verts;
    get_k_boundary_simplex(sh, 0, verts);
    assert(verts.size() == 3);
    face_normal = cross(get_coord(verts[1]) - get_coord(verts[0]),
                        get_coord(verts[2]) - get_coord(verts[0]));
    double len = norm(face_normal);
    assert(len > ZERO);
    face_normal /= len;
    return 0;
  }
}
