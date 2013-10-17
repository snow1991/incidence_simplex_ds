#ifndef IS_SIMPLEX_H
#define IS_SIMPLEX_H

#include "../common/common.h"
#include "simplex_handle.h"

namespace is_mesh
{
  class simplex
  {
    enum boundary_flg_type
    {
      BOUNDARY = 1,
      CO_BOUNDARY = 2
    };

  public:

    simplex() {}

    virtual ~simplex() {}

    size_t boundary_size() const
    {
      return boundary_.size();
    }

    size_t par_co_boundary_size() const
    {
      return par_co_boundary_.size();
    }

    void resize_boundary(size_t n)
    {
      boundary_.resize(n);
    }

    void resize_par_co_boundary(size_t n)
    {
      par_co_boundary_.resize(n);
    }

    const simplex_handle& get_ith_ele(size_t id, boundary_flg_type flg) const
    {
      if(flg == BOUNDARY)
        {
          assert(id >= 0 && id < boundary_size());
          return boundary_[id];
        }
      else if(flg == CO_BOUNDARY)
        {
          assert(id >= 0 && id < par_co_boundary_size());
          return par_co_boundary_[id];
        }
    }

    simplex_handle& get_ith_ele(size_t id, boundary_flg_type flg)
    {
      if(flg == BOUNDARY)
        {
          assert(id >= 0 && id < boundary_size());
          return boundary_[id];
        }
      else if(flg == CO_BOUNDARY)
        {
          assert(id >= 0 && id < par_co_boundary_size());
          return par_co_boundary_[id];
        }
    }

    const std::vector<simplex_handle>& get_boundary() const
    {return boundary_;}

    std::vector<simplex_handle>& get_boundary()
    {return boundary_;}

    const std::vector<simplex_handle>& get_par_co_boundary() const
    {return par_co_boundary_;}

    std::vector<simplex_handle>& get_par_co_boundary()
    {return par_co_boundary_;}

  private:
    std::vector<simplex_handle> boundary_;
    std::vector<simplex_handle> par_co_boundary_;
  };

  typedef std::vector<simplex> simplex_with_same_dim_type;
}

#endif // IS_SIMPLEX_H
