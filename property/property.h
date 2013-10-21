#ifndef PROPERTY_H
#define PROPERTY_H

#include "base_property.h"
#include "../common/common.h"
namespace is_mesh
{
  /**
    * This class is a template class of the property inherited from base_property, the value type
    * of the property is T.
    */
  template <class T>
  class property: public base_property
  {
  public:

    /// an alias used to define the value type
    typedef T value_type;

    /// an alias used to define the vector type
    typedef std::vector<T> vector_type;

    /// This member function creates a new instance of this class.
    property(const std::string& name="<unknown>"): base_property(name){}

    /// copy construct function
    property(const property& rhs): base_property(rhs.name_), pro_vec_(rhs.pro_vec_){}

    /// This member function destroys an instance of class
    virtual ~property() {}

    /// This function reserves the size of the property, see base_property
    virtual void reserve(size_t n) {pro_vec_.reserve(n);}

    /// This function resizes the size of the property, see base_property
    virtual void resize(size_t n) {pro_vec_.resize(n);}

    /// This function push back a default property, see base_property
    virtual void push_back() {pro_vec_.push_back(value_type());}

    /// This function clear the property, see base_property
    virtual void clear() {pro_vec_.clear(); vector_type().swap(pro_vec_);}

    /// This function swap two properties, see base_property
    virtual void swap(size_t id0, size_t id1) {std::swap(pro_vec_[id0], pro_vec_[id1]);}

    /// This function return the number of the element of the current property, see base_property
    virtual size_t n_elements() const {return pro_vec_.size();}

    /// This function clone the current property, see base_property
    virtual base_property* clone() const
    {
      property<T>* p = new property(*this);
      assert(p != NULL);
      return p;
    }

    /** This function overloads operator [], and returns the element if given index, it is a const version
      * \param id the given index
      * \return the element whose index is id
      */
    const T& operator[] (int id) const
    {
      assert(id >= 0 && id < pro_vec_.size());
      return pro_vec_[id];
    }

    /** This function overloads operator [], and returns the element if given index
      * \param id the given index
      * \return the element whose index is id
      */
    T& operator[] (int id)
    {
      assert(id >= 0 && id < pro_vec_.size());
      return pro_vec_[id];
    }

  private:
    /// vector to store the element property
    vector_type pro_vec_;
  };
}
#endif // PROPERTY_H

