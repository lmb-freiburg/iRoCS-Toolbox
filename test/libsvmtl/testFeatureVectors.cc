/**************************************************************************
**       Title: test feature vector classes
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
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#include <sstream>

#include "lmbunit.hh"
#include <libsvmtl/BasicFV.hh>
#include <libsvmtl/SparseFV.hh>

template<typename FV>
static void testSimple()
{
  FV fv1, fv2;
  fv1.resize( 3);
  fv1[0] = 1;
  fv1[1] = 2;
  fv1[2] = 3;
  
  fv2.resize( 3);
  fv2[0] = 3;
  fv2[1] = 4;
  fv2[2] = 5;

  LMBUNIT_ASSERT_EQUAL( fv1[0], 1);
  LMBUNIT_ASSERT_EQUAL( fv1[1], 2);
  LMBUNIT_ASSERT_EQUAL( fv1[2], 3);
  LMBUNIT_ASSERT_EQUAL( fv2[0], 3);
  LMBUNIT_ASSERT_EQUAL( fv2[1], 4);
  LMBUNIT_ASSERT_EQUAL( fv2[2], 5);

  std::vector<FV> featureVectors(2);

  featureVectors[0].setLabel(-1);
  featureVectors[0].resize( 3);
  featureVectors[0][0] = 0;
  featureVectors[0][1] = 0;
  featureVectors[0][2] = 0;
  
  
  featureVectors[1].setLabel(+1);
  featureVectors[1].resize( 3);
  featureVectors[1][0] = 1;
  featureVectors[1][1] = 0;
  featureVectors[1][2] = 0;

  LMBUNIT_ASSERT_EQUAL( featureVectors[0].getLabel(), -1);
  LMBUNIT_ASSERT_EQUAL( featureVectors[0][0], 0);
  LMBUNIT_ASSERT_EQUAL( featureVectors[0][1], 0);
  LMBUNIT_ASSERT_EQUAL( featureVectors[0][2], 0);
  LMBUNIT_ASSERT_EQUAL( featureVectors[1].getLabel(), +1);
  LMBUNIT_ASSERT_EQUAL( featureVectors[1][0], 1);
  LMBUNIT_ASSERT_EQUAL( featureVectors[1][1], 0);
  LMBUNIT_ASSERT_EQUAL( featureVectors[1][2], 0);
 
  fv1.setUniqueID( 123);
  LMBUNIT_ASSERT_EQUAL( fv1.uniqueID(), 123);
  
  LMBUNIT_ASSERT_EQUAL( fv1.square(), 14);  
  LMBUNIT_ASSERT_EQUAL( fv2.square(), 50);

  LMBUNIT_ASSERT_EQUAL( fv1.dotProduct(fv2), 26);  
  LMBUNIT_ASSERT_EQUAL( fv2.dotProduct(fv1), 26);  

  FV fv3(fv1);
  FV fv4;
  fv4 = fv2;
  LMBUNIT_ASSERT_EQUAL( fv1, fv3);
  LMBUNIT_ASSERT_EQUAL( fv2, fv4);
}

template<typename FV>
static void testInOut()
{
  FV fv1, fv2;
  fv1.resize( 3);
  fv1.setLabel(-1);
  fv1[0] = 1;
  fv1[1] = 2;
  fv1[2] = 3;
  
  fv2.resize( 3);
  fv1.setLabel(+1);
  fv2[0] = 3;
  fv2[1] = 4;
  fv2[2] = 5;

  /*-----------------------------------------------------------------------
   *  simple input output
   *-----------------------------------------------------------------------*/
  {
    std::ostringstream os;
    os << fv1;
    FV fv3;
    std::istringstream is( os.str());
    is >> fv3;
    LMBUNIT_ASSERT_EQUAL( bool(is), true);
    LMBUNIT_ASSERT_EQUAL( fv1, fv3);
  }
  
  {
    std::ostringstream os;
    os << fv2;
    FV fv4;
    std::istringstream is( os.str());
    is >> fv4;
    LMBUNIT_ASSERT_EQUAL( bool(is), true);
    LMBUNIT_ASSERT_EQUAL( fv2, fv4);
  }
  
  /*-----------------------------------------------------------------------
   *  input output with white spaces before and after
   *-----------------------------------------------------------------------*/
  {
    std::ostringstream os;
    os << "    " << fv1 << "   ";
    FV fv3;
    std::istringstream is( os.str());
    is >> fv3;
    LMBUNIT_ASSERT_EQUAL( bool(is), true);
    LMBUNIT_ASSERT_EQUAL( fv1, fv3);
  }
  
  /*-----------------------------------------------------------------------
   *  input output with arbitrary characters after
   *-----------------------------------------------------------------------*/
  {
    std::ostringstream os;
    os << fv1 << ",";
    FV fv3;
    std::istringstream is( os.str());
    is >> fv3;
    LMBUNIT_ASSERT_EQUAL( bool(is), true);
    LMBUNIT_ASSERT_EQUAL( fv1, fv3);
  }
  {
    std::ostringstream os;
    os << fv1 << "  ,";
    FV fv3;
    std::istringstream is( os.str());
    is >> fv3;
    LMBUNIT_ASSERT_EQUAL( bool(is), true);
    LMBUNIT_ASSERT_EQUAL( fv1, fv3);
  }
}

int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST( testSimple<svt::BasicFV>());
  LMBUNIT_RUN_TEST( testSimple<svt::SparseFV>());
  LMBUNIT_RUN_TEST( testInOut<svt::BasicFV>());
  LMBUNIT_RUN_TEST( testInOut<svt::SparseFV>());
  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}


