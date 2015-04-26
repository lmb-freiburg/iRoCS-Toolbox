/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 525 $$Name$
**       $Date: 2004-09-08 16:50:37 +0200 (Wed, 08 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2004/09/08 14:49:59  ronneber
**  - initial revision
**
**
**
**************************************************************************/

#include "lmbunit.hh"
#include <vector>
#include <libsvmtl/ParamInfo.hh>

static void testConstructor()
{
  svt::ParamInfo p( "gamma", "g", "value", "set gamma for RBF Kernel (default 1/k)");
  p.addGuiHint( "type", "lineedit");
  p.addGuiHint( "default", "1");
  
  LMBUNIT_ASSERT_EQUAL( p.longKey(), "gamma");
  LMBUNIT_ASSERT_EQUAL( p.shortKey(), "g");
  LMBUNIT_ASSERT_EQUAL( p.nAlternatives(), 1);
  
  LMBUNIT_ASSERT_EQUAL( p.alternatives(0).value, "value");
  LMBUNIT_ASSERT_EQUAL( p.alternatives(0).helpText, "set gamma for RBF Kernel (default 1/k)")
}


static void testSort()
{
  std::vector<svt::ParamInfo> paramInfoList;
  
  svt::ParamInfo p1( "gamma", "g", "value", 
                     "set gamma for RBF Kernel (default 1/k)");
  p1.addGuiHint( "type", "lineedit");
  p1.addGuiHint( "default", "1");
  paramInfoList.push_back( p1);
  

  svt::ParamInfo p2( "cost", "c", "value", 
                     "set cost for two-class SVM");
  p2.addGuiHint( "type", "lineedit");
  p2.addGuiHint( "default", "1");
  paramInfoList.push_back( p2);

  paramInfoList.push_back( 
      svt::ParamInfo( "gamma", "g", "value", 
                      "another gamma value"));
  paramInfoList.back().addGuiHint( "type", "combobox");
  paramInfoList.back().addGuiHint( "default", "1");

  std::sort( paramInfoList.begin(), paramInfoList.end());
  
  LMBUNIT_ASSERT_EQUAL( paramInfoList[0].longKey(), "cost");
  LMBUNIT_ASSERT_EQUAL( paramInfoList[1].longKey(), "gamma");
  LMBUNIT_ASSERT_EQUAL( paramInfoList[2].longKey(), "gamma");
  LMBUNIT_ASSERT_EQUAL( paramInfoList[2].alternatives(0).helpText, "set gamma for RBF Kernel (default 1/k)");
}

static void testUnique()
{
  std::vector<svt::ParamInfo> paramInfoList;
  
  svt::ParamInfo p1( "gamma", "g", "value", 
                     "set gamma for RBF Kernel (default 1/k)");
  p1.addGuiHint( "type", "lineedit");
  p1.addGuiHint( "default", "1");
  paramInfoList.push_back( p1);
  

  svt::ParamInfo p2( "cost", "c", "value", 
                     "set cost for two-class SVM");
  p2.addGuiHint( "type", "lineedit");
  p2.addGuiHint( "default", "1");
  paramInfoList.push_back( p2);

  paramInfoList.push_back( 
      svt::ParamInfo( "gamma", "g", "value", 
                      "another gamma value"));
  paramInfoList.back().addGuiHint( "type", "combobox");
  paramInfoList.back().addGuiHint( "default", "1");

  paramInfoList.push_back( 
      svt::ParamInfo( "gamma", "g", "value", 
                      "another gamma value"));
  paramInfoList.back().addGuiHint( "type", "combobox");
  paramInfoList.back().addGuiHint( "default", "1");

  LMBUNIT_ASSERT_EQUAL( paramInfoList.size(), 4);

  std::sort( paramInfoList.begin(), paramInfoList.end());
  paramInfoList.erase( std::unique(paramInfoList.begin(), paramInfoList.end()),
                       paramInfoList.end());
  
  LMBUNIT_ASSERT_EQUAL( paramInfoList.size(), 3);
  
  LMBUNIT_ASSERT_EQUAL( paramInfoList[0].longKey(), "cost");
  LMBUNIT_ASSERT_EQUAL( paramInfoList[1].longKey(), "gamma");
  LMBUNIT_ASSERT_EQUAL( paramInfoList[2].longKey(), "gamma");
  LMBUNIT_ASSERT_EQUAL( paramInfoList[2].alternatives(0).helpText, "set gamma for RBF Kernel (default 1/k)");
}



int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST( testConstructor() );
  LMBUNIT_RUN_TEST( testSort() );
  LMBUNIT_RUN_TEST( testUnique() );
  
  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}


