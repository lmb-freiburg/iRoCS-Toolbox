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
**   $Revision: 754 $$Name$
**       $Date: 2005-10-26 08:59:17 +0200 (Wed, 26 Oct 2005) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2005/10/26 06:59:17  ronneber
**  - added prettyPrintConfusionTable()
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef CLASSIFICATIONSTATISTICS_HH
#define CLASSIFICATIONSTATISTICS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>
#include <iostream>

namespace svt
{
  struct SingleClassResult
  {
    SingleClassResult()
            : label(0),
              truePositives(0),
              trueNumber(0),
              falsePositives(0)
          {}

    int label;
    int truePositives;
    int trueNumber;
    int falsePositives;
  };
  
  class ClassificationStatistics
  {
  public:
    /*======================================================================*/
    /*! 
     *   calculate per class statistics (truePositives and
     *   falsePositives) for given classifcation results 
     *
     *   \param trueLabels       true labels
     *   \param predictedLabels  predicted labels
     *   \param resultTable      per class results. vector will be resized
     *                           properly 
     *
     */
    /*======================================================================*/
    void calcStatistics( const std::vector<double>& trueLabels,
                         const std::vector<double>& predictedLabels,
                         std::vector<SingleClassResult>& resultTable);

    void prettyPrintStatistics( 
        const std::vector<SingleClassResult>& resultTable, std::ostream& os );
    
    void prettyPrintConfusionTable( 
        const std::vector<double>& trueLabels,
        const std::vector<double>& predictedLabels,
        std::ostream& os);

  };
}


#endif
                         
