/**************************************************************************
**       Title: perform a grid search on parameters for best crossval result
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
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef GRIDSEARCH_HH
#define GRIDSEARCH_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <iomanip>
#include "ClassificationStatistics.hh"

namespace svt
{
  class GridSearch
  {
  public:
    
    GridSearch()
            :_pr(0),
             _printGridLevel(0)
          {}
    
    
    /*======================================================================*/
    /*! 
     *   set progress reporter object. 0 means no progress
     *   reporting. You are still the owner of this object,
     *   so you are responsible, that it exists during further calls
     *   to train(), etc. and that it is deleted afterwards
     *
     *   \param pr pointer to ProgressReporter object or 0 if no
     *             progress reposting is desired 
     */
    /*======================================================================*/
    void setProgressReporter( ProgressReporter* pr)
          {
            _pr = pr;
          }
    

    /*======================================================================*/
    /*! 
     *   wether to print the grid (via ProgressReporter) during evaluation 
     *   (default 0)
     *
     *   \param  printGridLevel 0: nothing, 1: print the grid 
     *
     */
    /*======================================================================*/
    void setPrintGridFlag( int printGridLevel)
          {
            _printGridLevel = printGridLevel;
          }


    /*======================================================================*/
    /*! 
     *   search on a 2D grid (line wise). For speed optimziation, the
     *   changesKernel() flag of col (and row) may be set to false. In
     *   this case a cached Kernel Matrix of the previous gridpoint is
     *   reused.
     *
     *   \param row parameter name and its values along the grids row
     *   \param col parameter name and its values along the grids column
     *   \param cv  the cross validator that is executed at each grid
     *              point (setTrainingData() must be done already)
     *   \param subsetIndexByUID division of training data into
     *              subsets for doFullCV() method of cross validator
     *   \param gridPointInfos (output) crossvalidation results for
     *              each grid point (linear storage: index =
     *              row*ncols+col). vector will be resized properly
     *   \param bestGridPointIndex (output) index of the gridpoint with best
     *              crossvalidation result. 
     *   \param bestResultTable per class results. vector will be resized
     *              properly 
     */
    /*======================================================================*/
    template< typename CROSSVALIDATOR>
    void search2D( const GridAxis& row, const GridAxis& col, 
                   CROSSVALIDATOR* cv, 
                   const std::vector<int>& subsetIndexByUID,
                   std::vector<svt::StDataASCII>& gridPointInfos,
                   unsigned int& bestGridPointIndex,
                   std::vector<svt::SingleClassResult>& bestResultTable);
    
  private:
    ProgressReporter* _pr;
    int               _printGridLevel;
    
  };
}

#include "GridSearch.icc"
  
#endif

