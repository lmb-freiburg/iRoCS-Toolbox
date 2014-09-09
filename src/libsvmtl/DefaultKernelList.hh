/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 797 $$Name$
**       $Date: 2006-01-25 09:53:51 +0100 (Wed, 25 Jan 2006) $
**   Copyright: GPL $Author: fehr $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.4  2006/01/25 08:53:51  fehr
**  added HistIntersect to Kmatrix Kernels and new BasicSVMAdapter interface to TwoClassModels
**
**  Revision 1.3  2005/09/15 14:21:18  fehr
**  histogramm intersection kernel added
**
**  Revision 1.2  2004/09/03 07:06:59  ronneber
**  - Added Kernel_SCALE for feature vector scaling
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/
#ifndef DEFAULTKERNELLIST_HH
#define DEFAULTKERNELLIST_HH

#include "TList.hh"
#include "Kernel_LINEAR.hh"
#include "Kernel_RBF.hh"
#include "Kernel_POLY.hh"
#include "Kernel_SIGMOID.hh"
#include "Kernel_MATRIX.hh"
#include "Kernel_SCALE.hh"
#include "Kernel_HISTINTERSECT.hh"

namespace svt
{
  /*-------------------------------------------------------------------------
   *  specify kernel functions
   *-------------------------------------------------------------------------*/
  typedef TLIST_15( 
      svt::Kernel_MATRIX< svt::Kernel_RBF>, 
      svt::Kernel_MATRIX< svt::Kernel_LINEAR>, 
      svt::Kernel_MATRIX< svt::Kernel_POLY>, 
      svt::Kernel_MATRIX< svt::Kernel_SIGMOID>,
      svt::Kernel_MATRIX< svt::Kernel_HISTINTERSECT>,
      svt::Kernel_MATRIX< svt::Kernel_SCALE< svt::Kernel_RBF> >, 
      svt::Kernel_MATRIX< svt::Kernel_SCALE< svt::Kernel_LINEAR> >, 
      svt::Kernel_MATRIX< svt::Kernel_SCALE< svt::Kernel_POLY> >, 
      svt::Kernel_MATRIX< svt::Kernel_SCALE< svt::Kernel_SIGMOID> >,
      svt::Kernel_MATRIX< svt::Kernel_SCALE< svt::Kernel_HISTINTERSECT> >,
      svt::Kernel_RBF, 
      svt::Kernel_LINEAR, 
      svt::Kernel_POLY, 
      svt::Kernel_SIGMOID,
      svt::Kernel_HISTINTERSECT) DefaultKernelList;

}

#endif
