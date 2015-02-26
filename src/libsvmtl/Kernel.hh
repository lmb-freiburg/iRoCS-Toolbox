/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 509 $$Name$
**       $Date: 2004-09-03 13:35:04 +0200 (Fri, 03 Sep 2004) $
**   Copyright: LGPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2004/09/03 11:35:04  ronneber
**  - replaced Chi-Jen Lin's own min,max,swap with std::min,
**    std::max. std::swap to compile with programs, that "#include
**    <algorithm>" and do a "using namespace std;"
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef KERNEL_HH
#define KERNEL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

//
// Kernel evaluation
//
// the static method k_function is for doing single kernel evaluation
// the constructor of Kernel prepares to calculate the l*l kernel matrix
// the member function get_Q is for getting one column from the Q Matrix
//
// libsvm -> libsvmtl:
// - svm_node is replaced by FV template class
// - kernel evaluation is done in given KF template class
// - caching of square_x for RBF-kernels is done in BasicFV class
//
namespace svt
{
  template< typename FV, typename KF>
  class Kernel {
  public:
    Kernel( const KF& kernel, int l, FV * const * x);
    virtual ~Kernel();

  private:
    // forbid copying
    Kernel( const Kernel<FV,KF>& orig) {}
    void operator=( const Kernel<FV,KF>& orig) {}
  public:
    

//    static double k_function(const svm_node *x, const svm_node *y,
//                             const svm_parameter& param);
    virtual Qfloat *get_Q(int column, int len) const = 0;
    virtual void swap_index(int i, int j) const	// no so const...
          {
            std::swap(x[i],x[j]);
          }
  protected:
    
    double kernel_function(int i, int j) const
        {
          return _kernel.k_function( *(x[i]), *(x[j]));
        }
    
    
  private:
    const FV** x; // array of pointers to feature vectors
    const KF&    _kernel;
    

  };
}

#include "Kernel.icc"
#endif
