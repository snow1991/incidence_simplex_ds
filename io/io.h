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

    int read_mesh(const matrixd& node, const matrixst& top_simplex, mesh_type& mesh);

    int write_mesh(matrixd& node, matrixst& top_simplex, const mesh_type& mesh);

    int tet_mesh_read_from_zjumat(
        const char *path,
        matrixd *node = 0,
        matrixst *tet = 0,
        matrixst *tri = 0);

    int tet_mesh_write_to_zjumat(
        const char *path,
        matrixd *node = 0,
        matrixst *tet = 0,
        matrixst *tri = 0);

    template <typename T>
    int read_matrix(std::istream &is, zjucad::matrix::matrix<T> &m)
    {
            int nrow, ncol;
            is.read((char *)&nrow, sizeof(int));
            is.read((char *)&ncol, sizeof(int));
//            std::cout << nrow << "  " << ncol << std::endl;
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
