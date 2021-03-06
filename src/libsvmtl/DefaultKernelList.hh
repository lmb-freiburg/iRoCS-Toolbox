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

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
