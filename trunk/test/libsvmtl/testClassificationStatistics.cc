/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 4820 $$Name$
**       $Date: 2011-11-08 10:57:01 +0100 (Tue, 08 Nov 2011) $
**   Copyright: GPL $Author: tschmidt $
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

#include <cstdlib>
#include "lmbunit.hh"
#include <libsvmtl/ClassificationStatistics.hh>

static void simpleTest()
{
  std::vector<double> trueLabels;
  std::vector<double> predictedLabels;
  
  for( int  i = -3; i < 4; ++i)
  {
    for( int j = -3; j < 4; ++j)
    {
      trueLabels.push_back( i);
      predictedLabels.push_back(j);
    }
  }
  
  svt::ClassificationStatistics cs;
  std::vector<svt::SingleClassResult> resultTable;
  cs.calcStatistics( trueLabels, predictedLabels, resultTable);
  cs.prettyPrintStatistics( resultTable, LMBUNIT_DEBUG_STREAM);
  

  LMBUNIT_ASSERT_EQUAL( resultTable[0].label, -3);
  LMBUNIT_ASSERT_EQUAL( resultTable[0].truePositives, 1);
  LMBUNIT_ASSERT_EQUAL( resultTable[0].trueNumber, 7);
  LMBUNIT_ASSERT_EQUAL( resultTable[0].falsePositives, 6);

  LMBUNIT_ASSERT_EQUAL( resultTable[1].label, -2);
  LMBUNIT_ASSERT_EQUAL( resultTable[1].truePositives, 1);
  LMBUNIT_ASSERT_EQUAL( resultTable[1].trueNumber, 7);
  LMBUNIT_ASSERT_EQUAL( resultTable[1].falsePositives, 6);
}

int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST( simpleTest());
  
  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}
