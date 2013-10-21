#ifndef IS_MESH_H
#define IS_MESH_H

#include "topology_kernel.h"

namespace is_mesh
{
  /**
    * This class is inherited from the topology kernel, and it is a supplement of the topology kernel.
    */
  class mesh:public topology_kernel
  {
  public:

    /** This function add a property on the simplex whose dimension is given
      * \param dim the dimension of the property
      * \param prop a default value of the property
      * \param name the name of the property
      */
    template <typename T>
    void add_property(const simplex_dim& dim, const T& prop, std::string prop_name = "<unknown>")
    {
       pm_.add_property(dim, prop, prop_name);
    }

    /** This function returns the property value of a given simplex, it is a const version
      * \param sh the simplex_handle of the given simplex
      * \param prop_id the index of the property
      * \return the property value of the given simplex
      */
    template<typename T>
    const T& get_simplex_property(const simplex_handle& sh, size_t prop_id) const
    {
      return pm_.get_element_property<T>(sh, prop_id);
    }

    /** This function returns the property value of a given simplex
      * \param sh the simplex_handle of the given simplex
      * \param prop_id the index of the property
      * \return the property value of the given simplex
      */
    template<typename T>
    T& get_simplex_property(const simplex_handle& sh, size_t prop_id)
    {
      return pm_.get_element_property<T>(sh, prop_id);
    }


    /** This function returns the given property index
      * \param dim the dimension of the property
      * \param prop a default value of the property
      * \param name the name of the property
      * \return the index of the given property
      */
    template<typename T>
    int get_prop_id(const simplex_dim& dim, const T& prop, std::string prop_name = "<unknown>") const
    {
      return pm_.get_specific_prop_index(dim, prop, prop_name);
    }

    /** This function returns the simplex by given simplex handle, it is a const version
      * \param sh the simplex handle
      * \return the simplex which the handle reffered
      */
    const simplex& get_specific_simplex(const simplex_handle& sh) const
    {
      return sm_.get_specific_simplex(sh);
    }

    /** This function returns the simplex by given simplex handle
      * \param sh the simplex handle
      * \return the simplex which the handle reffered
      */
    simplex& get_specific_simplex(const simplex_handle& sh)
    {
      return sm_.get_specific_simplex(sh);
    }

    /** This function return the handle of the given vertexs
      * \param verts the given vertexs
      * \return the handle of given vertexs
      */
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

    /** This function return length of the given edge
      * \param sh the handle of given edge
      * \return the length of the egde
      */
    double get_length(const simplex_handle& sh);

    /** This function return area of the given face
      * \param sh the handle of given face
      * \return the area of the face
      */
    double get_area(const simplex_handle& sh);

    /** This function return volume of the given tet
      * \param sh the handle of given tet
      * \return the volume of the tet
      */
    double get_volume(const simplex_handle& sh);

    /** This function get the normal of the given face
      * \param sh the handle of given face
      * \param face_normal the normal of the face, it is normalized
      * \return 0 if the operation success otherwise non-zero
      */
    int get_face_normal(const simplex_handle& sh, matrixd& face_normal);

  private:
  };
}
#endif // MESH_H
