#ifndef IS_TOPOLOGY_KERNEL_H
#define IS_TOPOLOGY_KERNEL_H

#include "../property/property_manager.h"
#include "../property/status.h"
#include "../simplex/simplex_manager.h"

#include <boost/unordered_map.hpp>
#include <zjucad/matrix/matrix.h>

namespace is_mesh
{
  typedef zjucad::matrix::matrix<double> coord_type;

  class topology_kernel
  {
  public:

    typedef boost::unordered_map<std::vector<size_t>, simplex_handle> map_type;
    typedef std::vector<boost::unordered_map<std::vector<size_t>, simplex_handle > > simplex2handle_type;

    topology_kernel(): top_dim_(0), pm_(0), sm_(0), status_id_(-1), coord_id_(-1)
    {}

    void set_dim(size_t top_dim = 0);

    virtual ~topology_kernel() {}

    const property_manager& get_property_manager() const
    {return pm_;}

    property_manager& get_property_manager()
    {return pm_;}

    const simplex_manager& get_simplex_manager() const
    {return sm_;}

    simplex_manager& get_simplex_manager()
    {return sm_;}

    const simplex2handle_type& get_simplex2handle() const
    {return simplex2handle_;}

    simplex2handle_type& get_simplex2handle()
    { return simplex2handle_;}

    size_t top_dim() const
    {
      return top_dim_;
    }

    bool is_boundary_simplex(const simplex_handle& sh) const
    {
      return pm_.get_element_property<simplex_status>(sh, status_id_).is_boundary();
    }

    void set_simplex_boundary(const simplex_handle& sh)
    {
      pm_.get_element_property<simplex_status>(sh, status_id_).set_boundary();
    }

    void reset_simplex_boundary(const simplex_handle& sh)
    {
      pm_.get_element_property<simplex_status>(sh, status_id_).reset_boundary();
    }

    bool is_simplex_visited(const simplex_handle& sh) const
    {
      return pm_.get_element_property<simplex_status>(sh, status_id_).is_visited();
    }

    void set_simplex_visited(const simplex_handle& sh)
    {
      pm_.get_element_property<simplex_status>(sh, status_id_).set_visited();
    }

    void reset_simplex_visited(const simplex_handle& sh)
    {
      pm_.get_element_property<simplex_status>(sh, status_id_).reset_visited();
    }

    bool is_simplex_deleted(const simplex_handle& sh) const
    {
      return pm_.get_element_property<simplex_status>(sh, status_id_).is_deleted();
    }

    void set_simplex_deleted(const simplex_handle& sh)
    {
      pm_.get_element_property<simplex_status>(sh, status_id_).set_deleted();
    }

    void reset_simplex_deleted(const simplex_handle& sh)
    {
      pm_.get_element_property<simplex_status>(sh, status_id_).reset_deleted();
    }

    bool is_valid_handle(const simplex_handle& sh) const
    {
      return (sh.dim() >= 0 && sh.dim() <= top_dim_ &&
              sh.id()  >= 0 && sh.id()  < sm_.n_element(sh.dim()));
    }

    void set_coord(const simplex_handle& sh, const coord_type& coord)
    {
      assert(sh.dim() == 0);
      assert(is_valid_handle(sh));
      pm_.get_element_property<coord_type>(sh, coord_id_) = coord;
    }

    const coord_type& get_coord(const simplex_handle& sh) const
    {
      assert(sh.dim() == 0);
      assert(is_valid_handle(sh));
      return pm_.get_element_property<coord_type>(sh, coord_id_);
    }

    int new_top_simplex(const std::vector<size_t>& verts, simplex_handle& sh);

    int new_top_simplex(const std::vector<simplex_handle>& verts, simplex_handle& sh);

    int new_vert(size_t id, const coord_type& coord, simplex_handle& sh);

    int del_simplex(const simplex_handle& sh);

    void get_all_boundary_simplex(const simplex_handle& sh, std::vector<simplex_handle>& bounds);

    void get_k_boundary_simplex(const simplex_handle& sh, size_t k, std::vector<simplex_handle>& bounds);

    void get_all_co_boundary_simplex(const simplex_handle& sh, std::vector<simplex_handle>& co_bounds);

    void get_k_co_boundary_simplex(const simplex_handle& sh, size_t k, std::vector<simplex_handle>& co_bounds);

    void get_adjacent_simplex(const simplex_handle& sh, std::vector<simplex_handle>& adjacent);

    int get_other_verts(const simplex_handle& sh, const std::vector<simplex_handle>& given_verts,
                        std::vector<simplex_handle>& other_verts);

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
    property_manager pm_;
    simplex_manager sm_;
    simplex2handle_type simplex2handle_;
    int status_id_;
    int coord_id_;
    size_t top_dim_;
  };
}

#endif // TOPOLOGY_KERNEL_H
