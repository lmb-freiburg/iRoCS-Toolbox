/**************************************************************************
**       Title: test the TriangularMatrix class
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
**  Revision 1.2  2002/05/14 10:22:23  ronneber
**  - adapted to new TriangularMatrix (now using upper triangular matrix)
**
**  Revision 1.1  2002/05/06 13:47:29  ronneber
**  initial revision
**
**
**
**************************************************************************/

#include <cstdlib>
#include "lmbunit.hh"
#include <libsvmtl/TriangularMatrix.hh>


static void testConstructor()
{
  svt::TriangularMatrix<int> matrix(5);
}

static void testElementAccess()
{
  svt::TriangularMatrix<int> matrix(4);

  LMBUNIT_ASSERT_EQUAL( matrix.nElements(), 6);
  LMBUNIT_ASSERT_EQUAL( matrix.width(), 4);
  
  matrix(0,1) = 42;
  matrix(0,2) = 43;
  matrix(0,3) = 44;
  matrix(1,2) = 45;
  matrix(1,3) = 46;
  matrix(2,3) = 47;


  LMBUNIT_ASSERT_EQUAL( matrix[0], 42);
  LMBUNIT_ASSERT_EQUAL( matrix[1], 43);
  LMBUNIT_ASSERT_EQUAL( matrix[2], 44);
  LMBUNIT_ASSERT_EQUAL( matrix[3], 45);
  LMBUNIT_ASSERT_EQUAL( matrix[4], 46);
  LMBUNIT_ASSERT_EQUAL( matrix[5], 47);
  

}

static void testElementAccess2()
{
  svt::TriangularMatrix<int> matrix(5);

  LMBUNIT_ASSERT_EQUAL( matrix.nElements(), 10);
  LMBUNIT_ASSERT_EQUAL( matrix.width(), 5);
  
  matrix(0,1) = 42;
  matrix(0,2) = 43;
  matrix(0,3) = 44;
  matrix(0,4) = 45;
  matrix(1,2) = 46;
  matrix(1,3) = 47;
  matrix(1,4) = 48;
  matrix(2,3) = 49;
  matrix(2,4) = 50;
  matrix(3,4) = 51;


  LMBUNIT_ASSERT_EQUAL( matrix[0], 42);
  LMBUNIT_ASSERT_EQUAL( matrix[1], 43);
  LMBUNIT_ASSERT_EQUAL( matrix[2], 44);
  LMBUNIT_ASSERT_EQUAL( matrix[3], 45);
  LMBUNIT_ASSERT_EQUAL( matrix[4], 46);
  LMBUNIT_ASSERT_EQUAL( matrix[5], 47);
  LMBUNIT_ASSERT_EQUAL( matrix[6], 48);
  LMBUNIT_ASSERT_EQUAL( matrix[7], 49);
  LMBUNIT_ASSERT_EQUAL( matrix[8], 50);
  LMBUNIT_ASSERT_EQUAL( matrix[9], 51);
  

}




int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST( testConstructor() );
  LMBUNIT_RUN_TEST( testElementAccess() );
  LMBUNIT_RUN_TEST( testElementAccess2() );

  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}
