#ifndef IS_TOPOLOGY_KERNEL_H
#define IS_TOPOLOGY_KERNEL_H

#include "../property/property_manager.h"
#include "property/status.h"
#include "../simplex/simplex_manager.h"

#include <boost/unordered_map.hpp>

namespace is_mesh
{
  class topology_kernel
  {
  public:

    typedef std::vector<boost::unordered_map<std::vector<size_t>, simplex_handle > > simplex2handle_type;

    topology_kernel(size_t top_dim = 0):pm_(top_dim), sm_(top_dim)
    {
      if(top_dim > 0)
        simplex2handle_.resize(top_dim - 1);
      for(size_t i = 0; i < top_dim + 1; ++i)
        pm_.add_property(i, simplex_status(), "<status>");
      status_id_ = 0;
    }

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

    bool is_mainfold() const
    {return is_mainfold_;}

    bool is_boundary_simplex(const simplex_handle& sh) const
    {
      return pm_.get_element_property(sh, status_id_).is_boundary();
    }

    void set_simplex_boundary(const simplex_handle& sh)
    {
      pm_.get_element_property(sh, status_id_).set_boundary();
    }

    void reset_simplex_boundary(const simplex_handle& sh)
    {
      pm_.get_element_property(sh, status_id_).reset_boundary();
    }

    bool is_simplex_visited(const simplex_handle& sh) const
    {
      return pm_.get_element_property(sh, status_id_).is_visited();
    }

    void set_simplex_visited(const simplex_handle& sh)
    {
      pm_.get_element_property(sh, status_id_).set_visited();
    }

    void reset_simplex_visited(const simplex_handle& sh)
    {
      pm_.get_element_property(sh, status_id_).reset_visited();
    }

    bool is_simplex_deleted(const simplex_handle& sh) const
    {
      return pm_.get_element_property(sh, status_id_).is_deleted();
    }

    void set_simplex_deleted(const simplex_handle& sh)
    {
      pm_.get_element_property(sh, status_id_).set_deleted();
    }

    void reset_simplex_deleted(const simplex_handle& sh)
    {
      pm_.get_element_property(sh, status_id_).reset_deleted();
    }

    int new_simplex(const std::vector<size_t>& verts, simplex_handle& sh);

    int del_simplex(const simplex_handle& sh);

    void get_all_boundary_simplex(const simplex_handle& sh, std::vector<simplex_handle>& bounds) const;

    void get_k_boundary_simplex(const simplex_handle& sh, size_t k, std::vector<simplex_handle>& bounds) const;

    void get_all_co_boundary_simplex(const simplex_handle& sh, std::vector<simplex_handle>& co_bounds) const;

    void get_k_co_boundary_simplex(const simplex_handle& sh, size_t k, std::vector<simplex_handle>& co_bounds) const;

    int garbage_collector();

  protected:

  private:
    property_manager pm_;
    simplex_manager sm_;
    simplex2handle_type simplex2handle_;
    size_t status_id_;
    bool is_mainfold_;
  };
}

#endif // TOPOLOGY_KERNEL_H
