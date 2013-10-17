#ifndef IS_BASE_PROPERTY_H
#define IS_BASE_PROPERTY_H
#include <string>
namespace is_mesh
{
  class base_property
  {
  public:
    base_property(const std::string& name = "<unknown>"): name_(name) {}

    virtual ~base_property() {}

    const std::string& name() const {return name_;}

    virtual void resize (size_t n) = 0;

    virtual void reserve(size_t n) = 0;

    virtual void clear() = 0;

    virtual void push_back() = 0;

    virtual void swap(size_t id0, size_t id1) = 0;

    virtual size_t n_elements() const = 0;

    virtual base_property* clone() const = 0;

  protected:
    std::string name_;
  };
}

#endif // BASE_PROPERTY_H
