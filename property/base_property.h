#ifndef IS_BASE_PROPERTY_H
#define IS_BASE_PROPERTY_H
#include <string>
namespace is_mesh
{
  /**
    * This class is a virtual class of the base property, it has a member of name.
    */

  class base_property
  {
  public:

    /** This member function creates a new instance of this class.
      * \param name the name of the property, the default value is "<unknown>"
      */
    base_property(const std::string& name = "<unknown>"): name_(name) {}

    /// This member function destroys an instance of class
    virtual ~base_property() {}

    /** This function returns the name of the property
      * \return the name of the property
      */
    const std::string& name() const {return name_;}

    /** This function resizes the size of the property
      * \param n the size we want to resize
      */
    virtual void resize (size_t n) = 0;

    /** This function reserves the size of the property
      * \param n the size we want to reserve
      */
    virtual void reserve(size_t n) = 0;

    /// This function clear the property
    virtual void clear() = 0;

    /// This function push back a default property
    virtual void push_back() = 0;

    /** This function swap two properties
      * \param id0 one of the property
      * \param id1 another property
      */
    virtual void swap(size_t id0, size_t id1) = 0;

    /** This function return the number of the element of the current property
      * \return the number of the element of the vurrent property
      */
    virtual size_t n_elements() const = 0;

    /** This function clone the current property
      * \return a pointer to the clone property
      */
    virtual base_property* clone() const = 0;

  protected:
    /// the name of the property
    std::string name_;
  };
}

#endif // BASE_PROPERTY_H
