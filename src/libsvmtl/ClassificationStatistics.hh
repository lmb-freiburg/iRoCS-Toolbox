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
                         
