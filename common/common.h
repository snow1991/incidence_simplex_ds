#ifndef IS_COMMON_H
#define IS_COMMON_H

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <time.h>
#include <set>
#include <zjucad/matrix/matrix.h>

namespace is_mesh
{
  typedef zjucad::matrix::matrix<double> matrixd;
  typedef zjucad::matrix::matrix<size_t> matrixst;
  const double ZERO = 1e-10;


  class timer{
  public:
    timer():begin_(0), end_(0){}
    void start(){ begin_ = clock();}
    void finish(){end_ = clock();}
    long result()const{return (end_ - begin_)/1000;}
    long result_c()const{return (end_ - begin_);}
  private:
    long begin_, end_;
  };
}


#endif // IS_COMMON_H
