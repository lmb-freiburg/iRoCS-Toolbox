/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 534 $$Name$
**       $Date: 2004-09-09 16:44:53 +0200 (Thu, 09 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2004/09/09 14:44:53  ronneber
**  - initial revision
**
**
**
**************************************************************************/

#include <string>
#include <sstream>

#include "lmbunit.hh"
#include <libsvmtl/SparseFV.hh>


static void testInput()
{
  std::istringstream iss( "+1 1:0.708333 2:1 3:1 4:-0.320755 5:-0.105023 6:-1 7:1 8:-0.419847 9:-1 10:-0.225806 12:1 13:-1 ");
  
  svt::SparseFV fv;
  
  iss >> fv;

  LMBUNIT_ASSERT_EQUAL_DELTA( fv[1], 0.708333, 0.0000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( fv[2], 1, 0.0000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( fv[3], 1, 0.0000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( fv[4], -0.320755, 0.0000001);
}



int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST( testInput());
  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}


