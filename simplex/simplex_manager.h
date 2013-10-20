#ifndef IS_SIMPLEX_MANAGER_H
#define IS_SIMPLEX_MANAGER_H

#include "simpex.h"
#include "simplex_handle.h"

namespace is_mesh
{
  class simplex_manager
  {
  public:

    typedef std::vector<simplex_with_same_dim_type> mesh_simplices_type;

    simplex_manager(size_t top_dim = 0) {mesh_simplices_.resize(top_dim + 1);}

    void set_dim(size_t top_dim) {mesh_simplices_.resize(top_dim + 1);}

    size_t max_dim() const
    {
      return mesh_simplices_.size();
    }

    size_t size() const
    {
      return mesh_simplices_.size();
    }

    void resize(size_t n)
    {
      mesh_simplices_.resize(n);
    }

    void resize(const simplex_dim& dim, size_t n)
    {
      assert(dim >= 0 && dim < mesh_simplices_.size());
      mesh_simplices_[dim].resize(n);
    }

    void reserve(const simplex_dim& dim, size_t n)
    {
      assert(dim >= 0 && dim < mesh_simplices_.size());
      mesh_simplices_[dim].reserve(n);
    }

    void push_back(const simplex_dim& dim, const simplex& sh)
    {
      assert(dim >= 0 && dim < mesh_simplices_.size());
      mesh_simplices_[dim].push_back(sh);
    }

    void push_back(const simplex_dim& dim)
    {
      assert(dim >= 0 && dim < mesh_simplices_.size());
      mesh_simplices_[dim].push_back(simplex());
    }

    size_t n_element(const simplex_dim& dim) const
    {
      return mesh_simplices_[dim].size();
    }

    void clear(const simplex_dim& dim)
    {
      assert(dim >= 0 && dim < mesh_simplices_.size());
      mesh_simplices_[dim].clear();
      simplex_with_same_dim_type().swap(mesh_simplices_[dim]);
    }

    void swap(const simplex_dim& dim, size_t id0, size_t id1)
    {
      assert(dim >= 0 && dim < mesh_simplices_.size());
      assert(id0 >= 0 && id0 < n_element(dim));
      assert(id1 >= 0 && id1 < n_element(dim));
      std::swap(mesh_simplices_[dim][id0], mesh_simplices_[dim][id1]);
    }

    const mesh_simplices_type& get_mesh_simplices() const
    {
      return mesh_simplices_;
    }

    mesh_simplices_type& get_mesh_simplices()
    {
      return mesh_simplices_;
    }

    const simplex_with_same_dim_type& get_simplex_with_same_dim(const simplex_dim& dim) const
    {
      assert(dim >= 0 && dim < mesh_simplices_.size());
      return mesh_simplices_[dim];
    }

    simplex_with_same_dim_type& get_simplex_with_same_dim(const simplex_dim& dim)
    {
      assert(dim >= 0 && dim < mesh_simplices_.size());
      return mesh_simplices_[dim];
    }

    const simplex& get_specific_simplex(const simplex_handle& sh) const
    {
       const simplex_dim dim = sh.dim();
       const simplex_id id = sh.id();
       assert(dim >= 0 && dim < mesh_simplices_.size());
       assert(id >= 0 && id < n_element(dim));
       return mesh_simplices_[dim][id];
    }

    simplex& get_specific_simplex(const simplex_handle& sh)
    {
      const simplex_dim dim = sh.dim();
      const simplex_id id = sh.id();
      assert(dim >= 0 && dim < mesh_simplices_.size());
      assert(id >= 0 && id < n_element(dim));
      return mesh_simplices_[dim][id];
    }

  private:
    mesh_simplices_type mesh_simplices_;
  };
}

#endif // SIMPLEX_MANAGER_H
