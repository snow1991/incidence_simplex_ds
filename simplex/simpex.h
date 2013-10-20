#ifndef IS_SIMPLEX_H
#define IS_SIMPLEX_H

#include "../common/common.h"
#include "simplex_handle.h"

namespace is_mesh
{
  /**
    * This class stores the info of a simplex, it consists of all of the boundary simplexes and a partial co_boundary
    * simplex of the simplex. If the simplex is a top one, the parial co_boundary is empty, and if the simplex is a
    * second top one, the partial co_boundary consist of store all the co_boundaries. If the simplex is a vertex, the
    * boundary of it is empty.
    */
  class simplex
  {
    /// simplex boundary type
    enum boundary_flg_type
    {
      BOUNDARY = 1,
      CO_BOUNDARY = 2
    };

  public:

    /// This member function creates a new instance of this class.
    simplex() {}

    /// This member function destroys an instance of class
    virtual ~simplex() {}

    /** this function retruns the size of the boundary of the current simplex
      * \return the boundary size of the current simplex
      */
    size_t boundary_size() const
    {
      return boundary_.size();
    }

    /** this function retruns the size of the partial co_boundary of the current simplex
      * \return the partial co_boundary size of the current simplex
      */
    size_t par_co_boundary_size() const
    {
      return par_co_boundary_.size();
    }

    /** this function resizes the boundary size of the current simplex
      * \param the boundary size we wnat to resize
      */
    void resize_boundary(size_t n)
    {
      boundary_.resize(n);
    }

    /** this function resizes the partial co_boundary size of the current simplex
      * \param the partial co_boundary size we wnat to resize
      */
    void resize_par_co_boundary(size_t n)
    {
      par_co_boundary_.resize(n);
    }

    /** this function get the i'th element of the boundary or partial co_boundary of the current simplex
      * and it is a const version
      * \param id the index of the element we want to get
      * \param flg flg is a flag, see boundary_flg_type
      * \return the element we wan to get
      */
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

    /** this function get the i'th element of the boundary or partial co_boundary of the current simplex
      * \param id the index of the element we want to get
      * \param flg flg is a flag, see boundary_flg_type
      * \return the element we wan to get
      */
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

    /** This function return the boundary of the current simplex
      * and it is a const version
      * \return the boundary of the current simplex
      */
    const std::vector<simplex_handle>& get_boundary() const
    {return boundary_;}

    /** This function return the boundary of the current simplex
      * \return the boundary of the current simplex
      */
    std::vector<simplex_handle>& get_boundary()
    {return boundary_;}

    /** This function return the partial co_boundary of the current simplex
      * and it is a const version
      * \return the partial co_boundary of the current simplex
      */
    const std::vector<simplex_handle>& get_par_co_boundary() const
    {return par_co_boundary_;}

    /** This function return the partial co_boundary of the current simplex
      * \return the partial co_boundary of the current simplex
      */
    std::vector<simplex_handle>& get_par_co_boundary()
    {return par_co_boundary_;}

  private:
    /// the vector stores the boundary simplex of the current simplex
    std::vector<simplex_handle> boundary_;

    /// the vector stores the partial co_bounday simplex of the current simplex
    std::vector<simplex_handle> par_co_boundary_;
  };

  /// an alias used to define the simplex with the same dimension
  typedef std::vector<simplex> simplex_with_same_dim_type;
}

#endif // IS_SIMPLEX_H
