#ifndef IS_MESH_H
#define IS_MESH_H

#include "topology_kernel.h"

namespace is_mesh
{
  class mesh:public topology_kernel
  {
  public:

    template <typename T>
    int add_property(const simplex_dim& dim, const T& prop, std::string prop_name = "<unknown>")
    {
      return pm_.add_property(dim, prop, prop_name);
    }

    template<typename T>
    const T& get_simplex_property(const simplex_handle& sh, size_t prop_id) const
    {
      return pm_.get_element_property<T>(sh, prop_id);
    }

    template<typename T>
    T& get_simplex_property(const simplex_handle& sh, size_t prop_id)
    {
      return pm_.get_element_property<T>(sh, prop_id);
    }

    template<typename T>
    int get_prop_id(const simplex_dim& dim, const T& prop, std::string prop_name = "<unknown>") const
    {
      return pm_.get_specific_prop_index(dim, prop, prop_name);
    }

    const simplex& get_specific_simplex(const simplex_handle& sh) const
    {
      return sm_.get_specific_simplex(sh);
    }

    simplex& get_specific_simplex(const simplex_handle& sh)
    {
      return sm_.get_specific_simplex(sh);
    }

    simplex_handle get_handle(const std::vector<size_t>& verts) const
    {
      const int dim  = verts.size() - 1;
      assert(dim >= 0 && dim <= top_dim_);
      map_type::const_iterator c_it = simplex2handle_[dim].find(verts);
      simplex_handle sh;
      if(c_it != simplex2handle_[dim].end())
        sh = c_it->second;
      return sh;
    }

    double get_length(const simplex_handle& sh);

    double get_area(const simplex_handle& sh);

    double get_volume(const simplex_handle& sh);

    int get_face_normal(const simplex_handle& sh, matrixd& face_normal);

  private:
  };
}
#endif // MESH_H
