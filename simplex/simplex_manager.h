#ifndef IS_SIMPLEX_MANAGER_H
#define IS_SIMPLEX_MANAGER_H

#include "simpex.h"
#include "simplex_handle.h"

namespace is_mesh
{
  /**
    * This class stores all the simplex of the mesh and manage them. It stores differnet dimension simplexes and the
    * simplexes with same dimension are stored in a vector.
    */
  class simplex_manager
  {
  public:
    /// an alias used to define all the simplexes, see simplex_with_same_dim_type
    typedef std::vector<simplex_with_same_dim_type> mesh_simplices_type;

    /// This member function creates a new instance of this class.
    /** This member function creates a new instance of this class.
      * \param top_dim the dimension of the top simplex in the mesh, default value is 0
      */
    simplex_manager(size_t top_dim = 0) {mesh_simplices_.resize(top_dim + 1);}

    /** This function set the dimension of the top simplexes of the mesh.
      * \param top_dim the dimension of the top simplex in the mesh.
      */
    void set_dim(size_t top_dim) {mesh_simplices_.resize(top_dim + 1);}

    /** This function returns the dimension of the top simplexes of the mesh.
      * \return the dimension of the top simplexes of the mesh
      */
    size_t max_dim() const
    {
      return mesh_simplices_.size();
    }

    /** This function returns the size of the simplex_manager, in other words, it return the (top_dim + 1) of the mesh.
      * \return the size of simplex manager
      */
    size_t size() const
    {
      return mesh_simplices_.size();
    }

    /** This function resizes the simplex manager size
      * \param n the size we want to resize
      */
    void resize(size_t n)
    {
      mesh_simplices_.resize(n);
    }

    /** This function resizes the size of the simplex with same dimension
      * \param dim the dimension of the simplex which we wan to resize
      * \param n the size we want to resize
      */
    void resize(const simplex_dim& dim, size_t n)
    {
      assert(dim >= 0 && dim < mesh_simplices_.size());
      mesh_simplices_[dim].resize(n);
    }

    /** This function reserves the size of the simplex with same dimension
      * \param dim the dimension of the simplex which we wan to reserve
      * \param n the size we want to reserve
      */
    void reserve(const simplex_dim& dim, size_t n)
    {
      assert(dim >= 0 && dim < mesh_simplices_.size());
      mesh_simplices_[dim].reserve(n);
    }

    /** This function push a simplex into the simplex manager
      * \param dim the dimension of the simplex
      * \param sh the simplex which we want push back
      */
    void push_back(const simplex_dim& dim, const simplex& sh)
    {
      assert(dim >= 0 && dim < mesh_simplices_.size());
      mesh_simplices_[dim].push_back(sh);
    }

    /** This function push a simplex into the simplex manager
      * \param dim the dimension of the simplex
      */
    void push_back(const simplex_dim& dim)
    {
      assert(dim >= 0 && dim < mesh_simplices_.size());
      mesh_simplices_[dim].push_back(simplex());
    }

    /** This function return the size of the simplex with the same dimension
      * \param dim the simplex dimension of the simplex
      * \return the size of the simplex with the same dimension dim
      */
    size_t n_element(const simplex_dim& dim) const
    {
      return mesh_simplices_[dim].size();
    }

    /** This function clear the simplex with same dimension
      * \param dim the dimension the simplex which we want to clear
      */
    void clear(const simplex_dim& dim)
    {
      assert(dim >= 0 && dim < mesh_simplices_.size());
      mesh_simplices_[dim].clear();
      simplex_with_same_dim_type().swap(mesh_simplices_[dim]);
    }

    /** This function swap two simplexes with same dimension
      * \param dim the dimension of the swapped simplex
      * \param id0 the index of one simplex
      * \param id1 the index of another simplex
      */
    void swap(const simplex_dim& dim, size_t id0, size_t id1)
    {
      assert(dim >= 0 && dim < mesh_simplices_.size());
      assert(id0 >= 0 && id0 < n_element(dim));
      assert(id1 >= 0 && id1 < n_element(dim));
      std::swap(mesh_simplices_[dim][id0], mesh_simplices_[dim][id1]);
    }

    /** This function returns all the simplex of the mesh, it is a const version
      * \return all the simplex of the mesh
      */
    const mesh_simplices_type& get_mesh_simplices() const
    {
      return mesh_simplices_;
    }

    /** This function returns all the simplex of the mesh
      * \return all the simplex of the mesh
      */
    mesh_simplices_type& get_mesh_simplices()
    {
      return mesh_simplices_;
    }

    /** This function returns the simplex with same dimension, it is a const version
      * \param dim the given dimension the simplex
      * \return the simplex with given dimension of the mesh
      */
    const simplex_with_same_dim_type& get_simplex_with_same_dim(const simplex_dim& dim) const
    {
      assert(dim >= 0 && dim < mesh_simplices_.size());
      return mesh_simplices_[dim];
    }

    /** This function returns the simplex with same dimension
      * \param dim the given dimension the simplex
      * \return the simplex with given dimension of the mesh
      */
    simplex_with_same_dim_type& get_simplex_with_same_dim(const simplex_dim& dim)
    {
      assert(dim >= 0 && dim < mesh_simplices_.size());
      return mesh_simplices_[dim];
    }

    /** This function returns a simplex from a given simplex_handle, it is a const version
      * \param sh the given simplex_handle
      * \return the simplex get from the given simplex_handle
      */
    const simplex& get_specific_simplex(const simplex_handle& sh) const
    {
       const simplex_dim dim = sh.dim();
       const simplex_id id = sh.id();
       assert(dim >= 0 && dim < mesh_simplices_.size());
       assert(id >= 0 && id < n_element(dim));
       return mesh_simplices_[dim][id];
    }

    /** This function returns a simplex from a given simplex_handle
      * \param sh the given simplex_handle
      * \return the simplex get from the given simplex_handle
      */
    simplex& get_specific_simplex(const simplex_handle& sh)
    {
      const simplex_dim dim = sh.dim();
      const simplex_id id = sh.id();
      assert(dim >= 0 && dim < mesh_simplices_.size());
      assert(id >= 0 && id < n_element(dim));
      return mesh_simplices_[dim][id];
    }

  private:
    /// all the simplex of the mesh
    mesh_simplices_type mesh_simplices_;
  };
}

#endif // SIMPLEX_MANAGER_H
