/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 476 $$Name$
**       $Date: 2004-08-26 10:36:59 +0200 (Thu, 26 Aug 2004) $
**   Copyright: LGPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef SOLVER_NU_HH
#define SOLVER_NU_HH

#include "SolutionInfo.hh"
#include "Solver.hh"

namespace svt
{
  //
  // Solver for nu-svm classification and regression
  //
  // additional constraint: e^T \alpha = constant
  //
  template< typename FV, typename KF>
  class Solver_NU : public Solver<FV,KF>
  {
  public:
    Solver_NU() {}
    void Solve(int l, const Kernel<FV,KF>& Q, const double *b, const schar *y,
               double *alpha, double Cp, double Cn, double eps,
               SolutionInfo* si, int shrinking,
               ProgressReporter* progressReporter)
          {
            this->si = si;
            Solver<FV,KF>::Solve(l,Q,b,y,alpha,Cp,Cn,eps,si,shrinking,
                                 progressReporter);
          }
  private:
    SolutionInfo *si;
    int select_working_set(int &i, int &j);
    double calculate_rho();
    void do_shrinking();
  };
}

#include "Solver_NU.icc"

#endif
