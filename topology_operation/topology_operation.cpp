#include "topology_operation.h"
#include "utils.h"
#include "../io/io.h"

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
    assert(cur_mesh_.is_valid_handle(sh));
    assert(sh.dim() == 1);
    assert(!cur_mesh_.is_simplex_deleted(sh));
    if(!is_edge_collapse_ok(sh))
      {
        std::cerr << "the edge can not be collapsed" << std::endl;
        return 1;
      }
    // remove all the top simplex with edge_verts[0] and rebuild with edge_verts[1]
    const std::vector<simplex_handle> edge_verts =
        cur_mesh_.get_simplex_manager().get_specific_simplex(sh).get_boundary();
    cur_mesh_.set_coord(edge_verts[1], coord);

    // deal with some case
    std::vector<simplex_handle> del_simplexes;
    if(cur_mesh_.top_dim() == 2)
      deal_collapse_edge_case_tri_mesh(sh, edge_verts, del_simplexes);
    else if(cur_mesh_.top_dim() == 3)
      deal_collapse_edge_case_tet_mesh(sh, edge_verts, del_simplexes);

    // delet the simplex adjacent with edge_verts[0]
    std::vector<simplex_handle> vert_shs;
    std::vector<std::vector<simplex_handle> > other_verts;
    del_top_simplex(edge_verts[0], vert_shs, other_verts);
    for(size_t i = 0; i < del_simplexes.size(); ++i)
      {
        assert(!cur_mesh_.is_simplex_deleted(del_simplexes[i]));
        cur_mesh_.set_simplex_deleted(del_simplexes[i]);
      }

    // rebuild
    std::vector<simplex_handle> new_top(cur_mesh_.top_dim() + 1);
    simplex_handle new_top_sh;
    new_top[0] = edge_verts[1];
    std::vector<size_t> new_top_ids(new_top.size());
    for(size_t i = 0; i < other_verts.size(); ++i)
      {
        if(is_in(other_verts[i], edge_verts[1]))
          continue;
        for(size_t j = 0; j < other_verts[i].size(); ++j)
          new_top[j + 1] = other_verts[i][j];

        for(size_t j = 0; j < new_top.size(); ++j)
          new_top_ids[j] = new_top[j].id();
        std::sort(new_top_ids.begin(), new_top_ids.end());
        new_top_sh = cur_mesh_.get_handle(new_top_ids);

        if(new_top_sh.is_null())
          cur_mesh_.new_top_simplex(new_top, new_top_sh);
        else
          {
            std::cout << "exist top simplex: (" << new_top_sh.dim() << ", "
                      << new_top_sh.id()  << ")" << std::endl;
            simplex_handle snd_top_sh;
            cur_mesh_.get_vertex_opposite_simplex(edge_verts[1], new_top_sh, snd_top_sh);
            // update the relation of the simplex
            update_simplex_info(snd_top_sh);
          }
      }
    return 0;
  }

  int topology_operation::flip_edge(const simplex_handle& sh)
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

    for(size_t i = 0; i < vert_shs.size(); ++i)
      {
        new_top[2] = vert_shs[i];
        cur_mesh_.new_top_simplex(new_top, new_top_sh);
      }
    return 0;
  }

  int topology_operation::insert_top_simplex(const simplex_handle& v_sh,
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

//    std::cout << "delete simplex: ";
    for(size_t i = 0; i < del_simplex.size(); ++i)
      {
        assert(!cur_mesh_.is_simplex_deleted(del_simplex[i]));
        cur_mesh_.set_simplex_deleted(del_simplex[i]);
//        std::cout << " (" << del_simplex[i].dim() << ", "
//                  << del_simplex[i].id() << ")   ";
      }
//    std::cout << std::endl;
    return 0;
  }

  bool topology_operation::is_edge_collapse_ok(const simplex_handle &sh)
  {
#if 0
    const std::vector<simplex_handle>& edge_verts =
        cur_mesh_.get_simplex_manager().get_specific_simplex(sh).get_boundary();
#else
    std::vector<simplex_handle>& edge_verts =
        cur_mesh_.get_simplex_manager().get_specific_simplex(sh).get_boundary();
#endif
    std::vector<simplex_handle> vert1_adj_top, vert2_adj_top, edge_adj_top;
    cur_mesh_.get_k_co_boundary_simplex(edge_verts[0], cur_mesh_.top_dim(), vert1_adj_top);
    cur_mesh_.get_k_co_boundary_simplex(edge_verts[1], cur_mesh_.top_dim(), vert2_adj_top);
    cur_mesh_.get_k_co_boundary_simplex(sh, cur_mesh_.top_dim(), edge_adj_top);

    if(vert1_adj_top.size() > vert2_adj_top.size())
      std::swap(edge_verts[0], edge_verts[1]);

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

    // get all the top simplexs around the edge
    matrixst all_tet(4, only_vert_top.size() + edge_adj_top.size());
    int tet_cnt = 0;
    for(size_t i = 0; i < only_vert_top.size(); ++i)
      {
        std::vector<simplex_handle> bounds;
        cur_mesh_.get_k_boundary_simplex(only_vert_top[i], 0, bounds);
        for(size_t j = 0; j < bounds.size(); ++j)
            all_tet(j, tet_cnt) = bounds[j].id();
        ++tet_cnt;
      }
    for(size_t i = 0; i < edge_adj_top.size(); ++i)
      {
        std::vector<simplex_handle> bounds;
        cur_mesh_.get_k_boundary_simplex(edge_adj_top[i], 0, bounds);
        for(size_t j = 0; j < bounds.size(); ++j)
            all_tet(j, tet_cnt) = bounds[j].id();
        ++tet_cnt;
      }

    // get new top simplexs, maybe exist the same, so we use set
    std::set<std::vector<size_t> > new_cell_set;
    int t;
    for(size_t i = 0; i < only_vert_top.size(); ++i)
      {
        std::vector<simplex_handle> bounds;
        cur_mesh_.get_k_boundary_simplex(only_vert_top[i], 0, bounds);
        std::vector<size_t> temp_cell(bounds.size());
        for(size_t j = 0; j < bounds.size(); ++j)
          {
            if(bounds[j].id() == edge_verts[0].id())
              t = edge_verts[1].id();
            else
              t = bounds[j].id();
            temp_cell[j] = t;
          }
        std::sort(temp_cell.begin(), temp_cell.end());
        if(new_cell_set.find(temp_cell) == new_cell_set.end())
          new_cell_set.insert(temp_cell);
      }
    matrixst new_cells(cur_mesh_.top_dim() + 1, new_cell_set.size());
    size_t cur_cell_id = 0;
    for(std::set<std::vector<size_t> >::const_iterator c_it = new_cell_set.begin();
        c_it != new_cell_set.end(); ++c_it)
      {
        const std::vector<size_t>& cur_cell = *c_it;
        for(size_t j = 0; j < cur_cell.size(); ++j)
          new_cells(j, cur_cell_id) = cur_cell[j];
        ++cur_cell_id;
      }
    assert(cur_cell_id == new_cell_set.size());

//    std::cout << "new cell size:   " << new_cells.size(2)
//              << " new cell may have copy size: " << only_vert_top.size()
//              << " all tet size: " << all_tet.size(2) << std::endl;

    return detect_topology(new_cells) && detect_topology(all_tet);
  }

  int topology_operation::deal_collapse_edge_case_tri_mesh(const simplex_handle& sh,
                                                           const std::vector<simplex_handle>& edge_verts,
                                                           std::vector<simplex_handle>& del_simplexes)
  {
    const std::vector<simplex_handle> edge_tops =
        cur_mesh_.get_simplex_manager().get_specific_simplex(sh).get_par_co_boundary();
    for(size_t i = 0; i < edge_tops.size(); ++i)
      {
        simplex_handle edge_0, edge_1;
        cur_mesh_.get_vertex_opposite_simplex(edge_verts[0], edge_tops[i], edge_0);
        cur_mesh_.get_vertex_opposite_simplex(edge_verts[1], edge_tops[i], edge_1);
        if(cur_mesh_.get_specific_simplex(edge_0).par_co_boundary_size() == 2 ||
           cur_mesh_.get_specific_simplex(edge_1).par_co_boundary_size() == 2)
          {
            update_simplex_info(edge_0);
          }
        else
          {
            del_simplexes.push_back(edge_0);
            // !!! undo here !!!, maybe the vetex opposite to the edge sh can also be deleted
          }
      }
    return 0;
  }

  int topology_operation::deal_collapse_edge_case_tet_mesh(const simplex_handle& sh,
                                                           const std::vector<simplex_handle>& edge_verts,
                                                           std::vector<simplex_handle>& del_simplexes)
  {
    std::vector<simplex_handle> edge_tops;
    std::set<simplex_handle> visited_common_edges;
    std::set<simplex_handle> visited_common_ver;
    cur_mesh_.get_k_co_boundary_simplex(sh, cur_mesh_.top_dim(), edge_tops);
    for(size_t i = 0; i < edge_tops.size(); ++i)
      {
        simplex_handle face_0;
        cur_mesh_.get_vertex_opposite_simplex(edge_verts[0], edge_tops[i], face_0);

        simplex_handle face_1;
        cur_mesh_.get_vertex_opposite_simplex(edge_verts[1], edge_tops[i], face_1);

        // change the relation of the face_0, face_0 will not be deleted
        if(cur_mesh_.get_specific_simplex(face_0).get_par_co_boundary().size() == 2 ||
           cur_mesh_.get_specific_simplex(face_1).get_par_co_boundary().size() == 2)
          {
            const std::vector<simplex_handle>& edges =
                cur_mesh_.get_specific_simplex(face_0).get_boundary();
            for(size_t j = 0; j < edges.size(); ++j)
              {
                assert(!cur_mesh_.is_simplex_deleted(edges[j]));
                std::vector<simplex_handle>& edge_par_bound =
                    cur_mesh_.get_specific_simplex(edges[j]).get_par_co_boundary();
                assert(edge_par_bound.size() == 1);
                edge_par_bound[0] = face_0;
                const std::vector<simplex_handle>& vertex =
                    cur_mesh_.get_specific_simplex(edges[j]).get_boundary();
                if(vertex[0] != edge_verts[1] && vertex[1] != edge_verts[1])
                  continue;
                for(size_t k = 0; k < vertex.size(); ++k)
                  {
                    if(visited_common_ver.find(vertex[k]) != visited_common_ver.end())
                      continue;
                    visited_common_ver.insert(vertex[k]);
                    std::vector<simplex_handle>& ver_par_bound =
                        cur_mesh_.get_specific_simplex(vertex[k]).get_par_co_boundary();
                    assert(ver_par_bound.size() == 1);
                    ver_par_bound[0] = edges[j];
                  }
              }
          }
        else
          {
            simplex_handle e_sh;
            cur_mesh_.get_vertex_opposite_simplex(edge_verts[0], face_1, e_sh);
            del_simplexes.push_back(e_sh);
            del_simplexes.push_back(face_0);

            std::vector<simplex_handle> other_verts, given_verts;
            given_verts.push_back(edge_verts[1]);
            cur_mesh_.get_other_verts(face_0, given_verts, other_verts);
            assert(other_verts.size() == 2);
            for(size_t j = 0; j < other_verts.size(); ++j)
              {
                // is the face0 deleted
                simplex_handle face_other;
                cur_mesh_.get_vertex_opposite_simplex(other_verts[j], edge_tops[i], face_other);
                simplex_handle common_edge;
                cur_mesh_.get_vertex_opposite_simplex(edge_verts[0], face_other, common_edge);
                assert(!cur_mesh_.is_simplex_deleted(common_edge));
                const std::vector<simplex_handle>& par_bo =
                    cur_mesh_.get_specific_simplex(face_other).get_par_co_boundary();
                bool flg = true;
                if(visited_common_edges.find(common_edge) != visited_common_edges.end())
                  continue;
                visited_common_edges.insert(common_edge);
                if(par_bo.size() != 2)
                  {
                    del_simplexes.push_back(common_edge);
                    flg = false;
                    // !!! undo here !!!, maybe the vetex opposite to the edge sh can also be deleted
                  }
                else
                  {
                    for(size_t k = 0; k < par_bo.size(); ++k)
                      {
                        if(par_bo[k] == edge_tops[i])
                          continue;
                        simplex_handle adj_face;
                        cur_mesh_.get_vertex_opposite_simplex(edge_verts[0], par_bo[k], adj_face);
                        simplex_handle another_face;
                        cur_mesh_.get_vertex_opposite_simplex(edge_verts[1], par_bo[k], another_face);
                        if(cur_mesh_.get_specific_simplex(adj_face).par_co_boundary_size() != 2 &&
                           cur_mesh_.get_specific_simplex(another_face).par_co_boundary_size() != 2)
                          {
                            del_simplexes.push_back(common_edge);
                            flg = false;
                            // !!! undo here !!!, maybe the vetex opposite to the edge sh can also be deleted
                          }
                      }
                  }
                // update the relation of the common edge
                if(flg)
                  {
                    const std::vector<simplex_handle>& common_edge_ver =
                        cur_mesh_.get_specific_simplex(common_edge).get_boundary();

                    for(size_t k = 0; k < common_edge_ver.size(); ++k)
                      {
                        if(visited_common_ver.find(common_edge_ver[k]) != visited_common_ver.end())
                          continue;
                        visited_common_ver.insert(common_edge_ver[k]);
                        std::vector<simplex_handle>& ver_par_co =
                            cur_mesh_.get_specific_simplex(common_edge_ver[k]).get_par_co_boundary();
                        assert(ver_par_co.size() == 1);
                        ver_par_co[0] = common_edge;
                      }
                  }
              }
          }
      }
    return 0;
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
    return true;
  }

  int topology_operation::update_simplex_info(const simplex_handle& sh)
  {
    assert(!cur_mesh_.is_simplex_deleted(sh));
    if(sh.dim() == 2)
      {
        const std::vector<simplex_handle>& edges = cur_mesh_.get_specific_simplex(sh).get_boundary();
        for(size_t j = 0; j < edges.size(); ++j)
          {
            assert(!cur_mesh_.is_simplex_deleted(edges[j]));
            std::vector<simplex_handle>& edge_par_bound = cur_mesh_.get_specific_simplex(edges[j]).get_par_co_boundary();
            assert(edge_par_bound.size() == 1);
            edge_par_bound[0] = sh;
            const std::vector<simplex_handle>& vertex = cur_mesh_.get_specific_simplex(edges[j]).get_boundary();
            for(size_t k = 0; k < vertex.size(); ++k)
              {
                std::vector<simplex_handle>& ver_par_bound =
                    cur_mesh_.get_specific_simplex(vertex[k]).get_par_co_boundary();
                assert(ver_par_bound.size() == 1);
                ver_par_bound[0] = edges[j];
              }
          }
      }
    else if(sh.dim() == 1)
      {
        const std::vector<simplex_handle>& vertex = cur_mesh_.get_specific_simplex(sh).get_boundary();
        for(size_t k = 0; k < vertex.size(); ++k)
          {
            std::vector<simplex_handle>& ver_par_bound =
                cur_mesh_.get_specific_simplex(vertex[k]).get_par_co_boundary();
            assert(ver_par_bound.size() == 1);
            ver_par_bound[0] = sh;
          }
      }
    return 0;
  }

  void topology_operation::debug_print_edge_adj_cells(const simplex_handle &sh)
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

    matrixst new_cells(cur_mesh_.top_dim() + 1, only_vert_top.size() + edge_adj_top.size());
    int t;
    for(size_t i = 0; i < only_vert_top.size(); ++i)
      {
        std::vector<simplex_handle> bounds;
        cur_mesh_.get_k_boundary_simplex(only_vert_top[i], 0, bounds);
        for(size_t j = 0; j < bounds.size(); ++j)
          {
            t = bounds[j].id();
            new_cells(j, i) = t;
          }
      }
    for(size_t i = 0; i < edge_adj_top.size(); ++i)
      {
        std::vector<simplex_handle> bounds;
        cur_mesh_.get_k_boundary_simplex(edge_adj_top[i], 0, bounds);
        for(size_t j = 0; j < bounds.size(); ++j)
          {
            t = bounds[j].id();
            new_cells(j, i + only_vert_top.size()) = t;
          }
      }
    matrixd node;
    simplex_handle cur_sh;
    cur_sh.set_dim(0);
    node.resize(3, cur_mesh_.get_simplex_manager().n_element(0));
    for(size_t i = 0; i < node.size(2); ++i)
      {
        cur_sh.set_id(i);
        node(zjucad::matrix::colon(), i) = cur_mesh_.get_coord(cur_sh);
      }

    io::tet_mesh_write_to_zjumat("edge-adj-tet", &node, &new_cells);
  }
}
