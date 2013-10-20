#include "utils.h"

namespace is_mesh
{

  face2tet_adjacent *face2tet_adjacent::create(
      const matrixi &tet,
      const std::string & strategy)
  {
    std::auto_ptr<face2tet_adjacent> fa(new face2tet_adjacent);
    if(strategy == "geometry"){
      if(fa->init_geometry(tet))
        return 0;
    }else if(strategy == "topology"){
      if(fa->init_topology(tet))
        return 0;
    }else{
      std::cerr << "# [error] wrong face2tet_adjacent strategy: " << strategy << std::endl;
      return 0;
    }
    return fa.release();
  }

  int face2tet_adjacent::init_geometry(const matrixi &tet)
  {
    // undo here
    return 0;
  }

  size_t face2tet_adjacent::get_face_idx(size_t a, size_t b, size_t c) const
  {
    std::vector<size_t> tri(3);
    tri[0] = a; tri[1] = b; tri[2] = c;
    std::sort(tri.begin(), tri.end());
    boost::unordered_map<std::vector<size_t>,size_t>::const_iterator cit =
        face2idx_.find(tri);
    if(cit == face2idx_.end()) return -1;
    return cit->second;
  }

  size_t face2tet_adjacent::get_face_idx(const size_t *abc) const
  {
    return get_face_idx(abc[0], abc[1], abc[2]);
  }

  std::pair<size_t, size_t> face2tet_adjacent::query(size_t a, size_t b, size_t c) const
  {
    size_t idx = get_face_idx(a, b, c);
    if(idx >= faces_.size())
      return std::make_pair(-1, -1);
    return face2tet_[idx];
  }

  std::pair<size_t, size_t> face2tet_adjacent::query(const size_t *abc) const
  {
    return query(abc[0], abc[1], abc[2]);
  }


  int face2tet_adjacent::init_topology(const matrixi &tet)
  {
    strategy_ = "topology";
    typedef std::map<std::vector<size_t>, std::pair<size_t, size_t> > map_type;
    map_type adj_map;

    assert(tet.size(1) == 4);
    const size_t tet_n = tet.size(2);
    tet_num_ = tet_n;
    std::vector<size_t> tri(3), sort_tri(3);
    for(size_t tet_i = 0; tet_i < tet_n; ++tet_i) {
      for(size_t tri_i = 0; tri_i < 4; ++tri_i) {
        for(size_t ni = 0; ni < 3; ++ni)
          tri[ni] = tet((tri_i+ni)%4, tet_i);

        sort_tri = tri;
        std::sort(sort_tri.begin(), sort_tri.end());
        map_type::iterator it = adj_map.find(sort_tri);
        if(it == adj_map.end())
          adj_map.insert(std::make_pair(sort_tri,std::make_pair(tet_i,-1)));
        else{
          if(it->second.second != -1){
            std::cerr << "# [error] strange face " ;
            for(size_t pi = 0; pi < sort_tri.size(); ++pi)
              std::cerr << sort_tri[pi] << " ";
            std::cerr << std::endl;
            std::cerr << "# [error] this face associated with at least three tets "
                 << it->second.first << " " << it->second.second
                 << " " << tet_i << std::endl;
            return __LINE__;
          }
          it->second.second = tet_i;
        }
      }
    }
    const size_t face_num = adj_map.size();
    std::cerr << "# create face2tet_adjacent success with " << face_num << " entries." << std::endl;
    faces_.resize(face_num);
    face2tet_.resize(face_num);
    map_type::const_iterator itr = adj_map.begin();
    for(size_t i = 0; i < face_num; ++i, ++itr) {
      faces_[i] = itr->first;
      face2tet_[i] = itr->second;
    }
    return 0;
  }

  int get_outside_face(const face2tet_adjacent &m,
                       zjucad::matrix::matrix<size_t> &face)
  {
    std::vector<size_t> face0;
    for(size_t i = 0; i < m.faces_.size(); ++i) {
      const std::pair<size_t, size_t> &tet_i = m.face2tet_[i];
      if(!m.is_outside_face(tet_i)) continue;

      const std::vector<size_t> &tri = m.faces_[i];
      if(tet_i.first >= 0)
        face0.insert(face0.end(), tri.begin(), tri.end());
      else {
        face0.push_back(tri[0]);
        face0.push_back(tri[2]);
        face0.push_back(tri[1]);
      }
    }
    face.resize(3, face0.size()/3);
    std::copy(face0.begin(), face0.end(), face.begin());
    return 0;
  }

  int dfs_outside_face(const size_t index, std::vector<bool> &is_visited,
                       const matrixst &face,
                       const jtf::mesh::edge2cell_adjacent &edge_adj)
  {
    size_t face_index, edge_index;
    std::vector<size_t> edge2tri;
    if(!is_visited[index])
    {
      is_visited[index] = true;
      for(size_t i = 0; i < 3; ++i)
        for(size_t j = i+1; j < 3; ++j)
        {
          edge_index = edge_adj.get_edge_idx(face(i, index), face(j, index));
          edge2tri.clear();
          edge2tri.push_back(edge_adj.edge2cell_[edge_index].first);
          edge2tri.push_back(edge_adj.edge2cell_[edge_index].second);
          if(edge2tri.size() != 2)
            continue;
          if(edge2tri[0] == -1 || edge2tri[1] == -1)
            continue;
          face_index = edge2tri[0] + edge2tri[1] - index;
          dfs_outside_face(face_index, is_visited, face, edge_adj);
        }
    }
    return 0;
  }

  bool detect_topology(const matrixst& cells)
  {
    using namespace jtf::mesh;
    if(cells.size() == 0)
      {
        std::cerr << "there is no mesh input" << std::endl;
        return false;
      }
    matrixst face;
    if(cells.size(1) == 3)
      face = cells;
    else
      {
        std::auto_ptr<face2tet_adjacent> face_adj(face2tet_adjacent::create(cells));
        if(!face_adj.get())
          return false;
        get_outside_face(*face_adj, face);
      }
    std::auto_ptr<edge2cell_adjacent> edge_adj(edge2cell_adjacent::create(face, false));
    if(!edge_adj.get())
      return false;
    std::vector<bool> is_visited(face.size(2), false);
    dfs_outside_face(0, is_visited, face, *edge_adj);
    for(size_t i = 0; i < is_visited.size(); ++i)
      if(!is_visited[i])
      {
        std::cout << "exist a hole" << std::endl;
        return false;
      }
    return true;
  }
}
