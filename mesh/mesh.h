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

    double get_length(const simplex_handle& sh);

    double get_area(const simplex_handle& sh);

    double get_volume(const simplex_handle& sh);

    int get_face_normal(const simplex_handle& sh, matrixd& face_normal);

  private:
  };
}
#endif // MESH_H
