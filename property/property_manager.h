#ifndef IS_PROPERTY_MANAGER_H
#define IS_PROPERTY_MANAGER_H

#include "../simplex/simplex_handle.h"
#include "simplex_property.h"

namespace is_mesh
{
  class property_manager
  {
  public:
    typedef std::vector<simplex_property> mesh_property_type;

    property_manager(size_t top_dim = 0) {mesh_property_.resize(top_dim + 1);}

    void set_dim(size_t top_dim) {mesh_property_.resize(top_dim + 1);}

    size_t get_max_dim() const
    {return mesh_property_.size();}

    void resize(const simplex_dim& dim, size_t n)
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      mesh_property_[dim].resize(n);
    }

    void reserve(const simplex_dim& dim, size_t n)
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      mesh_property_[dim].reserve(n);
    }

    void clear(const simplex_dim& dim)
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      mesh_property_[dim].clear();
    }

    void swap(const simplex_dim& dim, size_t id0, size_t id1)
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      mesh_property_[dim].swap(id0, id1);
    }

    size_t prop_num(const simplex_dim& dim) const
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      return mesh_property_[dim].size();
    }

    size_t n_elements(const simplex_dim& dim) const
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      return mesh_property_[dim].n_elements();
    }

    void remove_property(const simplex_dim& dim, size_t prop_id)
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      mesh_property_[dim].remove_property(prop_id);
    }

    template<class T>
    void push_back(const simplex_dim&dim, size_t prop_id)
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      mesh_property_[dim].push_back<T>(prop_id);
    }

    template<class T>
    int get_specific_prop_index(const simplex_dim& dim, const T& prop, const std::string& name) const
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      return mesh_property_[dim].get_specific_prop_index(prop, name);
    }

    template<class T>
    const property<T>& get_specific_prop(const simplex_dim& dim, size_t prop_id) const
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      return mesh_property_[dim].get_specific_prop<T>(prop_id);
    }

    template<class T>
    property<T>& get_specific_prop(const simplex_dim& dim, size_t prop_id)
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      return mesh_property_[dim].get_specific_prop<T>(prop_id);
    }

    template<class T>
    void add_property(const simplex_dim& dim, const T& prop, const std::string name = "<unknown>")
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      mesh_property_[dim].add_property(prop, name);
    }

    template<class T>
    const T& get_element_property(const simplex_handle& sh, int prop_id) const
    {
      const simplex_dim dim = sh.dim();
      const simplex_dim ele_id = sh.id();
      const property<T>& props = get_specific_prop<T>(dim, prop_id);
      return props[ele_id];
    }

    template<class T>
    T& get_element_property(const simplex_handle &sh, int prop_id)
    {
      const simplex_dim& dim = sh.dim();
      const simplex_dim& ele_id = sh.id();
      property<T>& props = get_specific_prop<T>(dim, prop_id);
      return props[ele_id];
    }

  private:
    mesh_property_type mesh_property_;
  };
}

#endif // PROPERTY_MANAGER_H
