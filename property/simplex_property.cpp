#include "simplex_property.h"

namespace is_mesh
{
  /* =================== IMPLEMENTATION of CLASS simplex_property ===================== */
  simplex_property::simplex_property(const simplex_property &rhs)
  {
    if(&rhs != this)
      {
        std::for_each(dim_property_.begin(), dim_property_.end(), delete_functor());
        dim_property_ = rhs.dim_property_;
        for(size_t i = 0; i < dim_property_.size(); ++i)
          {
            if(rhs.dim_property_[i]  != NULL)
              dim_property_[i] = rhs.dim_property_[i]->clone();
          }
      }
  }

  simplex_property& simplex_property::operator= (const simplex_property& rhs)
  {
    if(&rhs != this)
      {
        std::for_each(dim_property_.begin(), dim_property_.end(), delete_functor());
        dim_property_ = rhs.dim_property_;
        for(size_t i = 0; i < dim_property_.size(); ++i)
          {
            if(rhs.dim_property_[i]  != NULL)
              dim_property_[i] = rhs.dim_property_[i]->clone();
          }
      }
    return *this;
  }

  simplex_property::~simplex_property()
  {
    std::for_each(dim_property_.begin(), dim_property_.end(), delete_functor());
  }

  void simplex_property::remove_property(size_t id)
  {
    assert(id >= 0 && id <= dim_property_.size());
    if(dim_property_[id] != NULL)
      delete dim_property_[id];
    dim_property_[id] = NULL;
  }

  void simplex_property::resize(size_t n)
  {
    std::for_each(dim_property_.begin(), dim_property_.end(), resize_functor(n));
  }

  void simplex_property::reserve(size_t n)
  {
    std::for_each(dim_property_.begin(), dim_property_.end(), reserve_functor(n));
  }

  void simplex_property::clear()
  {
    std::for_each(dim_property_.begin(), dim_property_.end(), clear_functor());
  }

  void simplex_property::swap(size_t id0, size_t id1)
  {
    std::for_each(dim_property_.begin(), dim_property_.end(), swap_functor(id0, id1));
  }
}
