/**************************************************************************
**       Title: 
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
#include <iostream>
#include <fstream>
#include <vector>

#include "lmbunit.hh"

#include <libsvmtl/StDataASCII.hh>
#include <libsvmtl/BasicFV.hh>
#include <libsvmtl/SparseFV.hh>

static void testSingleValues()
{
  /*-----------------------------------------------------------------------
   *  Test Constructor
   *-----------------------------------------------------------------------*/
  svt::StDataASCII param;
  
  /*-----------------------------------------------------------------------
   *  check different types 
   *  up to now, the library needs the following types:
   *-----------------------------------------------------------------------*/
  std::string   sVal = "the Answer To Any Question";
  double        dVal = 2.7123;
  float         fVal = 3.1415;
  unsigned int  uVal = 137;
  int           iVal = -42;
  bool          bVal = true;
 

  /*-----------------------------------------------------------------------
   *  Store them in the map
   *-----------------------------------------------------------------------*/
  param.setValue( "sVal", sVal); 
  param.setValue( "dVal", dVal); 
  param.setValue( "fVal", fVal); 
  param.setValue( "uVal", uVal); 
  param.setValue( "iVal", iVal); 
  param.setValue( "bVal", bVal);

  /*-----------------------------------------------------------------------
   *  reading values back
   *-----------------------------------------------------------------------*/
  std::string   sVal2;
  double        dVal2;
  float         fVal2;
  unsigned int  uVal2;
  int           iVal2;
  bool          bVal2;

  
  param.getValue( "sVal", sVal2); 
  param.getValue( "dVal", dVal2); 
  param.getValue( "fVal", fVal2); 
  param.getValue( "uVal", uVal2); 
  param.getValue( "iVal", iVal2); 
  param.getValue( "bVal", bVal2);

  LMBUNIT_ASSERT_EQUAL( sVal, sVal2); 
  LMBUNIT_ASSERT_EQUAL( dVal, dVal2); 
  LMBUNIT_ASSERT_EQUAL( fVal, fVal2); 
  LMBUNIT_ASSERT_EQUAL( uVal, uVal2); 
  LMBUNIT_ASSERT_EQUAL( iVal, iVal2); 
  LMBUNIT_ASSERT_EQUAL( bVal, bVal2);
  
}


static void testArrays()
{
  svt::StDataASCII param;
 
  /*-----------------------------------------------------------------------
   *  write arrays with different types
   *  up to now, the library needs the following types:
   *-----------------------------------------------------------------------*/
  std::vector<unsigned int> uVec;
  std::vector<double> dVec;
  double* dcVec = new double[5];

  uVec.push_back( 5);
  uVec.push_back( 3);
  uVec.push_back( 42);
  uVec.push_back( 137);
  
  dVec.push_back( 5.123);
  dVec.push_back( 3.1415);
  dVec.push_back( 42);
  dVec.push_back( 137);

  dcVec[0] = 5.123;
  dcVec[1] = 3.1415;
  dcVec[2] = 42;
  dcVec[3] = 137;
  dcVec[4] = 4711;
  
  
  param.setArray( "uVec", uVec.begin(), uVec.size());
  param.setArray( "dVec", dVec.begin(), dVec.size());
  param.setArray( "dcVec", dcVec, 5);
  
  /*-----------------------------------------------------------------------
   *  read values back
   *-----------------------------------------------------------------------*/
  unsigned int usize, dsize, dcsize;
  usize = param.getArraySize("uVec");
  dsize = param.getArraySize("dVec");
  dcsize = param.getArraySize("dcVec");

  std::vector<unsigned int> uVec2( usize);
  std::vector<double> dVec2( dsize);
  double* dcVec2 = new double[5];
  
  param.getArray( "uVec", uVec2.begin(), uVec2.size());
  param.getArray( "dVec", dVec2.begin(), dVec2.size());
  param.getArray( "dcVec", dcVec2, dcsize);
   
  LMBUNIT_ASSERT_EQUAL( uVec[0], uVec2[0]);
  LMBUNIT_ASSERT_EQUAL( uVec[1], uVec2[1]);
  LMBUNIT_ASSERT_EQUAL( uVec[2], uVec2[2]);
  LMBUNIT_ASSERT_EQUAL( uVec[3], uVec2[3]);

  LMBUNIT_ASSERT_EQUAL( dVec[0], dVec2[0]);
  LMBUNIT_ASSERT_EQUAL( dVec[1], dVec2[1]);
  LMBUNIT_ASSERT_EQUAL( dVec[2], dVec2[2]);
  LMBUNIT_ASSERT_EQUAL( dVec[3], dVec2[3]);

  LMBUNIT_ASSERT_EQUAL( dcVec[0], dcVec2[0]);
  LMBUNIT_ASSERT_EQUAL( dcVec[1], dcVec2[1]);
  LMBUNIT_ASSERT_EQUAL( dcVec[2], dcVec2[2]);
  LMBUNIT_ASSERT_EQUAL( dcVec[3], dcVec2[3]);
  LMBUNIT_ASSERT_EQUAL( dcVec[4], dcVec2[4]);

}

template< typename FV>
static void testFVArrays()
{
  svt::StDataASCII param;
 
  /*-----------------------------------------------------------------------
   *  write arrays with different types
   *  up to now, the library needs the following types:
   *-----------------------------------------------------------------------*/
  unsigned int nFeaVecs = 42;
  unsigned int nComponents = 17;
  std::vector<FV*> fvArr(nFeaVecs);
  FV** fvcArr = new FV*[nFeaVecs];

  for( unsigned int i = 0; i < nFeaVecs; ++i)
  {
    FV* fv1 = new FV;
    fv1->resize( nComponents);
    FV* fv2 = new FV;
    fv2->resize( nComponents);
    for( unsigned int j = 0; j < nComponents; ++j)
    {
      (*fv1)[j] = double(std::rand())/RAND_MAX;
      (*fv2)[j] = double(std::rand())/RAND_MAX;
    }
    fvArr[i]  = fv1;
    fvcArr[i] = fv2;
  }
  
  param.setFVArray( "hurz", fvArr.begin(), fvArr.size());
  param.setFVArray( "hurz42", fvcArr, nFeaVecs);
   
  /*-----------------------------------------------------------------------
   *  read values back
   *-----------------------------------------------------------------------*/
  unsigned int size1 = param.getFVArraySize("hurz");
  unsigned int size2 = param.getFVArraySize("hurz42");

  LMBUNIT_ASSERT_EQUAL( nFeaVecs, size1);
  LMBUNIT_ASSERT_EQUAL( nFeaVecs, size2);
  
  std::vector<FV*> fvArr2(size1);
  FV** fvcArr2 = new FV*[size2];


  for( unsigned int i = 0; i < nFeaVecs; ++i)
  {
    fvArr2[i]  = new FV;
    fvcArr2[i] = new FV;
  }
  
  param.getFVArray( "hurz", fvArr2.begin(), fvArr2.size());
  param.getFVArray( "hurz42", fvcArr2, nFeaVecs);
  
  for( unsigned int i = 0; i < nFeaVecs; ++i)
  {
    for( unsigned int j = 0; j < nComponents; ++j)
    {
      LMBUNIT_ASSERT_EQUAL_DELTA( (*(fvArr[i]))[j], (*(fvArr2[i]))[j], 0.000001);
      LMBUNIT_ASSERT_EQUAL_DELTA( (*(fvcArr[i]))[j], (*(fvcArr2[i]))[j], 0.000001);
    }
  }
}


static void testExceptions()
{
  svt::StDataASCII param;

  param.setValue("hurz", " keine Zahl!");
  
  
  try
  {
    float f = 42;
    param.getValue( "hurz", f);
  }
  catch( svt::InvalidDataTypeError& e)
  {
    return;
  }
  
  
  LMBUNIT_WRITE_FAILURE( "' keine Zahl!' interpreted as "
                         "float must throw exception!");
}

static void testExceptions2()
{
  
  svt::StDataASCII param;

  param.setValue("hurz", 42);
  
  try
  {
    float f = 42;
    param.getValue( "xyz", f);
  }
  catch( svt::KeyNotFoundError& e)
  {
      LMBUNIT_WRITE_FAILURE( "unknown key must not throw exception, "
                             "if exceptionFlag is set to false");
  }
  
}

static void testExceptions3()
{
  svt::StDataASCII param;

  param.setValue("hurz", 42);
  
  param.setExceptionFlag( true);

  try
  {
    float f = 42;
    param.getValue( "xyz", f);
  }
  catch( svt::KeyNotFoundError& e)
  {
    return;
  }
  
  LMBUNIT_WRITE_FAILURE( "unknown key must throw exception, "
                         "if exceptionFlag is set to true");
}

static void testExceptions4()
{
  svt::StDataASCII param;
  std::vector<double> fVec;
  fVec.push_back( 5.123);
  fVec.push_back( 3.1415);
  fVec.push_back( 42);
  fVec.push_back( 137);

  param.setArray("hurz", fVec.begin(), fVec.size());
  
  try
  {
    std::vector<double> fVec2(3);
    param.getArray( "hurz", fVec2.begin(), fVec2.size());
  }
  catch( svt::ContainerTooSmallError& e)
  {
    return;
  }
  
  LMBUNIT_WRITE_FAILURE( "getArray must throw exception, if given container"
                         " is too small");
}


static void testValueExists()
{
  svt::StDataASCII stdata;
  stdata.setValue( "hurz", 1234);
  LMBUNIT_ASSERT_EQUAL( stdata.valueExists( "hurz"), true);
  LMBUNIT_ASSERT_EQUAL( stdata.valueExists( "schnurz"), false);
}






int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();
  
  LMBUNIT_RUN_TEST_NOFORK( testSingleValues() );
  LMBUNIT_RUN_TEST_NOFORK( testArrays() );
  LMBUNIT_RUN_TEST_NOFORK( testFVArrays<svt::BasicFV>() );
  LMBUNIT_RUN_TEST_NOFORK( testFVArrays<svt::SparseFV>() );
  LMBUNIT_RUN_TEST_NOFORK( testExceptions() );
  LMBUNIT_RUN_TEST_NOFORK( testExceptions2() );
  LMBUNIT_RUN_TEST_NOFORK( testExceptions3() );
  LMBUNIT_RUN_TEST_NOFORK( testExceptions4() );
  LMBUNIT_RUN_TEST( testValueExists());
  
  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}


