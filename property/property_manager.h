#ifndef IS_PROPERTY_MANAGER_H
#define IS_PROPERTY_MANAGER_H

#include "../simplex/simplex_handle.h"
#include "simplex_property.h"

namespace is_mesh
{
  /**
    * This class stores all the properties of the simplex of the mesh and manage them. It stores differnet
    * dimension simplexes' property in a vector.
    */
  class property_manager
  {
  public:
    /// an alias used to define all properties of the mesh
    typedef std::vector<simplex_property> mesh_property_type;

    /// This member function creates a new instance of this class.
    /** This member function creates a new instance of this class.
      * \param top_dim the dimension of the top simplex in the mesh, default value is 0
      */
    property_manager(size_t top_dim = 0) {mesh_property_.resize(top_dim + 1);}

    /** This function set the dimension of the top simplexes of the mesh.
      * \param top_dim the dimension of the top simplex in the mesh.
      */
    void set_dim(size_t top_dim) {mesh_property_.resize(top_dim + 1);}

    /** This function returns the dimension of the top simplexes of the mesh.
      * \return the dimension of the top simplexes of the mesh
      */
    size_t max_dim() const
    {return mesh_property_.size();}

    /** This function resizes the size of the properties of the simplex with same dimension
      * \param dim the dimension of the simplex which we wan to resize
      * \param n the size we want to resize
      */
    void resize(const simplex_dim& dim, size_t n)
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      mesh_property_[dim].resize(n);
    }

    /** This function resrves the size of the properties of the simplex with same dimension
      * \param dim the dimension of the simplex which we wan to reserve
      * \param n the size we want to resize
      */
    void reserve(const simplex_dim& dim, size_t n)
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      mesh_property_[dim].reserve(n);
    }

    /** This function clear the properties of the simplex with same dimension
      * \param dim the dimension the simplex which we want to clear
      */
    void clear(const simplex_dim& dim)
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      mesh_property_[dim].clear();
    }

    /** This function swap two simplexes' properties with same dimension
      * \param dim the dimension of the swapped simplex
      * \param id0 the index of one property
      * \param id1 the index of another property
      */
    void swap(const simplex_dim& dim, size_t id0, size_t id1)
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      mesh_property_[dim].swap(id0, id1);
    }

    /** This function returns the number of properties of the given dimension simplex
      * \param dim the given dimension
      * \return the number of properties of the given dimension simplex
      */
    size_t prop_num(const simplex_dim& dim) const
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      return mesh_property_[dim].size();
    }

    /** This function return the size of the simplex with the same dimension
      * \param dim the simplex dimension of the simplex
      * \return the size of the simplex with the same dimension dim
      */
    size_t n_elements(const simplex_dim& dim) const
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      return mesh_property_[dim].n_elements();
    }

    /** This function remove the property by given the dimension and the property index
      * \param dim the dimension of simplex which the property belonging to
      * \param prop_id the index of the property
      */
    void remove_property(const simplex_dim& dim, size_t prop_id)
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      mesh_property_[dim].remove_property(prop_id);
    }

    /** This function push back a default value into the given property
      * \param dim the dimension of the given property
      * \param prop_id the index of the given property
      */
    template<class T>
    void push_back(const simplex_dim&dim, size_t prop_id)
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      mesh_property_[dim].push_back<T>(prop_id);
    }

    /** This function returns the given property index
      * \param dim the dimension of the property
      * \param prop a default value of the property
      * \param name the name of the property
      * \return the index of the given property
      */
    template<class T>
    int get_specific_prop_index(const simplex_dim& dim, const T& prop, const std::string& name) const
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      return mesh_property_[dim].get_specific_prop_index(prop, name);
    }

    /** This function returns the property we want to get, it is a const version
      * \param dim the dimension of the property
      * \param prop_id the index of the given property
      * \return the property we want to get
      */
    template<class T>
    const property<T>& get_specific_prop(const simplex_dim& dim, size_t prop_id) const
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      return mesh_property_[dim].get_specific_prop<T>(prop_id);
    }

    /** This function returns the property we want to get
      * \param dim the dimension of the property
      * \param prop_id the index of the given property
      * \return the property we want to get
      */
    template<class T>
    property<T>& get_specific_prop(const simplex_dim& dim, size_t prop_id)
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      return mesh_property_[dim].get_specific_prop<T>(prop_id);
    }

    /** This function add a property on the simplex whose dimension is given
      * \param dim the dimension of the property
      * \param prop a default value of the property
      * \param name the name of the property
      */
    template<class T>
    void add_property(const simplex_dim& dim, const T& prop, const std::string name = "<unknown>")
    {
      assert(dim >= 0 && dim < mesh_property_.size());
      mesh_property_[dim].add_property(prop, name);
    }

    /** This function returns the property value of a given simplex, it is a const version
      * \param sh the simplex_handle of the given simplex
      * \param prop_id the index of the property
      * \return the property value of the given simplex
      */
    template<class T>
    const T& get_element_property(const simplex_handle& sh, int prop_id) const
    {
      const simplex_dim dim = sh.dim();
      const simplex_dim ele_id = sh.id();
      const property<T>& props = get_specific_prop<T>(dim, prop_id);
      return props[ele_id];
    }

    /** This function returns the property value of a given simplex
      * \param sh the simplex_handle of the given simplex
      * \param prop_id the index of the property
      * \return the property value of the given simplex
      */
    template<class T>
    T& get_element_property(const simplex_handle &sh, int prop_id)
    {
      const simplex_dim& dim = sh.dim();
      const simplex_dim& ele_id = sh.id();
      property<T>& props = get_specific_prop<T>(dim, prop_id);
      return props[ele_id];
    }

  private:
    /// properties of all simplex in the mesh
    mesh_property_type mesh_property_;
  };
}

#endif // PROPERTY_MANAGER_H
