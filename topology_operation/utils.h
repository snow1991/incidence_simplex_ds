#ifndef IS_UTILS_H
#define IS_UTILS_H

#include "../common/common.h"
#include <boost/unordered_map.hpp>

#include <jtflib/mesh/mesh.h>
namespace is_mesh
{

  class face2tet_adjacent
  {
  public:
    typedef zjucad::matrix::matrix<size_t> matrixi;
    static face2tet_adjacent *create(const matrixi &tet,
                                     const std::string & strategy = "topology");

    std::string get_strategy() const {return strategy_;}
    size_t get_face_idx(size_t a, size_t b, size_t c) const;
    size_t get_face_idx(const size_t *abc) const;

    std::pair<size_t, size_t> query(size_t a, size_t b, size_t c) const;
    std::pair<size_t, size_t> query(const size_t *abc) const;
    static inline bool is_outside_face(const std::pair<size_t, size_t> &nb_tet_id) {
      return (nb_tet_id.first  == -1) ^ (nb_tet_id.second == -1);
    }

    matrixi get_face(size_t id) const {
      matrixi rtn(3);
      copy(faces_[id].begin(), faces_[id].end(), rtn.begin());
      return rtn;
    }

    // invalid index is -1
    // vector of length 3 vector
    std::vector<std::vector<size_t> > faces_;
    std::vector<std::pair<size_t, size_t> > face2tet_;
    size_t tet_num_;
  private:
    boost::unordered_map<std::vector<size_t>,size_t> face2idx_;

    face2tet_adjacent(){}
    int init_geometry(const matrixi &tet);
    int init_topology(const matrixi &tet);
    std::string strategy_;
    //! @brief node index as key in a face is order increased.  The
    //! normal of this key face point to the 4th node of first,
    //! i.e. [face key, 4th node of first tet] has positive volume.
  };

  int get_outside_face(const face2tet_adjacent &m,
                       zjucad::matrix::matrix<size_t> &face);

  int dfs_outside_face(const size_t index, std::vector<bool> &is_visited,
                       const matrixst &face,
                       const jtf::mesh::edge2cell_adjacent &edge_adj);

  bool detect_topology(const matrixst& cells);

}

#endif // UTILS_H
