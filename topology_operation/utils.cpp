#include "utils.h"

#include <stack>

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
      // add by shenxinxin
      face2idx_.insert(std::make_pair(faces_[i], i));
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

  int dfs_face2tet(const size_t index, std::vector<bool> &is_visited,
                   const matrixst &tet,
                   const face2tet_adjacent &face_adj)
  {
    size_t tet_idx, face_idx;
    std::stack<size_t> st;
    assert(!is_visited[index]);
    st.push(index);
    while(!st.empty())
      {
        const size_t cur = st.top();
        st.pop();
        is_visited[cur] = true;
        for(size_t i = 0; i < tet.size(1); ++i)
          for(size_t j = i + 1; j < tet.size(1); ++j)
            for(size_t k = j + 1; k < tet.size(1); ++k)
              {
                face_idx = face_adj.get_face_idx(tet(i, cur), tet(j, cur),
                                                 tet(k, cur));
                const std::pair<size_t, size_t>& face2tet =
                    face_adj.face2tet_[face_idx];
                assert(face2tet.first == cur || face2tet.second == cur);
                if(face2tet.first == -1 || face2tet.second == -1)
                  continue;
                tet_idx = (face2tet.first == cur) ? face2tet.second :
                                                    face2tet.first;
                if(!is_visited[tet_idx])
                  st.push(tet_idx);
              }
      }
  }

  int dfs_outside_face(const size_t index, std::vector<bool> &is_visited,
                       const matrixst &face,
                       const jtf::mesh::edge2cell_adjacent &edge_adj)
  {
    size_t face_index, edge_index;
    std::stack<size_t> st;
    assert(!is_visited[index]);
    st.push(index);
    while(!st.empty())
      {
        const size_t cur = st.top();
        st.pop();
        is_visited[cur] = true;
        for(size_t i = 0; i < face.size(1); ++i)
          for(size_t j = i+1; j < face.size(1); ++j)
            {
              edge_index = edge_adj.get_edge_idx(face(i, cur), face(j, cur));
              const std::pair<size_t, size_t>& edge2tri =
                  edge_adj.edge2cell_[edge_index];
              assert(edge2tri.first == cur || edge2tri.second == cur);
              if(edge2tri.first == -1 || edge2tri.second == -1)
                continue;
              face_index = (edge2tri.first == cur) ? edge2tri.second :
                                                     edge2tri.first;
              if(!is_visited[face_index])
                st.push(face_index);
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

        int eular_num = get_euler_num(*face_adj);
        if(eular_num != 1)
          {
            std:: cout << "[info] eular num is " << eular_num << std::endl;
            return false;
          }

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
        std::cout << "the face exists a hole" << std::endl;
        return false;
      }
    return true;
  }

  int get_euler_num(const face2tet_adjacent &face_adj)
  {
    std::set<std::pair<size_t, size_t> > edges;
    std::set<size_t> verts;
    const size_t face_num = face_adj.faces_.size();
    const size_t tet_num = face_adj.tet_num_;
    for(size_t i = 0; i < face_num; ++i)
      {
        const std::vector<size_t> &face = face_adj.faces_[i];
        for(size_t j = 0; j < face.size(); ++j)
          {
            verts.insert(face[j]);
            for(size_t k = j + 1; k < face.size(); ++k)
              {
                edges.insert(std::make_pair(face[j], face[k]));
              }
          }
      }
    return verts.size() + face_num - edges.size() - tet_num;
  }

  int get_euler_num(const zjucad::matrix::matrix<size_t> &tet)
  {
    std::set<std::vector<size_t> > tet_set;
    std::set<std::vector<size_t> > face_set;
    std::set<std::pair<size_t, size_t> > edge_set;
    std::set<size_t> vert_set;
    for(size_t i = 0; i < tet.size(2); ++i)
      {
        std::vector<size_t> tet_vec(4);
        for(size_t j = 0; j < tet.size(1); ++j)
          {
            tet_vec[j] = tet(j, i);
            vert_set.insert(tet(j, i));
          }
        std::sort(tet_vec.begin(), tet_vec.end());
        tet_set.insert(tet_vec);

        for(size_t j = 0; j < tet_vec.size(); ++j)
          for(size_t k = j + 1; k < tet_vec.size(); ++k)
            {
              std::pair<size_t, size_t> edge(std::make_pair(tet_vec[j], tet_vec[k]));
              edge_set.insert(edge);
              for(size_t l = k + 1; l < tet_vec.size(); ++l)
                {
                  std::vector<size_t> face_vec(3);
                  face_vec[0] = tet_vec[j];
                  face_vec[1] = tet_vec[k];
                  face_vec[2] = tet_vec[l];
                  face_set.insert(face_vec);
                }
            }
      }
    if(tet_set.size() != tet.size(2))
      {
        std::cout << "input tet size: " << tet.size(2)
                  << " ****** tet set size: " << tet_set.size() << std::endl;
        return -1;
      }

    return (vert_set.size() + face_set.size() - edge_set.size() - tet_set.size());
  }
}


