#ifndef IS_STATUS_H
#define IS_STATUS_H

namespace is_mesh
{

  /// status bits defines the value of flag(such as the simplex is visited)
  enum status_bit
  {
    /// if the simplex is visited, the lowwest bit is 1, otherwise is 0
    VISITED = 1,

    /// if the simplex is deleted, the second lowwest bit is 1, otherwise is 0
    DELETED = 2,

    /// if the simplex is boundary, the second lowwest bit is 1, otherwise is 0
    BOUND = 4,

    /// this is used in the getting boundary
    BOUND_VISITED = 8,

    /// this is used in the getting co_boundary
    CO_BOUND_VISITED = 16,

    /// this is used in the getting adjacent
    ADJACENT_VISITED = 32,

    AUX_FLG = 64
  };

  /// status class
  /** This class is used to describe the status of a simplex, we use bits of a
    * positive integer to indicate the simplex's status(such as is visited and
    * is deleted). The class was composed of a unsigned int value whose initial
    * value is 0 to indicate we have no operations on this simplex.
    */
  class simplex_status
  {
  public:
    /// This member function creates a new instance of this class.
    /** In this member function we initial the status of the simplex and the
      * initial value is 0.
      */
    simplex_status():status_(0) {}

    /// This member funciton destroys an instance of the class
    virtual ~simplex_status() {}

    /// This member function returns the status of the current simplex
    /** \return the status of the current instance
      */
    inline unsigned int get_status() const
    {return status_;}

    /// This member function updates the status of the current simplex
    /** \param st the status the current instance to be updated
      */
    inline void set_status(unsigned int st)
    {status_ = st;}

    /// This member function checks if the current simplex is visited
    /** \return true if the current simplex is visited, otherwise false
      */
    inline bool is_visited() const
    {return is_set_flag(VISITED);}

    /// This member function is used to mark the current simplex to be visited
    inline void set_visited()
    {set_flag(VISITED);}

    /// This member function is used to mark the current simplex to be not visited
    inline void reset_visited()
    {reset_flag(VISITED);}

    /// This member function checks if the current simplex is deleted
    /** \return true if the current simplex is deleted, otherwise false
      */
    inline bool is_deleted() const
    {return is_set_flag(DELETED);}

    /// This member function is used to mark the current simplex to be deleted
    inline void set_deleted()
    {set_flag(DELETED);}

    /// This member function is used to mark the current simplex to be not deleted
    inline void reset_deleted()
    {reset_flag(DELETED);}

    /// This member function checks if the current simplex is boundary
    /** \return true if the current simplex is boundary, otherwise false
      */
    inline bool is_boundary() const
    {return is_set_flag(BOUND);}

    /// This member function is used to mark the current simplex is boundary
    inline void set_boundary()
    {set_flag(BOUND);}

    /// This member function is used to mark the current simplex is not boundary
    inline void reset_boundary()
    {reset_flag(BOUND);}

    /// This member function checks if the current simplex is bound_visited
    /** \return true if the current simplex is visited, otherwise false
      */
    inline bool is_bound_visited() const
    {return is_set_flag(BOUND_VISITED);}

    /// This member function is used to mark the current simplex to be bound_visited
    inline void set_bound_visited()
    {set_flag(BOUND_VISITED);}

    /// This member function is used to mark the current simplex to be not bound_visited
    inline void reset_bound_visited()
    {reset_flag(BOUND_VISITED);}

    /// This member function checks if the current simplex is co_bound_visited
    /** \return true if the current simplex is visited, otherwise false
      */
    inline bool is_co_bound_visited() const
    {return is_set_flag(CO_BOUND_VISITED);}

    /// This member function is used to mark the current simplex to be co_bound_visited
    inline void set_co_bound_visited()
    {set_flag(CO_BOUND_VISITED);}

    /// This member function is used to mark the current simplex to be not co_bound_visited
    inline void reset_co_bound_visited()
    {reset_flag(CO_BOUND_VISITED);}

    /// This member function checks if the current simplex is adjacent_visited
    /** \return true if the current simplex is visited, otherwise false
      */
    inline bool is_adjacent_visited() const
    {return is_set_flag(ADJACENT_VISITED);}

    /// This member function is used to mark the current simplex to be adjacent_visited
    inline void set_adjacent_visited()
    {set_flag(ADJACENT_VISITED);}

    /// This member function is used to mark the current simplex to be not adjacent_visited
    inline void reset_adjacent_visited()
    {reset_flag(ADJACENT_VISITED);}

  public:

    /// This member function checks if the current simplex is marked as the given flag
    /** \param flag the given flag
        \return true if the current simplex is marked as the flag, otherwise false
      */
    inline bool is_set_flag(unsigned int flag) const
    {return status_ & flag; }

    /// This member function is used to mark the current simplex to be the given flag
    /** \param flag the given flag
      */
    inline void set_flag(unsigned int flag)
    {status_ |= flag;}

    /// This member function is used not to mark the current simplex to be thegiven flag
    /** \param flag the given flag
      */
    inline void reset_flag(unsigned int flag)
    {status_ &= ~flag;}

  private:
    /// the integer used to desribe the status of a simplex
    unsigned int status_;
  };
}
#endif // STATUS_H
