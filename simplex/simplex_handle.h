#ifndef IS_SIMPLEX_HANDLE_H
#define IS_SIMPLEX_HANDLE_H

#include "../common/common.h"

namespace is_mesh
{
  /// an alias used to define the simplex dimension
  typedef unsigned int simplex_dim;

  /// an alias used to define the position of the simplex in container
  typedef unsigned int simplex_id;

  /// a handle to a simplex directly encoded in the IS data structure
  /**
   * This class decribes a handle to a simplex dirctly encoded in the IS data structure. A handle to a simplex
   * is composed by the dimension and position of the simplex.This class is used to extract all the topological
   * relations and to access properties of a simplex.
   */
  class simplex_handle
  {
  public:

    /// This member function creates a new instance of this class.
    /** In this member function we assign dimension and identifier for the simplex.
      * \param dim the dimension of the simplexto be reffered by the new simplex handle, default value is -1
      * \param id the identifier of the simplexto be reffered by the new simplex handle, default value is -1
      */
    simplex_handle(simplex_dim dim = -1, simplex_id id = -1):dim_(dim), id_(id) {}

    /// This member function creates a new instance of this class.
    /** In this function, we create a copy of the input simplex handle
      * \param rhs the simplex handle to be copied
      */
    simplex_handle(const simplex_handle& rhs):dim_(rhs.dim_), id_(rhs.id_){}

    /// This member function destroys an instance of class
    virtual ~simplex_handle() {}

    /// This member function returns the dimension of the simplex handle
    /** \return the dimension of the current simplex handle
      */
    inline simplex_dim dim() const
    {return dim_;}

    /// This member function updates the dimension of the simplex handle
    /** \param dim the dimension to be updated by the current simplex handle
      */
    inline void set_dim(simplex_dim dim)
    {dim_ = dim;}

    /// This member function returns the identifier of the simplex handle
    /** \return the identifier of the current simplex handle
      */
    inline simplex_id id() const
    {return id_;}

    /// This member function updates the identifier of the simplex handle
    /** \param id the identifier to be updated by the current simplex handle
      */
    inline void set_id(simplex_id id)
    {id_ = id;}

    /// This operator checks if two simplex handls are equal
    /** In this operator, two simplex handles are equal if and only if the dimension
      * and the identifier of these two handles are equal.
      * \param rhs the simplex handle to be compared with the current one
      * \return true if the  two simplex handles are equal, otherwise, false
      */
    bool operator== (const simplex_handle& rhs)
    {
      return (id_ == rhs.id_) && (dim_ == rhs.dim_);
    }

    /// This operator compares two simplex handles
    /** In this operator, we compare two simplex handles, the first handle is the current
      * one and the second is the input.
      * \param rhs the simplex handle to be compared with the current one
      * \return true if the first handle is less than the second one, othserwise, false.
      */
    bool operator< (const simplex_handle& rhs)
    {
      if(dim_ == rhs.dim_)
        return id_ < rhs.id_;
      return dim_ < rhs.dim_;
    }

  protected:
    /// The dimension of the reffered simplex
    simplex_dim dim_;

    /// The identifier of the reffered simplex
    simplex_id id_;
  };

}


#endif // IS_SIMPLEX_HANDLE_H
