#ifndef PROPERTY_H
#define PROPERTY_H

#include "base_property.h"
#include "../common/common.h"
namespace is_mesh
{
  template <class T>
  class property: public base_property
  {
  public:

    typedef T value_type;
    typedef std::vector<T> vector_type;

    property(const std::string& name="<unknown>"): base_property(name){}

    property(const property& rhs): base_property(rhs.name_), pro_vec_(rhs.pro_vec_){}

    virtual ~property() {}

    virtual void reserve(size_t n) {pro_vec_.reserve(n);}

    virtual void resize(size_t n) {pro_vec_.resize(n);}

    virtual void push_back() {pro_vec_.push_back(value_type());}

    virtual void clear() {pro_vec_.clear(); vector_type().swap(pro_vec_);}

    virtual void swap(size_t id0, size_t id1) {std::swap(pro_vec_[id0], pro_vec_[id1]);}

    virtual size_t n_elements() const {return pro_vec_.size();}

    virtual base_property* clone() const
    {
      property<T>* p = new property(*this);
      assert(p != NULL);
      return p;
    }

    const T& operator[] (int id) const
    {
      assert(id >= 0 && id < pro_vec_.size());
      return pro_vec_[id];
    }

    T& operator[] (int id)
    {
      assert(id >= 0 && id < pro_vec_.size());
      return pro_vec_[id];
    }

  private:
    vector_type pro_vec_;
  };
}
#endif // PROPERTY_H

