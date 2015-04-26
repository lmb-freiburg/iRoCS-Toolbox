/**************************************************************************
 *
 * Copyright (C) 2004-2015 Olaf Ronneberger, Florian Pigorsch, Jörg Mechnich,
 *                         Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

/**************************************************************************
**       Title: Optimizer / Solver
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
**  Revision 1.4  2003/05/19 11:20:56  ronneber
**  - moved SolutionInfo struct to own class
**
**  Revision 1.3  2002/05/08 15:03:23  ronneber
**  - calculate_rho now writes its results to the passed SolutionInfo
**    struct (to get rid of the errornous private copy of si in SolverNu)
**
**  Revision 1.2  2002/05/08 10:36:11  ronneber
**  - added some debugging stuff
**
**  Revision 1.1  2002/03/26 12:44:02  ronneber
**  restructured for autoconf
**
**  Revision 1.5  2002/03/13 13:58:52  pigorsch
**  * put sources in Solver.icc
**  * added comments
**
**  Revision 1.4  2002/03/11 17:15:47  mechnich
**  modified some expressions to increase standard conformity (for compilation under Win32)
**
**  Revision 1.3  2002/01/31 10:55:30  pigorsch
**  * made some functions virtual
**
**  Revision 1.2  2001/12/17 13:18:30  pigorsch
**  *** empty log message ***
**
**  Revision 1.1  2001/12/11 11:03:00  pigorsch
**  Initial Revision
**
**
**
**************************************************************************/

#ifndef SOLVER_HH
#define SOLVER_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <algorithm>
#include <vector>
#include <iostream>
#include <cmath>

#include "svm_defines.hh"
#include "SolutionInfo.hh"
#include "Kernel.hh"
#include "ProgressReporter.hh"


namespace svt
{
  // Generalized SMO+SVMlight algorithm
  // Solves:
  //
  //	min 0.5(\alpha^T Q \alpha) + b^T \alpha
  //
  //		y^T \alpha = \delta
  //		y_i = +1 or -1
  //		0 <= alpha_i <= Cp for y_i = 1
  //		0 <= alpha_i <= Cn for y_i = -1
  //
  // Given:
  //
  //	Q, b, y, Cp, Cn, and an initial feasible point \alpha
  //	l is the size of vectors and matrices
  //	eps is the stopping criterion
  //
  // solution will be put in \alpha, objective value will be put in obj
  //
  template< typename FV, typename KF>
  class Solver 
  {
  public:
    Solver() {};
    virtual ~Solver() {};

  private:
    // forbid copying
    Solver( const Solver<FV,KF>& orig) {}
    void operator=( const Solver<FV,KF>& orig) {}
  public:
    

    void Solve(int l, const Kernel<FV,KF>& Q, const double *b_, 
               const schar *y_,
               double *alpha_, double Cp, double Cn, double eps,
               SolutionInfo* si, int shrinking,
               ProgressReporter* progressReporter);

  protected:
    int active_size;
    schar *y;
    double *G;		// gradient of objective function
    enum { LOWER_BOUND, UPPER_BOUND, FREE };
    char *alpha_status;	// LOWER_BOUND, UPPER_BOUND, FREE
    double *alpha;
    const Kernel<FV,KF>* Q;
    double eps;
    double Cp,Cn;
    double *b;
    int *active_set;
    double *G_bar;		// gradient, if we treat free variables as 0
    int l;
    bool unshrinked;	// XXX
    
    double get_C(int i)
          {
            return (y[i] > 0)? Cp : Cn;
          }
    void update_alpha_status(int i)
          {
            if(alpha[i] >= get_C(i))
                alpha_status[i] = UPPER_BOUND;
            else if(alpha[i] <= 0)
                alpha_status[i] = LOWER_BOUND;
            else alpha_status[i] = FREE;
          }
    bool is_upper_bound(int i) { return alpha_status[i] == UPPER_BOUND; }
    bool is_lower_bound(int i) { return alpha_status[i] == LOWER_BOUND; }
    bool is_free(int i) { return alpha_status[i] == FREE; }
    void swap_index(int i, int j);
    void reconstruct_gradient();
    virtual int select_working_set(int &i, int &j);
    virtual double calculate_rho();
    virtual void do_shrinking();
  };
  
}
#include "Solver.icc"

#endif





// old libsvmtl code
 //  
 //  
 //      /*====================================================================*/
 //      /*! 
 //       *   (description)
 //       *
 //       *   \param 
 //       */
 //      /*====================================================================*/
 //      void 
 //      Solve(
 //          int l, 
 //          KQ* Q, 
 //          const std::vector<double>& b_, 
 //          const std::vector<signed char>& y_,
 //          std::vector<double>& alpha_, 
 //          double Cp, 
 //          double Cn, 
 //          double eps,
 //          SolutionInfo& si, 
 //  	bool shrinking);
 //      
 //    protected:
 //      /*====================================================================*/
 //      /*! 
 //       *   (description)
 //       *
 //       *   \param 
 //       *
 //       *   \return
 //       */
 //      /*====================================================================*/   
 //      double 
 //      get_C(int i) const;
 //  
 //  
 //      /*====================================================================*/
 //      /*! 
 //       *   (description)
 //       *
 //       *   \param 
 //       */
 //      /*====================================================================*/   
 //      void 
 //      update_alpha_status(int i);
 //  
 //  
 //      /*====================================================================*/
 //      /*! 
 //       *   (description)
 //       *
 //       *   \param 
 //       *
 //       *   \return
 //       */
 //      /*====================================================================*/
 //      bool 
 //      is_upper_bound(int i) const;
 //  
 //      
 //      /*====================================================================*/
 //      /*! 
 //       *   (description)
 //       *
 //       *   \param 
 //       *
 //       *   \return
 //       */
 //      /*====================================================================*/
 //      bool 
 //      is_lower_bound(int i) const;
 //  
 //  
 //      /*====================================================================*/
 //      /*! 
 //       *   (description)
 //       *
 //       *   \param 
 //       *
 //       *   \return
 //       */
 //      /*====================================================================*/
 //      bool 
 //      is_free(int i) const;
 //      
 //  
 //      /*====================================================================*/
 //      /*! 
 //       *   (description)
 //       *
 //       *   \param 
 //       */
 //      /*====================================================================*/
 //      void 
 //      swap_index(
 //          int i, 
 //          int j);
 //  
 //  
 //      /*====================================================================*/
 //      /*! 
 //       *   (description)
 //       */
 //      /*====================================================================*/
 //      void 
 //      reconstruct_gradient();
 //  
 //  
 //      /*====================================================================*/
 //      /*! 
 //       *   (description)
 //       *
 //       *   \param 
 //       *
 //       *   \return
 //       */
 //      /*====================================================================*/
 //      virtual
 //      int 
 //      select_working_set(
 //          int &out_i, 
 //          int &out_j) const;
 //  
 //  
 //      /*====================================================================*/
 //      /*! 
 //       *   (description)
 //       *
 //       *   \return
 //       */
 //      /*====================================================================*/
 //      virtual
 //      void 
 //      calculate_rho( SolutionInfo& si);
 //  
 //  
 //      /*====================================================================*/
 //      /*! 
 //       *   (description)
 //       *
 //       *   \param 
 //       */
 //      /*====================================================================*/   
 //      virtual
 //      void 
 //      do_shrinking();    
 //  
 //   
 //      int active_size;
 //      std::vector<signed char> y;
 //      double* G;		// gradient of objective function
 //      enum { LOWER_BOUND, UPPER_BOUND, FREE };
 //      char* alpha_status;	// LOWER_BOUND, UPPER_BOUND, FREE
 //      std::vector<double> alpha;
 //      KQ* p_Q;
 //      double eps;
 //      double Cp,Cn;
 //      std::vector<double> b;
 //      int* active_set;
 //      double* G_bar;	// gradient, if we treat free variables as 0
 //      int l;
 //      bool unshrinked;	// XXX
 //  
 //  template< typename T>
 //   void print_array( T p, int len)
 //  {
 //    cout << "( ";
 //    for( int i = 0; i < len; ++i)
 //    {
 //      cout << double(p[i]) << " ";
 //    }
 //    cout << ")\n";
 //  }
 //  
 //    void print_internals()
 //          {
 //            cout << "active_size = " << active_size << endl;
 //            cout << "y           = "; print_array( y, active_size);
 //            cout << "G           = "; print_array( G, active_size);
 //            cout << "alpha_status= "; print_array( alpha_status, active_size);
 //            cout << "alpha       = "; print_array( alpha, active_size);
 //            cout << "eps         = " << eps << endl;
 //            cout << "Cp          = " << Cp << endl;
 //            cout << "Cn          = " << Cn << endl;
 //            cout << "b           = "; print_array( b, active_size);
 //            cout << "active_set  = "; print_array( active_set, active_size);
 //            cout << "G_bar       = "; print_array( G_bar, active_size);
 //            cout << "l           = " << l << endl;
 //            cout << "unshrinked  = " << unshrinked << endl;
 //          }
 //    
 //    };
 //  
 //  #include "Solver.icc"
 //  
 //  }
 //  
 //  #endif
