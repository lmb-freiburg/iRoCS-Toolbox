/**************************************************************************
**       Title: test grid search class
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
**  Revision 1.3  2004/09/08 14:45:28  ronneber
**  *** empty log message ***
**
**  Revision 1.2  2004/09/03 09:14:56  ronneber
**  *** empty log message ***
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#include <libsvmtl/MultiClassSVMOneVsRest.hh>
#include <libsvmtl/MultiClassSVMOneVsOne.hh>
#include <libsvmtl/TwoClassSVMc.hh>
#include <libsvmtl/Kernel_RBF.hh>
#include <libsvmtl/Kernel_MATRIX.hh>
#include <libsvmtl/CrossValidator.hh>

#include <libsvmtl/GridAxis.hh>
#include <libsvmtl/GridSearch.hh>
#include <libsvmtl/DirectAccessor.hh>

#include "lmbunit.hh"

static void testSearch2D()
{
  /*-----------------------------------------------------------------------
   *  create CrossValidator
   *-----------------------------------------------------------------------*/
  unsigned int nClasses = 5;
  unsigned int nFeatureVectorsPerClass = 10;
  unsigned int nComponents = 100;
  
  // create random feature vectors 
  std::vector<svt::BasicFV> featureVectors(nFeatureVectorsPerClass * nClasses);
  
  for( unsigned int i = 0; i < featureVectors.size(); ++i)
  {
    int label = i % nClasses;
    
    svt::BasicFV& fv = featureVectors[i];
    
    fv.resize( nComponents);
    fv.setLabel( label);
    fv[0] = label*3 + double(std::rand())/RAND_MAX;  // in feature 0 all
                                                        // classes are
                                                        // well
                                                        // separated 
    for( unsigned int j = 1; j < nComponents; ++j)
    {
      fv[j] = double(std::rand())/RAND_MAX;
    }
  }
  svt::adjustUniqueIDs( featureVectors);
  svt::CrossValidator<svt::BasicFV, svt::MultiClassSVMOneVsOne< svt::TwoClassSVMc< svt::Kernel_MATRIX< svt::Kernel_RBF> > > > cv;

  svt::ProgressReporter pr;
  pr.setVerboseLevel(0);
  
  cv.setProgressReporter( &pr);
  
  svt::GroupedTrainingData<svt::BasicFV> trainData( featureVectors.begin(),
                                                    featureVectors.end(),
                                                    svt::DirectAccessor());
  cv.setTrainingData( &trainData);

  std::vector<int> subsetIndexByUID;
  svt::generateSortedSubsets( featureVectors.size(), 5, subsetIndexByUID);
  
  /*-----------------------------------------------------------------------
   *  now do grid search with this cross validator
   *-----------------------------------------------------------------------*/
  svt::GridAxis row( "gamma:0.1,mul2, 5");
  row.setChangesKernel( true);
  svt::GridAxis col( "cost:0.1,mul10, 1000");
  col.setChangesKernel( false);
  std::vector<svt::StDataASCII> gridPointInfos;
  svt::GridSearch grid;

  grid.setProgressReporter( &pr);
  unsigned int bestGridPointIndex;
  std::vector<svt::SingleClassResult> bestResultTable;
  
  grid.search2D( row, col, &cv, subsetIndexByUID, 
                 gridPointInfos, bestGridPointIndex,
                 bestResultTable);
  
  unsigned int nExpectedGridPoints = row.nValues() * col.nValues();
  
  LMBUNIT_ASSERT_EQUAL( gridPointInfos.size(), nExpectedGridPoints);

  for( unsigned int r = 0; r < row.nValues(); ++r)
  {
    for( unsigned int c = 0; c < col.nValues(); ++c)
    {
      int index = r * col.nValues() + c;
      LMBUNIT_DEBUG_STREAM << gridPointInfos[index].asDouble("nCorrect") 
                           << "\t";
    }
    LMBUNIT_DEBUG_STREAM << std::endl;
  }
  
  for( unsigned int r = 0; r < row.nValues(); ++r)
  {
    for( unsigned int c = 0; c < col.nValues(); ++c)
    {
      int index = r * col.nValues() + c;
      LMBUNIT_DEBUG_STREAM << "-------------------------" << std::endl;
      gridPointInfos[index].debugPrint(LMBUNIT_DEBUG_STREAM);
      
    }
  }
  


  // for small gammas and any costs, the classification should be perfect
  int nCorrectClassifications = -1;
  gridPointInfos[0].getValue( "nCorrect", nCorrectClassifications);
  
  LMBUNIT_ASSERT_EQUAL( nCorrectClassifications, int(nClasses * nFeatureVectorsPerClass));
  
  
}


static void testException()
{
  try
  {
    svt::GridAxis row( "cost:1,mul10, 1000");
    row.setChangesKernel( false);
    svt::GridAxis col( "gamma:0.1,mul2, 5");
    col.setChangesKernel( true);
    svt::CrossValidator<svt::BasicFV, svt::MultiClassSVMOneVsRest< svt::TwoClassSVMc< svt::Kernel_MATRIX< svt::Kernel_RBF> > > > cv;
    std::vector<int> subsetIndexByUID;
    std::vector<svt::StDataASCII> gridPointInfos;
    svt::GridSearch grid;
    unsigned int bestGridPointIndex;
    std::vector<svt::SingleClassResult> bestResultTable;
    grid.search2D( row, col, &cv, subsetIndexByUID, 
                   gridPointInfos, bestGridPointIndex,
                   bestResultTable);
  }
  catch( svt::GridSearchError& err)
  {
    return;
  }
  
  
  LMBUNIT_WRITE_FAILURE( "Invalid combination of changesKernel in row and column must throw exception");
}

  


int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();
  LMBUNIT_RUN_TEST( testSearch2D() );
  LMBUNIT_RUN_TEST_NOFORK( testException() );
  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}


