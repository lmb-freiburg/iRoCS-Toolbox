/**************************************************************************
**       Title: test StDataASCIIFile class
**    $RCSfile$
**   $Revision: 533 $$Name$
**       $Date: 2004-09-09 16:44:36 +0200 (Thu, 09 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2004/09/09 14:44:34  ronneber
**  *** empty log message ***
**
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

#include <libsvmtl/StDataASCIIFile.hh>
#include <libsvmtl/BasicFV.hh>
#include <libsvmtl/SparseFV.hh>

template<typename FV>
static void testLoadSave( int compatibilityFlag, std::string sVal)
{
  svt::StDataASCIIFile param;
  double        dVal = 2.7123;
  float         fVal = 3.1415;
  unsigned int  uVal = 137;
  int           iVal = -42;
  bool          bVal = true;
  param.setValue( "sVal", sVal); 
  param.setValue( "dVal", dVal); 
  param.setValue( "fVal", fVal); 
  param.setValue( "uVal", uVal); 
  param.setValue( "iVal", iVal); 
  param.setValue( "bVal", bVal);

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
   *  save map
   *-----------------------------------------------------------------------*/
  std::ofstream outFile( "/tmp/testOutput.dat");
  param.save( outFile, compatibilityFlag);
  outFile.close();
  
  param.save( LMBUNIT_DEBUG_STREAM);
  

  /*-----------------------------------------------------------------------
   *  load map
   *-----------------------------------------------------------------------*/
  std::ifstream inFile( "/tmp/testOutput.dat");
  svt::StDataASCIIFile param2;
  param2.setExceptionFlag( true);
  param2.load( inFile, compatibilityFlag);
  
   
  /*-----------------------------------------------------------------------
   *  check values
   *-----------------------------------------------------------------------*/
  std::string   sVal2;
  double        dVal2;
  float         fVal2;
  unsigned int  uVal2;
  int           iVal2;
  bool          bVal2;

  
  param2.getValue( "sVal", sVal2); 
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

  unsigned int usize, dsize, dcsize;
  usize = param2.getArraySize("uVec");
  dsize = param2.getArraySize("dVec");
  dcsize = param2.getArraySize("dcVec");

  std::vector<unsigned int> uVec2( usize);
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

int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();
  
  LMBUNIT_RUN_TEST( testLoadSave<svt::BasicFV>( 0,"the Answer To Any Question") );
//  LMBUNIT_RUN_TEST_NOFORK( testLoadSave<svt::BasicFV>( 1,"NoSpacesInLibsvmCompatibilityMode") );
  LMBUNIT_RUN_TEST_NOFORK( testLoadSave<svt::SparseFV>( 0,"the Answer To Any Question") );
//  LMBUNIT_RUN_TEST_NOFORK( testLoadSave<svt::SparseFV>( 1,"NoSpacesInLibsvmCompatibilityMode") );

  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}

