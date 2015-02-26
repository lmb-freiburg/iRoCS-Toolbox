/**************************************************************************
**       Title: solution info struct needed by Solver
**    $RCSfile$
**   $Revision: 476 $$Name$
**       $Date: 2004-08-26 10:36:59 +0200 (Thu, 26 Aug 2004) $
**   Copyright: GPL $Author: ronneber $
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
**  Revision 1.1  2003/05/19 11:20:04  ronneber
**  - moved to own File
**
**
**
**************************************************************************/

#ifndef SOLUTIONINFO_HH
#define SOLUTIONINFO_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <iostream>

namespace svt
{
  struct SolutionInfo 
  {
    int    iter;            // number of iterations, that the solver needed
    double nu;              // resulting nu
    double cost;              // resulting cost
    double obj;             // value of objective function
    double rho;             // offset of hyperplane
    double upper_bound_p;   // upper bound for positive support vectors
    double upper_bound_n;   // upper bound for negative support vectors
    int    nSV;             // number of support vectors
    int    nBSV;            // number of bounded support vectors
    double r;               // for Solver_NU:  fraction of support vectors???

    void print( std::ostream& os)
          {
            os << "#iter = " << iter << "\n"
                "nu = " << nu << "\n"
                "C = " << cost << "\n"
                "obj = " << obj << ", rho = " << rho << "\n"
                "nSV = " << nSV << ", nBSV = " << nBSV << "\n";
          }
    
                
            
  };

}


#endif
