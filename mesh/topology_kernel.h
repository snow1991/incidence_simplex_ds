#ifndef IS_TOPOLOGY_KERNEL_H
#define IS_TOPOLOGY_KERNEL_H

#include "../property/property_manager.h"
#include "../property/status.h"
#include "../simplex/simplex_manager.h"

#include <boost/unordered_map.hpp>
#include <zjucad/matrix/matrix.h>

namespace is_mesh
{
  /// an alias used to define the type of coordinate
  typedef zjucad::matrix::matrix<double> coord_type;

  /**
    * This class is a topology kernel of the mesh, it includes some basic operations, such
    * as query adjacent information and construct the mesh.
    */
  class topology_kernel
  {
  public:

    /// an alias used to define the type of simplex with same dimension to handle map
    typedef boost::unordered_map<std::vector<size_t>, simplex_handle> map_type;

    /// an alias used to define the type all simplex to handle type
    typedef std::vector<boost::unordered_map<std::vector<size_t>, simplex_handle > > simplex2handle_type;

    /// This member function creates a new instance of this class.
    topology_kernel(): top_dim_(0), pm_(0), sm_(0), status_id_(-1), coord_id_(-1)
    {}

    /** This function set the dimension of the mesh, and add some basic properties to the mesh.
      * \param top_dim the dimension of the top simplex of the mesh
      */
    void set_dim(size_t top_dim = 0);

    /// This member function destroys an instance of class
    virtual ~topology_kernel() {}

    /** This function returns the property manager, it is a const version
      * \return the property manager
      */
    const property_manager& get_property_manager() const
    {return pm_;}

    /** This function returns the property manager
      * \return the property manager
      */
    property_manager& get_property_manager()
    {return pm_;}

    /** This function returns the simplex manager, it is a const version
      * \return the simplex manager
      */
    const simplex_manager& get_simplex_manager() const
    {return sm_;}

    /** This function returns the simplex manager
      * \return the simplex manager
      */
    simplex_manager& get_simplex_manager()
    {return sm_;}

    /** This function returns the all the simplex to handle vector
      * \return all the simplex to handle vector
      */
    const simplex2handle_type& get_simplex2handle() const
    {return simplex2handle_;}

    /** This function returns the all the simplex to handle vector
      * \return all the simplex to handle vector
      */
    simplex2handle_type& get_simplex2handle()
    { return simplex2handle_;}

    /** This function returns the dimension of the top simplexes of the mesh.
      * \return the dimension of the top simplexes of the mesh
      */
    size_t top_dim() const
    {
      return top_dim_;
    }

    /** This function returns the simplex is visited
      * \param sh the handle of given simplex
      * \return if the simplex is visited, return true, otherwise, false
      */
    bool is_simplex_visited(const simplex_handle& sh) const
    {
      return pm_.get_element_property<simplex_status>(sh, status_id_).is_visited();
    }

    /** This function set the simplex to be visited
      * \param sh the handle of given simplex
      */
    void set_simplex_visited(const simplex_handle& sh)
    {
      pm_.get_element_property<simplex_status>(sh, status_id_).set_visited();
    }

    /** This function set the simplex not to be visited
      * \param sh the handle of given simplex
      */
    void reset_simplex_visited(const simplex_handle& sh)
    {
      pm_.get_element_property<simplex_status>(sh, status_id_).reset_visited();
    }

    /** This function returns the simplex is deleted
      * \param sh the handle of given simplex
      * \return if the simplex is deleted, return true, otherwise, false
      */
    bool is_simplex_deleted(const simplex_handle& sh) const
    {
      return pm_.get_element_property<simplex_status>(sh, status_id_).is_deleted();
    }

    /** This function set the simplex to be deleted
      * \param sh the handle of given simplex
      */
    void set_simplex_deleted(const simplex_handle& sh)
    {
      pm_.get_element_property<simplex_status>(sh, status_id_).set_deleted();
    }

    /** This function set the simplex not to be deleted
      * \param sh the handle of given simplex
      */
    void reset_simplex_deleted(const simplex_handle& sh)
    {
      pm_.get_element_property<simplex_status>(sh, status_id_).reset_deleted();
    }

    /** This function returns a simplex handle is valid or not
      * \param sh the given handle
      * \return true if the handle is valid, otherwise false
      */
    bool is_valid_handle(const simplex_handle& sh) const
    {
      return (sh.dim() >= 0 && sh.dim() <= top_dim_ &&
              sh.id()  >= 0 && sh.id()  < sm_.n_element(sh.dim()));
    }

    /** This function set the coordinate of the given vertex
      * \param sh the simplex handle of given vertex
      * \param coord the coordinate of the vertex we want to set
      */
    void set_coord(const simplex_handle& sh, const coord_type& coord)
    {
      assert(sh.dim() == 0);
      assert(is_valid_handle(sh));
      pm_.get_element_property<coord_type>(sh, coord_id_) = coord;
    }

    /** This function returns the coordinate of the given vertex
      * \param sh the simplex handle of given vertex
      * \return the coordinate of the given vertex
      */
    const coord_type& get_coord(const simplex_handle& sh) const
    {
      assert(sh.dim() == 0);
      assert(is_valid_handle(sh));
      return pm_.get_element_property<coord_type>(sh, coord_id_);
    }

    /** This function new a top simplex
      * \param verts the vertex index of the top simplex
      * \param sh the simplex handle of the new top simplex
      * \return 0 if operation suncess othervise non-zero
      */
    int new_top_simplex(const std::vector<size_t>& verts, simplex_handle& sh);

    /** This function new a top simplex
      * \param verts the simplex handle of the top simplex
      * \param sh the simplex handle of the new top simplex
      * \return 0 if operation suncess othervise non-zero
      */
    int new_top_simplex(const std::vector<simplex_handle>& verts, simplex_handle& sh);

    /** This function new a vertex
      * \param id the index of the vertex
      * \param coord the coordinate of the new vertex
      * \param sh the simplex handle of the new vertex
      * \return 0 if operation suncess othervise non-zero
      */
    int new_vert(size_t id, const coord_type& coord, simplex_handle& sh);

    /** This function delete the simplex, in other words, it set the simplex and its' all
      * co_bounadry simplex deleted, be cautious to use it, because when the simplex set deleted,
      * it may causes errors when querying adjacent information. So make sure you need not query
      * adjacent information about deleted simplex when you use this function.
      * \param sh the handle of simplex to be deleted
      */
    int del_simplex(const simplex_handle& sh);

    /** This function used to query all boundary simplex of the given simplex
      * \param sh the handle of given simplex
      * \param bounds it stores all boundary simplex of the given simplex
      */
    void get_all_boundary_simplex(const simplex_handle& sh, std::vector<simplex_handle>& bounds);

    /** This function used to query the given dimension boundary simplex of the given simplex
      * \param sh the handle of given simplex
      * \param k the given dimesion
      * \param bounds it stores given dimension boundary simplex of the given simplex
      */
    void get_k_boundary_simplex(const simplex_handle& sh, size_t k, std::vector<simplex_handle>& bounds);

    /** This function used to query all co-boundary simplex of the given simplex
      * \param sh the handle of given simplex
      * \param co_bounds it stores all co-boundary simplex of the given simplex
      */
    void get_all_co_boundary_simplex(const simplex_handle& sh, std::vector<simplex_handle>& co_bounds);

    /** This function used to query the given dimension co-boundary simplex of the given simplex
      * \param sh the handle of given simplex
      * \param k the given dimesion
      * \param bounds it stores given dimension co-boundary simplex of the given simplex
      */
    void get_k_co_boundary_simplex(const simplex_handle& sh, size_t k, std::vector<simplex_handle>& co_bounds);

    /** This function used to query all adjacent simplex of the given simplex
      * \param sh the handle of given simplex
      * \param adjacent it stores all adjacent simplex of the given simplex
      */
    void get_adjacent_simplex(const simplex_handle& sh, std::vector<simplex_handle>& adjacent);

    /** This function get other vertexs except for the given vertexs from all vertexs of the given simplex
      * \param sh the handle of given simplex
      * \param given_verts the given vertexs
      * \param other_verts it stores other vertexs
      * \return 0 if the operation success othe-wise non-zero
      */
    int get_other_verts(const simplex_handle& sh, const std::vector<simplex_handle>& given_verts,
                        std::vector<simplex_handle>& other_verts);

    /** This function get the boundary of a given simplex which is opposite the given vertex, for example,
      *  we get a edge opposite to a given vertex in a given triangle mesh.
      * \param v_sh the handle of given vertex
      * \param sh the handle of given simplex
      * \param oppo_sh the handle we wanf get
      */
    int get_vertex_opposite_simplex(const simplex_handle& v_sh, const simplex_handle& sh,
                                    simplex_handle& oppo_sh);

    /// This function remove the deleted simplex, be careful when using it, it will change the handle
    /// of the simplex.
    int garbage_collector();

  protected:

    int new_simplex(const std::vector<size_t>& verts, simplex_handle& sh, bool& is_new);

    int new_tet(const std::vector<size_t>& verts, simplex_handle& sh);

    int new_tri(const std::vector<size_t>& verts, simplex_handle& sh);

    bool is_belong(const simplex_handle& low_sh, const simplex_handle& high_sh);

    bool is_belong(const std::vector<simplex_handle>& low_shs, const simplex_handle& high_sh);


    bool is_simplex_bound_visited(const simplex_handle& sh) const
    {
      return pm_.get_element_property<simplex_status>(sh, status_id_).is_bound_visited();
    }

    void set_simplex_bound_visited(const simplex_handle& sh)
    {
      pm_.get_element_property<simplex_status>(sh, status_id_).set_bound_visited();
    }

    void reset_simplex_bound_visited(const simplex_handle& sh)
    {
      pm_.get_element_property<simplex_status>(sh, status_id_).reset_bound_visited();
    }

    bool is_simplex_co_bound_visited(const simplex_handle& sh) const
    {
      return pm_.get_element_property<simplex_status>(sh, status_id_).is_co_bound_visited();
    }

    void set_simplex_co_bound_visited(const simplex_handle& sh)
    {
      pm_.get_element_property<simplex_status>(sh, status_id_).set_co_bound_visited();
    }

    void reset_simplex_co_bound_visited(const simplex_handle& sh)
    {
      pm_.get_element_property<simplex_status>(sh, status_id_).reset_co_bound_visited();
    }

    bool is_simplex_adjacent_visited(const simplex_handle& sh) const
    {
      return pm_.get_element_property<simplex_status>(sh, status_id_).is_adjacent_visited();
    }

    void set_simplex_adjacent_visited(const simplex_handle& sh)
    {
      pm_.get_element_property<simplex_status>(sh, status_id_).set_adjacent_visited();
    }

    void reset_simplex_adjacent_visited(const simplex_handle& sh)
    {
      pm_.get_element_property<simplex_status>(sh, status_id_).reset_adjacent_visited();
    }

  protected:
    /// property manager
    property_manager pm_;

    /// simplex manager
    simplex_manager sm_;

    /// simplex to handle type
    simplex2handle_type simplex2handle_;

    /// statuse property index
    int status_id_;

    /// coordinate property index
    int coord_id_;

    /// the dimension of the top simplex of the mesh
    size_t top_dim_;
  };
}

#endif // TOPOLOGY_KERNEL_H
