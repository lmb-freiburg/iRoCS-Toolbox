/**************************************************************************
**       Title: test grid axis
**    $RCSfile$
**   $Revision: 524 $$Name$
**       $Date: 2004-09-08 16:45:49 +0200 (Wed, 08 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2004/09/08 14:45:19  ronneber
**  *** empty log message ***
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#include <libsvmtl/GridAxis.hh>
#include "lmbunit.hh"

static void testCreateLinear()
{
  svt::GridAxis ga;
  ga.parseString( "gamma:0,add2.5,12.5");
  LMBUNIT_ASSERT_EQUAL( ga.keyName(), "gamma");
  LMBUNIT_ASSERT_EQUAL( ga.nValues(), 6);
  LMBUNIT_ASSERT_EQUAL_DELTA( ga.value(0), 0, 0.0000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( ga.value(1), 2.5, 0.0000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( ga.value(2), 5, 0.0000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( ga.value(3), 7.5, 0.0000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( ga.value(4), 10, 0.0000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( ga.value(5), 12.5, 0.0000001);
  LMBUNIT_ASSERT_EQUAL( ga.changesKernel(), true);
}

static void testCreateLog()
{
  svt::GridAxis ga;
  ga.parseString( "cost:1,mul2,16");
  LMBUNIT_ASSERT_EQUAL( ga.keyName(), "cost");
  LMBUNIT_ASSERT_EQUAL( ga.nValues(), 5);
  LMBUNIT_ASSERT_EQUAL_DELTA( ga.value(0), 1, 0.0000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( ga.value(1), 2, 0.0000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( ga.value(2), 4, 0.0000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( ga.value(3), 8, 0.0000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( ga.value(4), 16, 0.0000001);
}

static void testCreateList()
{
  svt::GridAxis ga( "coef0:-5,-23,42");
  LMBUNIT_ASSERT_EQUAL( ga.keyName(), "coef0");
  LMBUNIT_ASSERT_EQUAL( ga.nValues(), 3);
  LMBUNIT_ASSERT_EQUAL_DELTA( ga.value(0), -5, 0.0000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( ga.value(1), -23, 0.0000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( ga.value(2), 42, 0.0000001);
}


static void testChangesKernel()
{
  svt::GridAxis ga;
  // default is true
  LMBUNIT_ASSERT_EQUAL( ga.changesKernel(), true);
  ga.setChangesKernel( false);
  LMBUNIT_ASSERT_EQUAL( ga.changesKernel(), false);
}

  

int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();
  LMBUNIT_RUN_TEST( testCreateLinear() );
  LMBUNIT_RUN_TEST( testCreateLog() );
  LMBUNIT_RUN_TEST( testCreateList() );
  LMBUNIT_RUN_TEST( testChangesKernel() );
  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}


