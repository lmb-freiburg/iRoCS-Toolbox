/**************************************************************************
**       Title:
**    $RCSfile$
**   $Revision: 681 $$Name$
**       $Date: 2005-03-30 13:33:37 +0200 (Wed, 30 Mar 2005) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2005/03/30 11:33:37  ronneber
**  - find training file even if compiled in a different directory
**
**  Revision 1.1  2005/03/29 18:10:08  ronneber
**  initial revision
**
**
**
**************************************************************************/

#include <iostream>
#include <fstream>
#include "lmbunit.hh"
#include <libsvmtl/SVMApplication.hh>
#include <libsvmtl/BasicFV.hh>
#include <libsvmtl/AlgorithmLists.hh>
#include <libsvmtl/DefaultMultiClassList.hh>
#include <libsvmtl/DefaultTwoClassList.hh>
#include <libsvmtl/DefaultOneClassList.hh>
#include <libsvmtl/DefaultKernelList.hh>
#include <libsvmtl/LoadSaveASCII.hh>

typedef svt::SVMApplication< svt::BasicFV,
                             svt::AlgorithmLists<

    TTLIST_1(svt::MultiClassSVMOneVsOne),
    TTLIST_1(svt::TwoClassSVMc),
    svt::DefaultOneClassList,
    TLIST_2(  svt::Kernel_SCALE< svt::Kernel_RBF>,
              svt::Kernel_MATRIX< svt::Kernel_SCALE< svt::Kernel_RBF> >)
>,
                             svt::LoadSaveASCII>   SVMApplicationMini;


static void testTrainClassifyScaled()
{
  /*-----------------------------------------------------------------------
   *  training with Klaus' spectra
   *-----------------------------------------------------------------------*/
  const char* argv[] = {"svmtl", "train",
                        "--kernel_type", "kmatrix_scaled_rbf",
                        "--scale_algorithm", "stddev",
                        "--multi_class_type", "one_vs_one",
                        "--modelfile", "svmtl_model_423_499",
                        "--gamma", "1e-06",
                        "--cost",  "2e6",
                        "--verbose_level",  "2",
                        "--draw_progress_bar", "0",
                        TOP_SRC_DIR "/test/libsvmtl/data/423_499_bireint_plain"};

  int argc = 19;
  SVMApplicationMini sa;

  int status = sa.main( argc, argv);

  LMBUNIT_ASSERT_EQUAL( status, 0);

  /*-----------------------------------------------------------------------
   *  classification of training data should result in 100% correct
   *-----------------------------------------------------------------------*/
  const char* argv2[] = {"svmtl", "classify",
                         "--modelfile", "svmtl_model_423_499",
                         "--verbose_level",  "4",
                         "--draw_progress_bar", "0",
                         "--outfile", "423_499.output",
                         TOP_SRC_DIR "/test/libsvmtl/data/423_499_bireint_plain"};
  int argc2 = 11;
  SVMApplicationMini sa2;

  status = sa2.main( argc2, argv2);

  LMBUNIT_ASSERT_EQUAL( status, 0);

  /*-----------------------------------------------------------------------
   *  compare the resulting output with input labels
   *-----------------------------------------------------------------------*/
  std::ifstream trainfile( TOP_SRC_DIR "/test/libsvmtl/data/423_499_bireint_plain");
  std::ifstream classfile( "423_499.output");
  LMBUNIT_ASSERT_EQUAL( !!trainfile, true);
  LMBUNIT_ASSERT_EQUAL( !!classfile, true);

  int trueLabel;
  int classLabel;

  while( trainfile >> trueLabel)
  {
    classfile >> classLabel;
    LMBUNIT_ASSERT_EQUAL( trueLabel, classLabel);
    trainfile.ignore(1000000, '\n'); // ignore characters until line end
  }



}




int main(int, char**)
{

  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST_NOFORK( testTrainClassifyScaled());

  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}
