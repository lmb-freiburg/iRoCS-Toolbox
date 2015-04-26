/**************************************************************************
**       Title: test strcutured Data for Hdf5 Files
**    $RCSfile: $
**   $Revision: $$Name:  $
**       $Date: $
**   Copyright: GPL $Author: $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: $
**
**
**************************************************************************/

#include <vector>
#include <iostream>
#include <fstream>

#include "lmbunit.hh"
#include <libsvmtl/StDataHdf5.hh>
#include <libsvmtl/BasicFV.hh>
#include <libsvmtl/SparseFV.hh>

static void testSingleValues()
{
  /*-----------------------------------------------------------------------
   *  Test Constructor
   *-----------------------------------------------------------------------*/
  svt::StDataHdf5 param( "/tmp/test1.h5", H5F_ACC_TRUNC);
  
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
  param.setValue( "sVal_strange_name", sVal); 
  param.setValue( "dVal", dVal); 
  param.setValue( "fVal", fVal); 
  param.setValue( "uVal", uVal); 
  param.setValue( "iVal", iVal); 
  param.setValue( "bVal", bVal);

  /*-----------------------------------------------------------------------
   *  close file and open it read only
   *-----------------------------------------------------------------------*/
  param.close();
  
  svt::StDataHdf5 param2( "/tmp/test1.h5");

  /*-----------------------------------------------------------------------
   *  reading values back
   *-----------------------------------------------------------------------*/
  std::string   sVal2;
  double        dVal2;
  float         fVal2;
  unsigned int  uVal2;
  int           iVal2;
  bool          bVal2;
  
  param2.getValue( "sVal", sVal2); 
  param2.getValue( "sVal_strange_name", sVal2); 
  param2.getValue( "dVal", dVal2); 
  param2.getValue( "fVal", fVal2); 
  param2.getValue( "uVal", uVal2); 
  param2.getValue( "iVal", iVal2); 
  param2.getValue( "bVal", bVal2);

  LMBUNIT_ASSERT_EQUAL( sVal, sVal2); 
  LMBUNIT_ASSERT_EQUAL( dVal, dVal2); 
  LMBUNIT_ASSERT_EQUAL( fVal, fVal2); 
  LMBUNIT_ASSERT_EQUAL( uVal, uVal2); 
  LMBUNIT_ASSERT_EQUAL( iVal, iVal2); 
  LMBUNIT_ASSERT_EQUAL( bVal, bVal2);
  
}


static void testArrays()
{
  svt::StDataHdf5 param( "/tmp/test2.h5", H5F_ACC_TRUNC);
 
  /*-----------------------------------------------------------------------
   *  write arrays with different types
   *  up to now, the library needs the following types:
   *-----------------------------------------------------------------------*/
  //64-bit can't take unsigned int 
  std::vector<int> uVec;
  std::vector<double> dVec;
  double* dcVec = new double[5];
  long* lVec = new long[5];

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

  lVec[0] = 123;
  lVec[1] = 234;
  lVec[2] = 345;
  lVec[3] = 456;
  lVec[4] = 567;
  
  std::cout << "sizeof(long) = " <<sizeof(long) << std::endl;
  std::cout << "sizeof(float) = " <<sizeof(float) << std::endl;
  
  
  param.setArray( "uVec", uVec.begin(), uVec.size());
  param.setArray( "dVec", dVec.begin(), dVec.size());
  param.setArray( "dcVec", dcVec, 5);
  param.setArray( "lVec", lVec, 5);
  
  /*-----------------------------------------------------------------------
   *  close file and open it read only
   *-----------------------------------------------------------------------*/
  param.close();
  
  svt::StDataHdf5 param2( "/tmp/test2.h5");

  /*-----------------------------------------------------------------------
   *  read values back
   *-----------------------------------------------------------------------*/
  unsigned int usize, dsize, dcsize;
  usize = param2.getArraySize("uVec");
  dsize = param2.getArraySize("dVec");
  dcsize = param2.getArraySize("dcVec");

  //64-bit can't take unsigned int
  std::vector< int> uVec2( usize);
  std::vector<double> dVec2( dsize);
  double* dcVec2 = new double[5];
  
  param2.getArray( "uVec", uVec2.begin(), uVec2.size());
  param2.getArray( "dVec", dVec2.begin(), dVec2.size());
  param2.getArray( "dcVec", dcVec2, dcsize);
   
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
  svt::StDataHdf5 param( "/tmp/test3.h5", H5F_ACC_TRUNC);
 
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
   *  close file and open it read only
   *-----------------------------------------------------------------------*/
  param.close();
  
  svt::StDataHdf5 param2( "/tmp/test3.h5");

  unsigned int size1 = param2.getFVArraySize("hurz");
  unsigned int size2 = param2.getFVArraySize("hurz42");

  LMBUNIT_ASSERT_EQUAL( nFeaVecs, size1);
  LMBUNIT_ASSERT_EQUAL( nFeaVecs, size2);
  
  std::vector<FV*> fvArr2(size1);
  FV** fvcArr2 = new FV*[size2];


  for( unsigned int i = 0; i < nFeaVecs; ++i)
  {
    fvArr2[i]  = new FV;
    fvcArr2[i] = new FV;
  }
  
  param2.getFVArray( "hurz", fvArr2.begin(), fvArr2.size());
  param2.getFVArray( "hurz42", fvcArr2, nFeaVecs);
  
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
  svt::StDataHdf5 param( "/tmp/test4.h5", H5F_ACC_TRUNC);;

  param.setValue("hurz", std::string(" keine Zahl!"));
  
  
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
  
  svt::StDataHdf5 param( "/tmp/test5.h5", H5F_ACC_TRUNC);;

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
//   das kappier ich nicht da muss doch noch ein catch all hin
  catch(...)
  {}
  
}

static void testExceptions3()
{
  svt::StDataHdf5 param( "/tmp/test6.h5", H5F_ACC_TRUNC);;

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
  svt::StDataHdf5 param( "/tmp/test7.h5", H5F_ACC_TRUNC);;
  std::vector<float> fVec;
  fVec.push_back( 5.123);
  fVec.push_back( 3.1415);
  fVec.push_back( 42);
  fVec.push_back( 137);

  param.setArray("hurz", fVec.begin(), fVec.size());
  
  try
  {
    std::vector<float> fVec2(3);
    param.getArray( "hurz", fVec2.begin(), fVec2.size());
  }
  catch( svt::ContainerTooSmallError& e)
  {
    return;
  }
  
  LMBUNIT_WRITE_FAILURE( "getArray must throw exception, if given container"
                         " is too small");
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

  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}


