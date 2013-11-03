#include "topology_kernel.h"

#include <queue>

namespace is_mesh
{
  void topology_kernel::set_dim(size_t top_dim)
  {
    for(size_t i = 0; i < top_dim_; ++i)
      {
        pm_.clear(i);
        sm_.clear(i);
        simplex2handle_[i].clear();
      }
    simplex2handle_.clear();

    top_dim_ = top_dim;
    pm_.set_dim(top_dim);
    sm_.set_dim(top_dim);
    if(top_dim > 0)
      simplex2handle_.resize(top_dim + 1);
    for(size_t i = 0; i < top_dim + 1; ++i)
      pm_.add_property(i, simplex_status(), "<status>");
    status_id_ = 0;
    pm_.add_property(0, coord_type(), "<coordinate>");
    coord_id_ = 1;
  }

  int topology_kernel::new_top_simplex(const std::vector<size_t>& verts, simplex_handle& sh)
  {
    assert(verts.size() == top_dim_ + 1);

    std::vector<size_t> sorted_verts(verts);
    std::sort(sorted_verts.begin(), sorted_verts.end());
    assert(sorted_verts.back() < sm_.n_element(0));
    for(size_t i = 0; i < sorted_verts.size() - 1; ++i)
      assert(sorted_verts[i] != sorted_verts[i + 1]);

//    std::cout << "new top simplex: ";
//    for(size_t i = 0; i < sorted_verts.size(); ++i)
//        std::cout << sorted_verts[i] << "  ";
//    std::cout << std::endl;

    if(sorted_verts.size() == 4)
      return new_tet(sorted_verts, sh);
    else if(sorted_verts.size() == 3)
      return new_tri(sorted_verts, sh);
    else
      {
        std::cerr << "the simplex was not supported" << std::endl;
        return __LINE__;
      }
  }

  int topology_kernel::new_top_simplex(const std::vector<simplex_handle>& verts, simplex_handle& sh)
  {
    std::vector<size_t> temp_vert(verts.size());
    for(size_t i = 0; i < temp_vert.size(); ++i)
      temp_vert[i] = verts[i].id();
    return new_top_simplex(temp_vert, sh);
  }


  int topology_kernel::del_simplex(const simplex_handle& sh)
  {
    assert(is_valid_handle(sh));
    assert(!is_simplex_deleted(sh));
    std::vector<simplex_handle> co_bounds;
    get_all_co_boundary_simplex(sh, co_bounds);
    for(size_t i = 0; i < co_bounds.size(); ++i)
      {
        assert(!is_simplex_deleted(co_bounds[i]));
        set_simplex_deleted(co_bounds[i]);
      }
    set_simplex_deleted(sh);
    return 0;
  }

  int topology_kernel::new_vert(size_t id, const coord_type& coord, simplex_handle& sh)
  {
    const simplex_dim cur_dim = 0;
    sm_.push_back(cur_dim, simplex());
    pm_.resize(cur_dim, sm_.n_element(cur_dim));
    sh.set_dim(cur_dim);
    sh.set_id(sm_.n_element(cur_dim) - 1);
    set_coord(sh, coord);
    return 0;
  }


  void topology_kernel::get_all_boundary_simplex(const simplex_handle& sh,
                                                 std::vector<simplex_handle>& bounds)
  {
    assert(is_valid_handle(sh));
    assert(!is_simplex_deleted(sh));
    bounds.clear();
    if(sh.dim() == 0)
      return;
    std::queue<simplex_handle> q;
    const std::vector<simplex_handle>& simplex_bound = sm_.get_specific_simplex(sh).get_boundary();
    for(size_t i = 0; i < simplex_bound.size(); ++i)
      {
        q.push(simplex_bound[i]);
      }
    while(!q.empty())
      {
        simplex_handle cur_sh = q.front();
        q.pop();
        if(!is_simplex_bound_visited(cur_sh))
          {
            bounds.push_back(cur_sh);
            set_simplex_bound_visited(cur_sh);
            if(cur_sh.dim() > 0)
              {
                const std::vector<simplex_handle>& cur_simplex_bound =
                    sm_.get_specific_simplex(cur_sh).get_boundary();
                for(size_t i = 0; i < cur_simplex_bound.size(); ++i)
                  if(!is_simplex_bound_visited(cur_simplex_bound[i]))
                    q.push(cur_simplex_bound[i]);
              }
          }
      }
    for(size_t i = 0; i < bounds.size(); ++i)
      reset_simplex_bound_visited(bounds[i]);
  }

  void topology_kernel::get_k_boundary_simplex(const simplex_handle& sh, size_t k,
                                               std::vector<simplex_handle>& bounds)
  {
    assert(is_valid_handle(sh));
    assert(!is_simplex_deleted(sh));
    assert(sh.dim() > k && k >= 0);
    bounds.clear();
    if(sh.dim() == 0)
      return;
    std::queue<simplex_handle> q;
    std::vector<simplex_handle> visited_simplex;
    const std::vector<simplex_handle>& simplex_bound = sm_.get_specific_simplex(sh).get_boundary();
    for(size_t i = 0; i < simplex_bound.size(); ++i)
      q.push(simplex_bound[i]);
    while(!q.empty())
      {
        simplex_handle cur_sh = q.front();
        q.pop();
        if(!is_simplex_bound_visited(cur_sh))
          {
            set_simplex_bound_visited(cur_sh);
            visited_simplex.push_back(cur_sh);
            if(cur_sh.dim() == k)
              bounds.push_back(cur_sh);
            else if(cur_sh.dim() > k)
              {
                const std::vector<simplex_handle>& cur_simplex_bound =
                    sm_.get_specific_simplex(cur_sh).get_boundary();
                for(size_t i = 0; i < cur_simplex_bound.size(); ++i)
                  if(!is_simplex_bound_visited(cur_simplex_bound[i]))
                    q.push(cur_simplex_bound[i]);
              }
          }
      }
    for(size_t i = 0; i < visited_simplex.size(); ++i)
      reset_simplex_bound_visited(visited_simplex[i]);
  }

  void topology_kernel::get_all_co_boundary_simplex(const simplex_handle& sh,
                                                    std::vector<simplex_handle>& co_bounds)
  {
    assert(is_valid_handle(sh));
    assert(!is_simplex_deleted(sh));
    co_bounds.clear();
    if(sh.dim() == top_dim_)
      return;
    std::queue<simplex_handle> q;
    std::vector<simplex_handle> visited_simplex;
    q.push(sh);
    assert(!is_simplex_co_bound_visited(sh));
    set_simplex_co_bound_visited(sh);
    visited_simplex.push_back(sh);
    while(!q.empty())
      {
        simplex_handle cur_sh = q.front();
        q.pop();
        if(cur_sh.dim() > sh.dim())
          {
            co_bounds.push_back(cur_sh);
          }
        const std::vector<simplex_handle>& cur_par_co_bound =
            sm_.get_specific_simplex(cur_sh).get_par_co_boundary();
        for(size_t i = 0; i < cur_par_co_bound.size(); ++i)
          {
            if(!is_simplex_co_bound_visited(cur_par_co_bound[i]))
              {
                q.push(cur_par_co_bound[i]);
                set_simplex_co_bound_visited(cur_par_co_bound[i]);
                visited_simplex.push_back(cur_par_co_bound[i]);
              }
          }
        if(cur_sh.dim() > sh.dim() + 1)
          {
            const std::vector<simplex_handle>& cur_simplex_bound =
                sm_.get_specific_simplex(cur_sh).get_boundary();
            for(size_t i = 0; i < cur_simplex_bound.size(); ++i)
              {
                if(!is_simplex_co_bound_visited(cur_simplex_bound[i]) &&
                   is_belong(sh, cur_simplex_bound[i]))
                  {
                    q.push(cur_simplex_bound[i]);
                    set_simplex_co_bound_visited(cur_simplex_bound[i]);
                    visited_simplex.push_back(cur_simplex_bound[i]);
                  }
              }
          }
      }
    for(size_t i = 0; i < visited_simplex.size(); ++i)
      reset_simplex_co_bound_visited(visited_simplex[i]);
  }

  void topology_kernel::get_k_co_boundary_simplex(const simplex_handle& sh, size_t k,
                                                  std::vector<simplex_handle>& co_bounds)
  {
    assert(is_valid_handle(sh));
    assert(!is_simplex_deleted(sh));
    assert(sh.dim() < k);
    co_bounds.clear();
    std::queue<simplex_handle> q;
    std::vector<simplex_handle> visited_simplex;
    q.push(sh);
    assert(!is_simplex_co_bound_visited(sh));
    set_simplex_co_bound_visited(sh);
    visited_simplex.push_back(sh);
    while(!q.empty())
      {
        simplex_handle cur_sh = q.front();
        assert(!is_simplex_deleted(cur_sh));
        q.pop();
        if(cur_sh.dim() > sh.dim())
          {
            if(cur_sh.dim() == k)
              co_bounds.push_back(cur_sh);
          }
        const std::vector<simplex_handle>& cur_par_co_bound =
            sm_.get_specific_simplex(cur_sh).get_par_co_boundary();
        for(size_t i = 0; i < cur_par_co_bound.size(); ++i)
          {
            assert(!is_simplex_deleted(cur_par_co_bound[i]));
            if(!is_simplex_co_bound_visited(cur_par_co_bound[i]))
              {
                q.push(cur_par_co_bound[i]);
                set_simplex_co_bound_visited(cur_par_co_bound[i]);
                visited_simplex.push_back(cur_par_co_bound[i]);
              }
          }
        if(cur_sh.dim() > sh.dim() + 1)
          {
            const std::vector<simplex_handle>& cur_simplex_bound =
                sm_.get_specific_simplex(cur_sh).get_boundary();
            for(size_t i = 0; i < cur_simplex_bound.size(); ++i)
              {
                assert(!is_simplex_deleted(cur_simplex_bound[i]));
                if(!is_simplex_co_bound_visited(cur_simplex_bound[i]) &&
                   is_belong(sh, cur_simplex_bound[i]))
                  {
                    q.push(cur_simplex_bound[i]);
                    set_simplex_co_bound_visited(cur_simplex_bound[i]);
                    visited_simplex.push_back(cur_simplex_bound[i]);
                  }
              }
          }
      }
    for(size_t i = 0; i < visited_simplex.size(); ++i)
      reset_simplex_co_bound_visited(visited_simplex[i]);
  }

  void topology_kernel::get_adjacent_simplex(const simplex_handle& sh,
                                             std::vector<simplex_handle>& adjacent)
  {

    if(!is_valid_handle(sh))
      {
        std::cerr << "it is not a valid handle" << std::endl;
        return;
      }
    assert(!is_simplex_deleted(sh));
    adjacent.clear();
    bool is_vertex = (sh.dim() == 0);
    std::queue<simplex_handle> q;
    std::vector<simplex_handle> visited_simplex;
    #if 0
    if(is_vertex)
      {
        std::vector<simplex_handle> co_bounds;
        get_k_co_boundary_simplex(sh, 1, co_bounds);
        for(size_t i = 0; i < co_bounds.size(); ++i)
          {
            const std::vector<simplex_handle>& bounds =
                sm_.get_specific_simplex(co_bounds[i]).get_boundary();
            for(size_t j = 0; j < bounds.size(); ++j)
              {
                if(bounds[j] != sh && !is_simplex_adjacent_visited(bounds[j]))
                  {
                    adjacent.push_back(bounds[j]);
                    set_simplex_adjacent_visited(bounds[j]);
                    visited_simplex.push_back(bounds[j]);
                  }
              }
          }
      }
    else
      {
        const std::vector<simplex_handle>& simplex_bounds =
            sm_.get_specific_simplex(sh).get_boundary();
        std::vector<simplex_handle> co_bounds;
        for(size_t i = 0; i < simplex_bounds.size(); ++i)
          {
            get_k_co_boundary_simplex(simplex_bounds[i], sh.dim(), co_bounds);
            for(size_t j = 0; j < co_bounds.size(); ++j)
              {
                if(co_bounds[j] != sh && !is_simplex_adjacent_visited(co_bounds[j]))
                  {
                    adjacent.push_back(co_bounds[j]);
                    set_simplex_adjacent_visited(co_bounds[j]);
                    visited_simplex.push_back(co_bounds[j]);
                  }
              }
          }
      }
    #else
    if(is_vertex)
      {
        q.push(sh);
        assert(!is_simplex_adjacent_visited(sh));
        set_simplex_adjacent_visited(sh);
        visited_simplex.push_back(sh);
      }
    const std::vector<simplex_handle>& simplex_bounds =
        sm_.get_specific_simplex(sh).get_boundary();
    for(size_t i = 0; i < simplex_bounds.size(); ++i)
      {
        q.push(simplex_bounds[i]);
        assert(!is_simplex_adjacent_visited(simplex_bounds[i]));
        set_simplex_adjacent_visited(simplex_bounds[i]);
        visited_simplex.push_back(simplex_bounds[i]);
      }
    while(!q.empty())
      {
        simplex_handle cur_sh = q.front();
        q.pop();
        if(cur_sh.dim() == sh.dim() && cur_sh.id() != sh.id())
          {
            adjacent.push_back(cur_sh);
          }
        const std::vector<simplex_handle>& cur_par_co_bound =
            sm_.get_specific_simplex(cur_sh).get_par_co_boundary();
        for(size_t i = 0; i < cur_par_co_bound.size(); ++i)
          {
            if(!is_simplex_adjacent_visited(cur_par_co_bound[i]))
              {
                q.push(cur_par_co_bound[i]);
                set_simplex_adjacent_visited(cur_par_co_bound[i]);
                visited_simplex.push_back(cur_par_co_bound[i]);
              }
          }
        if(cur_sh.dim() > sh.dim())
          {
            const std::vector<simplex_handle>& cur_simplex_bound =
                sm_.get_specific_simplex(cur_sh).get_boundary();
            for(size_t i = 0; i < cur_simplex_bound.size(); ++i)
              {
                bool vis_flg = is_simplex_adjacent_visited(cur_simplex_bound[i]);
                if(vis_flg)
                  continue;
                bool push_flg;
                if(is_vertex)
                  {
                    push_flg = is_belong(sh, cur_simplex_bound[i]);
                    if(cur_simplex_bound[i].dim() == sh.dim() &&
                       cur_simplex_bound[i].id() != sh.id())
                      {
                        adjacent.push_back(cur_simplex_bound[i]);
                        continue;
                      }
                  }
                else
                  push_flg = is_belong(simplex_bounds, cur_simplex_bound[i]);
                if(push_flg)
                  {
                    q.push(cur_simplex_bound[i]);
                    set_simplex_adjacent_visited(cur_simplex_bound[i]);
                    visited_simplex.push_back(cur_simplex_bound[i]);
                  }
              }
          }
      }
    #endif
    for(size_t i = 0; i < visited_simplex.size(); ++i)
      reset_simplex_adjacent_visited(visited_simplex[i]);
  }


  int topology_kernel::get_other_verts(const simplex_handle& sh, const std::vector<simplex_handle>& given_verts,
                                       std::vector<simplex_handle>& other_verts)
  {
    assert(is_valid_handle(sh));
    assert(sh.dim() > 0);
    std::vector<simplex_handle> verts;
    other_verts.clear();
    get_k_boundary_simplex(sh, 0, verts);
    for(size_t i = 0; i < given_verts.size(); ++i)
      {
        assert(!pm_.get_element_property<simplex_status>(given_verts[i], status_id_).is_set_flag(AUX_FLG));
        pm_.get_element_property<simplex_status>(given_verts[i], status_id_).set_flag(AUX_FLG);
      }
    for(size_t i = 0;  i < verts.size(); ++i)
      {
        if(!pm_.get_element_property<simplex_status>(verts[i], status_id_).is_set_flag(AUX_FLG))
          other_verts.push_back(verts[i]);
      }
    for(size_t i = 0; i < given_verts.size(); ++i)
      {
        assert(pm_.get_element_property<simplex_status>(given_verts[i], status_id_).is_set_flag(AUX_FLG));
        pm_.get_element_property<simplex_status>(given_verts[i], status_id_).reset_flag(AUX_FLG);
      }
    return 0;
  }

  int topology_kernel::get_vertex_opposite_simplex(const simplex_handle& v_sh, const simplex_handle& sh,
                                                   simplex_handle& oppo_sh)
  {
    assert(sh.dim() >= 2);
    oppo_sh = simplex_handle();
    const std::vector<simplex_handle>& bounds = sm_.get_specific_simplex(sh).get_boundary();
    for(size_t i = 0; i < bounds.size(); ++i)
      {
        bool flg = true;
        std::vector<simplex_handle> verts;
        get_k_boundary_simplex(bounds[i], 0, verts);
        for(size_t j = 0; j < verts.size(); ++j)
          if(verts[j] == v_sh)
            {
              flg = false;
              break;
            }
        if(flg)
          {
            oppo_sh = bounds[i];
            break;
          }
      }
    return 0;
  }

  int topology_kernel::garbage_collector()
  {
    simplex_handle cur_left_sh, cur_right_sh;
    for(size_t dim = 1; dim <= top_dim_; ++dim)
      {
        const size_t cur_dim = dim;
        size_t left = 0;
        size_t right = sm_.n_element(cur_dim) - 1;
        cur_left_sh.set_dim(cur_dim);
        cur_left_sh.set_id(left);
        cur_right_sh.set_dim(cur_dim);
        cur_right_sh.set_id(right);
        while(true)
          {
            while(!is_simplex_deleted(cur_left_sh) && (left < right))
              {
                ++left;
                cur_left_sh.set_id(left);
              }
            while(is_simplex_deleted(cur_right_sh) && (left < right))
              {
                --right;
                cur_right_sh.set_id(right);
              }
            if(left >= right)
              break;
            sm_.swap(cur_dim, left, right);
            pm_.swap(cur_dim, left, right);
          }
        sm_.resize(cur_dim, is_simplex_deleted(cur_left_sh) ? left : left + 1);
        pm_.resize(cur_dim, is_simplex_deleted(cur_left_sh) ? left : left + 1);
      }
    return 0;
  }

  int topology_kernel::new_tet(const std::vector<size_t>& verts, simplex_handle& sh)
  {
    assert(verts.size() == 4);
    const int face_num = 4;
    const int edge_num = 6;
    std::vector<simplex_handle> face_shs, edge_shs;
    std::vector<bool> is_new_face, is_new_edge;
    face_shs.reserve(face_num); is_new_face.reserve(face_num);
    edge_shs.reserve(edge_num); is_new_edge.reserve(edge_num);
    std::vector<size_t> face_verts(3), edge_verts(2);
    simplex_handle face_sh, edge_sh;
    bool is_new;

    new_simplex(verts, sh, is_new);
    assert(is_new);

    for(size_t i = 0; i < verts.size(); ++i)
      for(size_t j = i + 1; j < verts.size(); ++j)
        for(size_t k = j + 1; k < verts.size(); ++k)
          {
            face_verts[0] = verts[i];
            face_verts[1] = verts[j];
            face_verts[2] = verts[k];
            new_simplex(face_verts, face_sh, is_new);
            face_shs.push_back(face_sh);
            is_new_face.push_back(is_new);
          }

    for(size_t i = 0; i < verts.size(); ++i)
      for(size_t j = i + 1; j < verts.size(); ++j)
        {
          edge_verts[0] = verts[i];
          edge_verts[1] = verts[j];
          new_simplex(edge_verts, edge_sh, is_new);
          edge_shs.push_back(edge_sh);
          is_new_edge.push_back(is_new);
        }

    sm_.get_specific_simplex(sh).get_boundary() = face_shs;

    const int face2edge[] = {0, 1, 3,
                             0, 2, 4,
                             1, 2, 5,
                             3, 4, 5};
    for(size_t i = 0; i < face_shs.size(); ++i)
      {
        sm_.get_specific_simplex(face_shs[i]).get_par_co_boundary().push_back(sh);
        assert(sm_.get_specific_simplex(face_shs[i]).par_co_boundary_size() <= 2);
        if(is_new_face[i])
          {
            std::vector<simplex_handle>& face_bounds =
                sm_.get_specific_simplex(face_shs[i]).get_boundary();
            assert(face_bounds.size() == 0);
            for(size_t j = 0; j < 3; ++j)
              face_bounds.push_back(edge_shs[face2edge[3*i+j]]);
          }
      }

    const int edge2face[] = {0, 2, 1, 3, 3, 3};
    for(size_t i = 0; i < edge_shs.size(); ++i)
      {
        std::vector<simplex_handle>& co_bound =
            sm_.get_specific_simplex(edge_shs[i]).get_par_co_boundary();
        if(is_new_edge[i])
          {
            assert(co_bound.size() == 0);
            co_bound.push_back(face_shs[edge2face[i]]);
          }
        else
          {
            assert(co_bound.size() == 1);
            co_bound[0] = face_shs[edge2face[i]];
          }

      }

    const int vert2edge[] = {0, 3, 5, 2};
    for(size_t i = 0; i < verts.size(); ++i)
      {
        std::vector<simplex_handle>& co_bound =
            sm_.get_specific_simplex(simplex_handle(0, verts[i])).get_par_co_boundary();
        if(co_bound.size() == 0)
          co_bound.push_back(edge_shs[vert2edge[i]]);
        else
          co_bound[0] = edge_shs[vert2edge[i]];
      }
    return 0;
  }

  int topology_kernel::new_tri(const std::vector<size_t>& verts, simplex_handle& sh)
  {
    assert(verts.size() == 3);
    std::vector<simplex_handle> edge_shs;
    std::vector<size_t> edge_verts(2);
    simplex_handle edge_sh;
    bool is_new;

    new_simplex(verts, sh, is_new);
    assert(is_new);

    for(size_t i = 0; i < verts.size(); ++i)
      for(size_t j = i + 1; j < verts.size(); ++j)
        {
          edge_verts[0] = verts[i];
          edge_verts[1] = verts[j];
          new_simplex(edge_verts, edge_sh, is_new);
          edge_shs.push_back(edge_sh);
        }
    assert(edge_shs.size() == 3);
    sm_.get_specific_simplex(sh).get_boundary() = edge_shs;

    for(size_t i = 0; i < edge_shs.size(); ++i)
      {
        sm_.get_specific_simplex(edge_shs[i]).get_par_co_boundary().push_back(sh);
        assert(sm_.get_specific_simplex(edge_shs[i]).par_co_boundary_size() <= 2);
      }

    const int vert2edge[] = {0, 2, 1};
    for(size_t i = 0; i < verts.size(); ++i)
      {
        std::vector<simplex_handle>& co_bound =
            sm_.get_specific_simplex(simplex_handle(0, verts[i])).get_par_co_boundary();
        if(co_bound.size() == 0)
          co_bound.push_back(edge_shs[vert2edge[i]]);
        else
          co_bound[0] = edge_shs[vert2edge[i]];
      }
    return 0;
  }

  /// make sure the verts is sorted
  int topology_kernel::new_simplex(const std::vector<size_t>& verts, simplex_handle& sh, bool& is_new)
  {
    assert(verts.size() <= top_dim_ + 1  && verts.size() >= 2);
    const size_t cur_dim = verts.size() - 1;
    map_type::iterator it = simplex2handle_[cur_dim].find(verts);
    if(it == simplex2handle_[cur_dim].end())
      {
        simplex sim;
        sim.get_boundary().reserve(cur_dim + 1);
        if(cur_dim == 1)
          {
            sim.get_boundary().push_back(simplex_handle(0, verts[0]));
            sim.get_boundary().push_back(simplex_handle(0, verts[1]));
            assert(sim.boundary_size() == 2);
          }
        sm_.push_back(cur_dim, sim);
        pm_.resize(cur_dim, sm_.n_element(cur_dim));
        sh.set_dim(cur_dim);
        sh.set_id(sm_.n_element(cur_dim) - 1);
        is_new = true;
        simplex2handle_[cur_dim].insert(std::make_pair(verts, sh));
      }
    else
      {
        sh = it->second;
        is_new = false;
        if(is_simplex_deleted(sh))
        {
          reset_simplex_deleted(sh);
          std::cout << "reset deleted simplex (" << sh.dim()
                    << ", " << sh.id() << ")" << std::endl;
        }
      }
    return 0;
  }


  bool topology_kernel::is_belong(const simplex_handle& low_sh, const simplex_handle& high_sh)
  {
    bool flg = false;
    if(low_sh.dim() < high_sh.dim())
      {
        std::vector<simplex_handle> bounds;
        get_k_boundary_simplex(high_sh, low_sh.dim(), bounds);
        for(size_t i = 0; i < bounds.size(); ++i)
          {
            if(bounds[i] == low_sh)
              {
                flg = true;
                break;
              }
          }
      }
    return flg;
  }

  bool topology_kernel::is_belong(const std::vector<simplex_handle>& low_shs, const simplex_handle& high_sh)
  {
    bool flg = false;
    assert(low_shs.size() != 0);
    for(size_t i = 0; i < low_shs.size(); ++i)
      {
        assert(!pm_.get_element_property<simplex_status>(low_shs[i], status_id_).is_set_flag(AUX_FLG));
        pm_.get_element_property<simplex_status>(low_shs[i], status_id_).set_flag(AUX_FLG);
      }
    if(low_shs[0].dim() < high_sh.dim())
      {
        std::vector<simplex_handle> bounds;
        get_k_boundary_simplex(high_sh, low_shs[0].dim(), bounds);
        for(size_t i = 0; i < bounds.size(); ++i)
          if(pm_.get_element_property<simplex_status>(bounds[i], status_id_).is_set_flag(AUX_FLG))
            {
              flg = true;
              break;
            }
      }
    for(size_t i = 0; i < low_shs.size(); ++i)
      {
        assert(pm_.get_element_property<simplex_status>(low_shs[i], status_id_).is_set_flag(AUX_FLG));
        pm_.get_element_property<simplex_status>(low_shs[i], status_id_).reset_flag(AUX_FLG);
      }
    return flg;
  }
}
