/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 492 $$Name$
**       $Date: 2004-09-01 16:45:13 +0200 (Wed, 01 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2004/09/01 14:45:13  ronneber
**  changed IterToPointerTraits stuff to
**  DirectAccessor and DereferencingAccessor, to make code more
**  intuitive understandable
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.1  2003/05/19 11:38:11  ronneber
**  - initial revision
**
**
**
**************************************************************************/


#include "lmbunit.hh"
#include <libsvmtl/BasicFV.hh>
#include <libsvmtl/Kernel_LINEAR.hh>
#include <libsvmtl/Kernel_RBF.hh>
#include <libsvmtl/TwoClassSVMc.hh>
#include <libsvmtl/TwoClassSVMnu.hh>
#include <libsvmtl/MultiClassSVMOneVsOne.hh>
#include <libsvmtl/TriangularMatrix.hh>
#include <libsvmtl/MultiClassSVMOneVsRest.hh>
#include <libsvmtl/DirectAccessor.hh>
#include <libsvmtl/DereferencingAccessor.hh>

template< typename FV>
void _fillFV( FV& fv, int label, double f0, double f1, double f2)
{
  fv.resize( 3);
  fv.setLabel( label);
  fv[0] = f0;
  fv[1] = f1;
  fv[2] = f2;
}

static void testDereferencingAccessorTwoClassSVMc()
{
  std::vector<svt::BasicFV> featureVectors(2);

  _fillFV( featureVectors[0], -1,  0, 0, 0);
  _fillFV( featureVectors[1], +1,  1, 0, 0);

  /*-----------------------------------------------------------------------
   *  setup a vector with pointers to feature vectors
   *-----------------------------------------------------------------------*/
  std::vector<svt::BasicFV*> featureVectorPointers(2);
  for( int i = 0; i < 2; ++i)
  {
    featureVectorPointers[i] = &(featureVectors[i]);
  }
  
  svt::TwoClassSVMc< svt::Kernel_LINEAR> svm;
  
  svt::Model<svt::BasicFV> model;

  svm.setCost( 100);

  svm.train( featureVectorPointers.begin(), 
             featureVectorPointers.end(), 
             model,
             svt::DereferencingAccessor());

  LMBUNIT_ASSERT_EQUAL( model.size(), 2);
  LMBUNIT_ASSERT_EQUAL_DELTA( svm.classify( featureVectors[0], model), -1,
                              0.000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( svm.classify( featureVectors[1], model), +1,
                              0.000001);
  
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("iterations"), 1);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nu"),         0.02);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("cost"),       100);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("obj"),        -2);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nSV"),        2);
  LMBUNIT_ASSERT_EQUAL( model.getTrainingInfoValue("nBSV"),       0);
 

}



static void testDereferencingAccessorOneVsOne()
{
  std::vector<svt::BasicFV> featureVectors(4);

  _fillFV( featureVectors[0], 3,  0, 0, 0);
  _fillFV( featureVectors[1], 2,  1, 0, 0);
  _fillFV( featureVectors[2], 1,  0, 1, 0);
  _fillFV( featureVectors[3], 0,  0, 0, 1);
  adjustUniqueIDs( featureVectors);

  /*-----------------------------------------------------------------------
   *  setup a vector with pointers to feature vectors
   *-----------------------------------------------------------------------*/
  std::vector<svt::BasicFV*> featureVectorPointers(4);
  for( int i = 0; i < 4; ++i)
  {
    featureVectorPointers[i] = &(featureVectors[i]);
  }
  

  svt::MultiClassSVMOneVsOne< svt::TwoClassSVMc< svt::Kernel_LINEAR> > svm;
  svm.twoClassSVM().setCost( 100);
  
      
  svt::Model_MC_OneVsOne< svt::Model<svt::BasicFV> > mcModel;

  
  svm.train( featureVectorPointers.begin(), 
             featureVectorPointers.end(), 
             mcModel,
             svt::DereferencingAccessor());

  /*-----------------------------------------------------------------------
   *  print training statistics
   *-----------------------------------------------------------------------*/
  for( int i = 0; i < 4; ++i)
  {
    for( int j = i+1; j < 4; ++j)
    {
      LMBUNIT_DEBUG_STREAM << "training statistics for classes " << i << " vs. " << j
                << std::endl;
      LMBUNIT_DEBUG_STREAM << mcModel.twoClassModel(i,j).trainingInfoPlainText();
    }
  }
  
  
  svt::TriangularMatrix<double> resultMatrix(4);

  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[0], mcModel, resultMatrix), 3);
#if 0
  LMBUNIT_DEBUG_STREAM << std::endl;
  for( int row = 0; row < 3; ++row)
  {
    for( int indent = 0; indent < row; ++indent)
    {
      LMBUNIT_DEBUG_STREAM << "\t";
    }
    
    for( int col = row+1; col < 4; ++col)
    {
      LMBUNIT_DEBUG_STREAM << resultMatrix(row,col) << "\t";
    }
    LMBUNIT_DEBUG_STREAM << std::endl;
  }
#endif
  
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[1], mcModel), 2);
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[2], mcModel), 1);
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[3], mcModel), 0);
}


static void testDereferencingAccessorOneVsRest()
{
  std::vector<svt::BasicFV> featureVectors(4);

  _fillFV( featureVectors[0], 3,  0, 0, 0);
  _fillFV( featureVectors[1], 2,  1, 0, 0);
  _fillFV( featureVectors[2], 1,  0, 1, 0);
  _fillFV( featureVectors[3], 0,  0, 0, 1);
  adjustUniqueIDs( featureVectors);

  /*-----------------------------------------------------------------------
   *  setup a vector with pointers to feature vectors
   *-----------------------------------------------------------------------*/
  std::vector<svt::BasicFV*> featureVectorPointers(4);
  for( int i = 0; i < 4; ++i)
  {
    featureVectorPointers[i] = &(featureVectors[i]);
  }
  

  svt::MultiClassSVMOneVsRest< svt::TwoClassSVMc< svt::Kernel_LINEAR> > svm;
  svm.twoClassSVM().setCost( 100);
  
      
  svt::Model_MC_OneVsRest< svt::Model<svt::BasicFV> > mcModel;

  
  svm.train( featureVectorPointers.begin(), 
             featureVectorPointers.end(), mcModel,
             svt::DereferencingAccessor());

  
  std::vector<double> resultVector(4);

  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[0], mcModel, resultVector), 3);
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[1], mcModel), 2);
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[2], mcModel), 1);
  LMBUNIT_ASSERT_EQUAL( svm.classify( featureVectors[3], mcModel), 0);
}


  
int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();
  LMBUNIT_RUN_TEST( testDereferencingAccessorTwoClassSVMc() );
  LMBUNIT_RUN_TEST( testDereferencingAccessorOneVsOne() );
  LMBUNIT_RUN_TEST( testDereferencingAccessorOneVsRest() );

  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}


