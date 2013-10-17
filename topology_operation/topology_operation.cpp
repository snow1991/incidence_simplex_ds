#include "topology_operation.h"
#include "utils.h"

namespace is_mesh
{
  int topology_operation::insert_vertex(const simplex_handle& sh, const matrixd& coord)
  {
    int flg;
    assert(cur_mesh_.is_valid_handle(sh));
    assert(sh.dim() != 0);
    assert(!cur_mesh_.is_simplex_deleted(sh));
    simplex_handle new_vert_sh;
    const size_t vert_num = cur_mesh_.get_simplex_manager().n_element(0);
    cur_mesh_.new_vert(vert_num, coord, new_vert_sh);

    std::vector<simplex_handle> vert_shs;
    std::vector<std::vector<simplex_handle> > other_verts;
    del_top_simplex(sh, vert_shs, other_verts);

    if(sh.dim() == cur_mesh_.top_dim())
      flg = insert_top_simplex(new_vert_sh, vert_shs, other_verts);
    else if(sh.dim() == cur_mesh_.top_dim() - 1)
      flg = insert_snd_top_simplex(new_vert_sh, vert_shs, other_verts);
    else
      flg = insert_edge_in_tet(new_vert_sh, vert_shs, other_verts);
    return flg;
  }

  int topology_operation::collapse_edge(const simplex_handle &sh, const matrixd &coord)
  {
    assert(cur_mesh_.top_dim() == 2);
    assert(cur_mesh_.is_valid_handle(sh));
    assert(sh.dim() == 1);
    assert(!cur_mesh_.is_simplex_deleted(sh));
    if(!is_edge_collapse_ok(sh))
      {
        std::cerr << "the edge can not be collapsed" << std::endl;
        return 1;
      }
    const std::vector<simplex_handle> edge_verts =
        cur_mesh_.get_simplex_manager().get_specific_simplex(sh).get_boundary();
    cur_mesh_.set_coord(edge_verts[1], coord);

    const std::vector<simplex_handle> edge_tops =
        cur_mesh_.get_simplex_manager().get_specific_simplex(sh).get_par_co_boundary();
    std::vector<simplex_handle> del_edges;
    for(size_t i = 0; i < edge_tops.size(); ++i)
      {
        const std::vector<simplex_handle>& bou =
            cur_mesh_.get_simplex_manager().get_specific_simplex(edge_tops[i]).get_boundary();
        for(size_t j = 0; j < bou.size(); ++j)
          {
            const std::vector<simplex_handle>& verts =
                cur_mesh_.get_simplex_manager().get_specific_simplex(bou[j]).get_boundary();
            if(verts[0] != edge_verts[0] && verts[1] != edge_verts[0])
              {
                const std::vector<simplex_handle>& par =
                    cur_mesh_.get_simplex_manager().get_specific_simplex(bou[j]).get_par_co_boundary();
                if(par.size() == 1)
                  del_edges.push_back(bou[j]);
                break;
              }
          }
      }


    std::vector<simplex_handle> vert_shs;
    std::vector<std::vector<simplex_handle> > other_verts;
    del_top_simplex(edge_verts[0], vert_shs, other_verts);
    for(size_t i = 0; i < del_edges.size(); ++i)
      {
        assert(!cur_mesh_.is_simplex_deleted(del_edges[i]));
        cur_mesh_.set_simplex_deleted(del_edges[i]);
      }

    std::vector<simplex_handle> new_top(cur_mesh_.top_dim() + 1);
    simplex_handle new_top_sh;

    new_top[0] = edge_verts[1];
    for(size_t i = 0; i < other_verts.size(); ++i)
      {
        if(is_in(other_verts[i], edge_verts[1]))
          continue;
        for(size_t j = 0; j < other_verts[i].size(); ++j)
          new_top[j + 1] = other_verts[i][j];
        cur_mesh_.new_top_simplex(new_top, new_top_sh);
      }
    return 0;
  }

  int topology_operation::flip_edge(const simplex_handle& sh, std::vector<simplex_handle>& new_edge)
  {
    if(!is_edge_flip_ok(sh))
      {
        std::cerr << "the edge can not be flipped" << std::endl;
        return 1;
      }
    std::vector<simplex_handle> vert_shs;
    std::vector<std::vector<simplex_handle> > other_verts;
    del_top_simplex(sh, vert_shs, other_verts);

    std::vector<simplex_handle> new_top(cur_mesh_.top_dim() + 1);
    simplex_handle new_top_sh;
    new_top[0] = other_verts[0][0];
    new_top[1] = other_verts[1][0];
    new_edge.clear();
    new_edge.push_back(new_top[0]);
    new_edge.push_back(new_top[1]);

    for(size_t i = 0; i < vert_shs.size(); ++i)
      {
        new_top[2] = vert_shs[i];
        cur_mesh_.new_top_simplex(new_top, new_top_sh);
      }
    return 0;
  }

  int topology_operation:: insert_top_simplex(const simplex_handle& v_sh,
                                              const std::vector<simplex_handle>& vert_shs,
                                              const std::vector<std::vector<simplex_handle> >& other_verts)
  {
    std::vector<simplex_handle> new_top(vert_shs);
    simplex_handle new_top_sh;
    for(size_t i = 0; i < vert_shs.size(); ++i)
      {
        if(i > 0)
            new_top[i - 1] = vert_shs[i - 1];
        new_top[i] = v_sh;
        cur_mesh_.new_top_simplex(new_top, new_top_sh);
      }
    return 0;
  }

  int topology_operation::insert_snd_top_simplex(const simplex_handle& v_sh,
                                                 const std::vector<simplex_handle>& vert_shs,
                                                 const std::vector<std::vector<simplex_handle> >& other_verts)
  {
    std::vector<simplex_handle> new_top(cur_mesh_.top_dim() + 1);
    simplex_handle new_top_sh;
    if(cur_mesh_.top_dim() == 2)
      {
        new_top[0] = v_sh;
        for(size_t i = 0; i < other_verts.size(); ++i)
          {
            new_top[1] = other_verts[i][0];
            for(size_t j = 0; j < vert_shs.size(); ++j)
              {
                new_top[2] = vert_shs[j];
                cur_mesh_.new_top_simplex(new_top, new_top_sh);
              }
          }
      }
    else if(cur_mesh_.top_dim() == 3)
      {
        new_top[0] = v_sh;
        for(size_t i = 0; i < other_verts.size(); ++i)
          {
            new_top[1] = other_verts[i][0];
            for(size_t j = 0; j < vert_shs.size(); ++j)
              for(size_t k = j + 1;  k < vert_shs.size(); ++k)
              {
                new_top[2] = vert_shs[j];
                new_top[3] = vert_shs[k];
                cur_mesh_.new_top_simplex(new_top, new_top_sh);
              }
          }
      }
    return 0;
  }

  int topology_operation::insert_edge_in_tet(const simplex_handle& v_sh,
                                             const std::vector<simplex_handle>& vert_shs,
                                             const std::vector<std::vector<simplex_handle> >& other_verts)
  {
    std::vector<simplex_handle> new_top(cur_mesh_.top_dim() + 1);
    simplex_handle new_top_sh;
    new_top[0] = v_sh;
    for(size_t i = 0; i < other_verts.size(); ++i)
      {
        new_top[1] = other_verts[i][0];
        new_top[2] = other_verts[i][1];
        for(size_t j = 0; j < vert_shs.size(); ++j)
          {
            new_top[3] = vert_shs[j];
            cur_mesh_.new_top_simplex(new_top, new_top_sh);
          }
      }
    return 0;
  }

  int topology_operation::del_top_simplex(const simplex_handle& sh,
                                          std::vector<simplex_handle>& vert_shs,
                                          std::vector<std::vector<simplex_handle> >& other_verts)
  {
    vert_shs.clear();
    other_verts.clear();
    if(sh.dim() > 0)
      cur_mesh_.get_k_boundary_simplex(sh, 0, vert_shs);
    else
      vert_shs.push_back(sh);
    assert(vert_shs.size() == sh.dim() + 1);
    std::vector<simplex_handle> adj_top;
    if(sh.dim() < cur_mesh_.top_dim())
      cur_mesh_.get_k_co_boundary_simplex(sh, cur_mesh_.top_dim(), adj_top);
    else
      adj_top.push_back(sh);
    other_verts.reserve(adj_top.size());

    std::vector<simplex_handle> del_simplex;
    cur_mesh_.get_all_co_boundary_simplex(sh, del_simplex);
    del_simplex.push_back(sh);
    for(size_t i = 0; i < adj_top.size(); ++i)
      {
        std::vector<simplex_handle> other;
        cur_mesh_.get_other_verts(adj_top[i], vert_shs, other);
        assert(other.size() == cur_mesh_.top_dim() - sh.dim());
        other_verts.push_back(other);
        const std::vector<simplex_handle>& bounds =
            cur_mesh_.get_simplex_manager().get_specific_simplex(adj_top[i]).get_boundary();
        assert(bounds.size() == cur_mesh_.top_dim() + 1);
        for(size_t j = 0; j < bounds.size(); ++j)
          {
            std::vector<simplex_handle>& co_bound =
                cur_mesh_.get_simplex_manager().get_specific_simplex(bounds[j]).get_par_co_boundary();
            assert(co_bound.size() <= 2);
            std::vector<simplex_handle>::iterator it = co_bound.begin();
            for(; it != co_bound.end(); ++it)
              if(*it == adj_top[i])
                break;
            assert(it != co_bound.end());
            co_bound.erase(it);
          }
      }
    assert(other_verts.size() == adj_top.size());

    for(size_t i = 0; i < del_simplex.size(); ++i)
      {
        assert(!cur_mesh_.is_simplex_deleted(del_simplex[i]));
        cur_mesh_.set_simplex_deleted(del_simplex[i]);
      }
//    cur_mesh_.del_simplex(sh);
    return 0;
  }

  bool topology_operation::is_edge_collapse_ok(const simplex_handle &sh)
  {
    const std::vector<simplex_handle>& edge_verts =
        cur_mesh_.get_simplex_manager().get_specific_simplex(sh).get_boundary();
    std::vector<simplex_handle> vert1_adj_top, vert2_adj_top, edge_adj_top;
    cur_mesh_.get_k_co_boundary_simplex(edge_verts[0], cur_mesh_.top_dim(), vert1_adj_top);
    cur_mesh_.get_k_co_boundary_simplex(edge_verts[1], cur_mesh_.top_dim(), vert2_adj_top);
    cur_mesh_.get_k_co_boundary_simplex(sh, cur_mesh_.top_dim(), edge_adj_top);

    std::vector<simplex_handle> only_vert_top;
    for(size_t i = 0;  i < edge_adj_top.size(); ++i)
      {
        assert(!cur_mesh_.is_simplex_visited(edge_adj_top[i]));
        cur_mesh_.set_simplex_visited(edge_adj_top[i]);
      }
    for(size_t i = 0;  i < vert1_adj_top.size(); ++i)
      if(!cur_mesh_.is_simplex_visited(vert1_adj_top[i]))
        only_vert_top.push_back(vert1_adj_top[i]);
    for(size_t i = 0;  i < vert2_adj_top.size(); ++i)
      if(!cur_mesh_.is_simplex_visited(vert2_adj_top[i]))
        only_vert_top.push_back(vert2_adj_top[i]);
    for(size_t i = 0;  i < edge_adj_top.size(); ++i)
      {
        assert(cur_mesh_.is_simplex_visited(edge_adj_top[i]));
        cur_mesh_.reset_simplex_visited(edge_adj_top[i]);
      }

    matrixst new_cells(cur_mesh_.top_dim() + 1, only_vert_top.size());
    int t;
    for(size_t i = 0; i < only_vert_top.size(); ++i)
      {
        std::vector<simplex_handle> bounds;
        cur_mesh_.get_k_boundary_simplex(only_vert_top[i], 0, bounds);
        for(size_t j = 0; j < bounds.size(); ++j)
          {
            if(bounds[j].id() == edge_verts[0].id())
              t = edge_verts[1].id();
            else
              t = bounds[j].id();
            new_cells(j, i) = t;
          }
      }
    bool flg = detect_topology(new_cells);
    return flg;
  }

  bool topology_operation::is_edge_flip_ok(const simplex_handle& sh)
  {
    assert(cur_mesh_.top_dim() == 2);
    assert(cur_mesh_.is_valid_handle(sh));
    assert(sh.dim() == 1);
    assert(!cur_mesh_.is_simplex_deleted(sh));
    const std::vector<simplex_handle>& top =
        cur_mesh_.get_simplex_manager().get_specific_simplex(sh).get_par_co_boundary();
    assert(top.size() > 0 && top.size() < 3);
    if(top.size() != 2)
      {
        std::cerr << "it is a boundary edge" << std::endl;
        return false;
      }
    const std::vector<simplex_handle>& edge_vert =
        cur_mesh_.get_simplex_manager().get_specific_simplex(sh).get_boundary();
    std::vector<simplex_handle> other_verts;
    std::vector<size_t> others(2);

    for(size_t i = 0; i < top.size(); ++i)
      {
        cur_mesh_.get_other_verts(top[i], edge_vert, other_verts);
        assert(other_verts.size() == 1);
        others[i] = other_verts[0].id();
      }
    if(others[0] == others[1])
      return false;
    else if(others[0] > others[1])
      std::swap(others[0], others[1]);
    const mesh::simplex2handle_type& simplex2handle =
        cur_mesh_.get_simplex2handle();
    if(simplex2handle[sh.dim()].find(others) !=
       simplex2handle[sh.dim()].end())
      return false;
//    std::vector<simplex_handle> adj_verts;
//    cur_mesh_.get_adjacent_simplex(simplex_handle(0, others[0]), adj_verts);
//    for(size_t j = 0; j < adj_verts.size(); ++j)
//      if(adj_verts[j].id() == others[1])
//        return false;

    return true;
  }
}
