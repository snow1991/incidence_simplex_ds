#ifndef IS_SIMPLEX_PROPERTY_H
#define IS_SIMPLEX_PROPERTY_H

#include "property.h"

namespace is_mesh
{
  /**
    * This class is the properties of the simplex with same dimension, for example, the vertex has color property,
    * coordinate property and so on.
    */
  class simplex_property
  {
  public:
    /// an alias used to define the properties of the simplex with same dimension
    typedef std::vector<base_property*> dim_property_type;

    /// This function creats a new instace of the class
    simplex_property() {}

    /// copy construct function
    simplex_property(const simplex_property& rhs);

    /// assignment construct function
    simplex_property& operator= (const simplex_property& rhs);

    /// This member function destroys an instance of class
    virtual ~simplex_property();

    /** This function return the properties of the simplex with same dimension, it is a const version
      * \return the properties of the simplex with same dimension
      */
    const dim_property_type& get_dim_property() const
    {return dim_property_;}

    /** This function return the properties of the simplex with same dimension, it is a const version
      * \return the properties of the simplex with same dimension
      */
    dim_property_type& get_dim_property()
    {return dim_property_;}

    /** This function resizes the properties of the simplex with same dimension
      * \param n the size we want to resize
      */
    void resize(size_t n);

    /** This function reserves the properties of the simplex with same dimension
      * \param n the size we want to reserve
      */
    void reserve(size_t n);

    /// This function clear all the properties of the simplex with same dimension
    void clear();

    /** This function swap the property which were given by index
      * \param id0 the index of one property
      * \param id1 the index of another property
      */
    void swap(size_t id0, size_t id1);

    /** This function returns the size of property, in other words, the number of properties
      * \return the size of property
      */
    size_t size() const {return dim_property_.size();}

    /** This function returns the element of the simplex, in other words, it returns the number of element of each property
      * \return the elemet of the simplex
      */
    size_t n_elements() const
    {
      size_t elements = 0;
      for(size_t i = 0; i < dim_property_.size(); ++i)
        if(dim_property_[i] != NULL)
          {
            elements = dim_property_[i]->n_elements();
            break;
          }
      return elements;
    }

    /** This function returns the index of the given property, the property was decided by its' name
      * \return the index of the given property in the vector
      */
    template<class T>
    int get_specific_prop_index(const T&, const std::string& prop_name) const;

    /** This function returns the given property by its' index, it is a const version
      * \param prop_id the given index of the property
      * \return the property
      */
    template<class T>
    const property<T>& get_specific_prop(size_t prop_id) const;

    /** This function returns the given property by its' index
      * \param prop_id the given index of the property
      * \return the property
      */
    template<class T>
    property<T>& get_specific_prop(size_t prop_id);

    /** This function push a default property value into the given property by index
      * \param prop_id the index of the property
      */
    template <class T>
    void push_back(size_t prop_id)
    {

      property<T>& props = get_specific_prop<T>(prop_id);
      props.push_back();
    }

    /** This function add a property of the simplex of the same dimension
      * \param prop the default value of the type T
      * \param prop_name the name of the added property, dafault value is "<unknown>"
      */
    template<class T>
    void add_property(const T& prop, const std::string& prop_name="<unknown>");

    /** This function remove the property given by its' index
      * \param the index of the peoperty
      */
    void remove_property(size_t id);

  protected:

    struct resize_functor
    {
      resize_functor(size_t n):n_(n) {}
      void operator() (base_property* p) const
      {
        if(p != NULL)
          p->resize(n_);
      }
      size_t n_;
    };

    struct reserve_functor
    {
      reserve_functor(size_t n):n_(n) {}
      void operator() (base_property* p) const
      {
        if(p != NULL)
          p->reserve(n_);
      }
      size_t n_;
    };

    struct clear_functor
    {
      clear_functor() {}
      void operator() (base_property* p) const
      {
        if(p != NULL)
          p->clear();
      }
    };

    struct swap_functor
    {
      swap_functor(size_t id0, size_t id1):id0_(id0), id1_(id1) {}
      void operator() (base_property* p) const
      {
        if(p != NULL)
          p->swap(id0_, id1_);
      }
      size_t id0_, id1_;
    };

    struct delete_functor
    {
      delete_functor() {}
      void operator() (base_property* p)
      {
        if(p != NULL)
          delete p;
        p = NULL;
      }
    };

  private:
    dim_property_type dim_property_;
  };

  template<class T>
  inline int simplex_property::get_specific_prop_index(const T&, const std::string &prop_name) const
  {
    int index = -1;
    for(size_t i = 0; i < dim_property_.size(); ++i)
      if(dim_property_[i] != NULL && dim_property_[i]->name() == prop_name
         && dynamic_cast<property<T>*>(dim_property_[i]) != NULL)
        {
          index = i;
          break;
        }
    return index;
  }

  template<class T>
  inline const property<T>& simplex_property::get_specific_prop(size_t prop_id) const
  {
    assert(prop_id >= 0 && prop_id < dim_property_.size() && dim_property_[prop_id] != NULL);
    property<T>* p = dynamic_cast<property<T>*>(dim_property_[prop_id]);
    assert(p != NULL);
    return *p;
  }

  template<class T>
  inline property<T>& simplex_property::get_specific_prop(size_t prop_id)
  {
    assert(prop_id >= 0 && prop_id < dim_property_.size() && dim_property_[prop_id] != NULL);
    property<T>* p = dynamic_cast<property<T>*>(dim_property_[prop_id]);
    assert(p != NULL);
    return *p;
  }

  template<class T>
  inline void simplex_property::add_property(const T&, const std::string& prop_name)
  {
    size_t index = 0;
    for(; index < dim_property_.size() && dim_property_[index] != NULL; ++index)
      if(dim_property_[index]->name() == prop_name)
        {
          std::cerr << "The property has been existed" << std::endl;
          return;
        }
    if(dim_property_.size() == index)
      dim_property_.push_back(NULL);
    dim_property_[index] = new property<T>(prop_name);
    assert(dim_property_[index] != NULL);
  }

}

#endif // SIMPLEX_PROPERTY_H
