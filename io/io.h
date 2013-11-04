#ifndef IS_IO_H
#define IS_IO_H

#include "../mesh/topology_kernel.h"
#include "../mesh/mesh.h"

#include <fstream>
#include <stdint.h>

namespace is_mesh
{
  namespace io
  {
    typedef mesh mesh_type;

    /// This function creates a mesh from node and top simplex
    /** This function creates a mesh from node and top simplex which were got from file
      * \param node the coordinate of the nodes, it is a 3*N matrix, N is the number of nodes
      * \param top_simplex the top simplex of the mesh, it is a 3*M(triangle) mesh or 4*M(tet mesh) matrix, M is the number of top simplex
      * \param mesh the class we store the infomation of the input mesh
      */
    int read_mesh(const matrixd& node, const matrixst& top_simplex, mesh_type& mesh);

    /// This function write a mesh to matrix
    /** This function write the top simplex and nodes of a mesh to matrix
      * \param node the coordinate of the nodes, it is a 3*N matrix, N is the number of nodes
      * \param top_simplex the top simplex of the mesh, it is a 3*M(triangle) mesh or 4*M(tet mesh) matrix, M is the number of top simplex
      * \param mesh the mesh which we want to write out
      * \param is_remove if true, remove the extra nodes of the mesh, else not
      */
    int write_mesh(matrixd& node, matrixst& top_simplex, const mesh_type& mesh, bool is_remove = true);

    /// This function read a tet mesh from file
    /** This function read a tet mesh from file and write the nodes and top simplex into matrix
      * \param path the file path of the tet file
      * \param node the coordinate of the nodes, it is a 3*N matrix, N is the number of nodes
      * \param tet the top simplex of the tet mesh, it is 4*M(tet mesh) matrix, M is the number of tet
      * \param tri the default value is 0
      */
    int tet_mesh_read_from_zjumat(
        const char *path,
        matrixd *node = 0,
        matrixst *tet = 0,
        matrixst *tri = 0);

    /// This function write a tet mesh into file
    /** This function write a tet mesh into file from the nodes and top simplex matrix of the mesh
      * \param path the file path of the tet file we want to write
      * \param node the coordinate of the nodes, it is a 3*N matrix, N is the number of nodes
      * \param tet the top simplex of the tet mesh, it is 4*M(tet mesh) matrix, M is the number of tet
      * \param tri the default value is 0
      */
    int tet_mesh_write_to_zjumat(
        const char *path,
        matrixd *node = 0,
        matrixst *tet = 0,
        matrixst *tri = 0);

    /// This function remove the nodes which was not used
    /** \param cells the input cells
      * \param node the input nodes
      * \return
      */
    int remove_extra_nodes(matrixst &cells, matrixd &node);

    template <typename T>
    int read_matrix(std::istream &is, zjucad::matrix::matrix<T> &m)
    {
            int nrow, ncol;
            is.read((char *)&nrow, sizeof(int));
            is.read((char *)&ncol, sizeof(int));
            m.resize(nrow, ncol);
            is.read((char *)&m[0], sizeof(T)*m.size());
            return is.fail();
    }

    template <typename T>
    int read_matrix(std::istream &is, zjucad::matrix::matrix<T> &m, size_t nrow)
    {
            zjucad::matrix::matrix<T> tmp;
            read_matrix(is, tmp);
            if((tmp.size() % nrow) != 0)
                    return -1;
            m.resize(nrow, tmp.size()/nrow);
            copy(tmp.begin(), tmp.end(), m.begin());
            return is.fail();
    }

    template <typename T>
    int write_matrix(std::ostream &os, const zjucad::matrix::matrix<T> &m)
    {
        int nrow = m.size(1), ncol = m.size(2);
            os.write((const char *)&nrow, sizeof(int));
            os.write((const char *)&ncol, sizeof(int));
        os.write((const char *)&m[0], sizeof(T)*m.size());
            return os.fail();
    }

    template <typename T>
    inline int read_matrix(const char *path, zjucad::matrix::matrix<T> &m)
    {
            std::ifstream ifs(path, std::ifstream::binary);
            if(ifs.fail()) {
                    std::cerr << "open " << path << " for read fail." << std::endl;
                    return __LINE__;
            }
            read_matrix(ifs, m);
            return ifs.fail();
    }

    template <typename T>
    inline int write_matrix(const char *path, const zjucad::matrix::matrix<T> &m)
    {
            std::ofstream ofs(path, std::ofstream::binary);
            if(ofs.fail()) {
                    std::cerr << "open " << path << " for write fail." << std::endl;
                    return __LINE__;
            }
            write_matrix(ofs, m);
            return ofs.fail();
    }
  }
}

#endif
