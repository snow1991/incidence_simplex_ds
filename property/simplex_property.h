#ifndef IS_SIMPLEX_PROPERTY_H
#define IS_SIMPLEX_PROPERTY_H

#include "property.h"

namespace is_mesh
{
  class simplex_property
  {
  public:
    typedef std::vector<base_property*> dim_property_type;

    simplex_property() {}

    simplex_property(const simplex_property& rhs);

    simplex_property& operator= (const simplex_property& rhs);

    virtual ~simplex_property();

    const dim_property_type& get_dim_property() const
    {return dim_property_;}

    dim_property_type& get_dim_property()
    {return dim_property_;}

    void resize(size_t n);

    void reserve(size_t n);

    void clear();

    void swap(size_t id0, size_t id1);

    size_t size() const {return dim_property_.size();}

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

    template<class T>
    int get_specific_prop_index(const T&, const std::string& prop_name) const;

    template<class T>
    const property<T>& get_specific_prop(size_t prop_id) const;

    template<class T>
    property<T>& get_specific_prop(size_t prop_id);

    template <class T>
    void push_back(size_t prop_id)
    {

      property<T>& props = get_specific_prop<T>(prop_id);
      props.push_back();
    }

    template<class T>
    void add_property(const T& prop, const std::string& prop_name="<unknown>");

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
