/**************************************************************************
**       Title: test svm application
**    $RCSfile$
**   $Revision: 678 $$Name$
**       $Date: 2005-03-29 20:10:22 +0200 (Tue, 29 Mar 2005) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.4  2005/03/29 18:08:42  ronneber
**  - adapted to new manual call of updateKernelCache() and clearKernelCache()
**
**  Revision 1.3  2004/09/09 14:44:30  ronneber
**  *** empty log message ***
**
**  Revision 1.2  2004/09/08 14:45:49  ronneber
**  *** empty log message ***
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


#include <iostream>
#include <fstream>
#include "lmbunit.hh"
#include <libsvmtl/SVMFactory.hh>
#include <libsvmtl/SVMApplication.hh>
#include <libsvmtl/SVMApplicationWithDefaults.hh>





static void testHelp()
{
  const char* argv[] = {"svmtl", "--help"};
  int argc = 2;
  svt::SVMApplicationWithDefaults sa;
  int status = sa.main(argc, argv, LMBUNIT_DEBUG_STREAM);
  LMBUNIT_ASSERT_EQUAL( status, 0);
  
}

static void testHelpTrain()
{
  const char* argv[] = {"svmtl", "train", "--help"};
  int argc = 3;
  svt::SVMApplicationWithDefaults sa;
  int status = sa.main(argc, argv, LMBUNIT_DEBUG_STREAM);
  LMBUNIT_ASSERT_EQUAL( status, 0);
}

static void testHelpClassify()
{
  const char* argv[] = {"svmtl", "classify", "--help"};
  int argc = 3;
  svt::SVMApplicationWithDefaults sa;
  int status = sa.main(argc, argv, LMBUNIT_DEBUG_STREAM);
  LMBUNIT_ASSERT_EQUAL( status, 0);
}

static void testHelpCrossVal()
{
  const char* argv[] = {"svmtl", "crossval", "--help"};
  int argc = 3;
  svt::SVMApplicationWithDefaults sa;
  int status = sa.main(argc, argv, LMBUNIT_DEBUG_STREAM);
  LMBUNIT_ASSERT_EQUAL( status, 0);
}


static void testHelpGridSearch()
{
  const char* argv[] = {"svmtl", "gridsearch", "--help"};
  int argc = 3;
  svt::SVMApplicationWithDefaults sa;
  int status = sa.main(argc, argv, LMBUNIT_DEBUG_STREAM);
  LMBUNIT_ASSERT_EQUAL( status, 0);
}


static void testTrain()
{
  // create training file
  std::ofstream trainFile( "/tmp/simple.train");
  trainFile << "3  0 0 0\n"
      "2  1 0 0\n"
      "1  0 1 0\n"
      "0  0 0 1\n";
  trainFile.close();
  

  const char* argv[] = {"svmtl", "train", 
                        "--verbose_level", "0",
                        "--modelfile", "/tmp/simple.model",
                        "/tmp/simple.train"};
  int argc = 7;
  svt::SVMApplicationWithDefaults sa;
  
  int status = sa.main( argc, argv);
  
  LMBUNIT_ASSERT_EQUAL( status, 0);

  // read model file
  std::ifstream modelFile( "/tmp/simple.model");
  LMBUNIT_ASSERT_EQUAL( modelFile.is_open(), true);
  
}



static void testClassify()
{
  // create test file
  std::ofstream testFile( "/tmp/simple.test");
  testFile << "-1  0 0 0\n"
      "-1  1 0 0\n"
      "-1  0 1 0\n"
      "-1  0 0 1\n";
  testFile.close();
  

  const char* argv[] = {"svmtl", "classify",
                        "--verbose_level", "0",
                        "--details", "0",
                        "--modelfile", "/tmp/simple.model",
                        "--outfile", "/tmp/simple.output",
                        "/tmp/simple.test"};
  int argc = 11;
  svt::SVMApplicationWithDefaults sa;
  
  int status = sa.main( argc, argv);
  
  LMBUNIT_ASSERT_EQUAL( status, 0);

  // read output file
  std::ifstream outputFile( "/tmp/simple.output");
  LMBUNIT_ASSERT_EQUAL( outputFile.is_open(), true);
  double label = 4711;
  outputFile >> label;
  LMBUNIT_ASSERT_EQUAL( label, 3);
  outputFile >> label;
  LMBUNIT_ASSERT_EQUAL( label, 2);
  outputFile >> label;
  LMBUNIT_ASSERT_EQUAL( label, 1);
  outputFile >> label;
  LMBUNIT_ASSERT_EQUAL( label, 0);    
}





static void testCrossValidation()
{
  // create training file
  std::ofstream trainFile( "/tmp/simple2.train");
  trainFile << 
      "3  0 0 0\n"
      "2  1 0 0\n"
      "1  0 1 0\n"
      "0  0 0 1\n"
      "3  0.1 0 0\n"
      "2  1.1 0 0\n"
      "1  0.1 1 0\n"
      "0  0.1 0 1\n";
  
  trainFile.close();
  

  const char* argv[] = {"svmtl", "crossval", 
                        "--nfold", "8", 
                        "--cost", "100", 
                        "--shuffle_subsets", "0", 
                        "--verbose_level", "0", 
                        "/tmp/simple2.train"};
  
  int argc = 11;
  svt::SVMApplicationWithDefaults sa;
  
  int status = sa.main( argc, argv, LMBUNIT_DEBUG_STREAM);
  
  LMBUNIT_ASSERT_EQUAL( status, 0);
}

static void testCrossValidationSparse()
{
  // create training file
  std::ofstream trainFile( "/tmp/simple2.sparse");
  trainFile << 
      "3  1:0 2:0 3:0\n"
      "2  1:1 2:0 3:0\n"
      "1  1:0 2:1 3:0\n"
      "0  1:0 2:0 3:1\n"
      "3  1:0.1 2:0 3:0 \n"  /* white space at line end :-) */
      "2  1:1.1 2:0 3:0\n"
      "1  1:0.1 2:1 3:0\n"
      "0  1:0.1 2:0 3:1 \n"; 
  
  trainFile.close();
  

  const char* argv[] = {"svmtl", "crossval", 
                        "--nfold", "8", 
                        "--cost", "100", 
                        "--shuffle_subsets", "0", 
                        "--verbose_level", "0", 
                        "/tmp/simple2.sparse"};
  
  int argc = 11;
  svt::SVMApplication< svt::SparseFV,
      svt::AlgorithmLists< svt::DefaultMultiClassList, 
      svt::DefaultTwoClassList,
      svt::DefaultOneClassList,
      svt::DefaultKernelList>,
      svt::LoadSaveASCII> sa;
  
  int status = sa.main( argc, argv, LMBUNIT_DEBUG_STREAM);
  
  LMBUNIT_ASSERT_EQUAL( status, 0);
}


static void testCrossValidationShortKeys()
{
  // create training file
  std::ofstream trainFile( "/tmp/simple2.train");
  trainFile << 
      "3  0 0 0\n"
      "2  1 0 0\n"
      "1  0 1 0\n"
      "0  0 0 1\n"
      "3  0.1 0 0\n"
      "2  1.1 0 0\n"
      "1  0.1 1 0\n"
      "0  0.1 0 1\n";
  
  trainFile.close();
  

  const char* argv[] = {"svmtl", "crossval", 
                        "-v", "8", 
                        "-c", "100", 
                        "-sh", "0", 
                        "-vb", "0", 
                        "/tmp/simple2.train"};
  
  int argc = 11;
  svt::SVMApplicationWithDefaults sa;
  
  int status = sa.main( argc, argv, LMBUNIT_DEBUG_STREAM);
  
  LMBUNIT_ASSERT_EQUAL( status, 0);
}

static void testGridSearch()
{
  // create training file
  std::ofstream trainFile( "/tmp/simple2.train");
  trainFile << 
      "3  0 0 0\n"
      "2  1 0 0\n"
      "1  0 1 0\n"
      "0  0 0 1\n"
      "3  0.1 0 0\n"
      "2  1.1 0 0\n"
      "1  0.1 1 0\n"
      "0  0.1 0 1\n";
  
  trainFile.close();
  

  const char* argv[] = {"svmtl", "gridsearch", 
                        "--param1", "gamma:0.1,mul2,5", 
                        "--param2", "cost:0.1,mul10,1000",
                        "--kernel_type", "rbf",
                        "--nfold", "10", 
                        "--shuffle_subsets", "0", 
                        "--verbose_level", "0", 
                        "/tmp/simple2.train"};
  
  int argc = 15;
  svt::SVMApplicationWithDefaults sa;
  
  int status = sa.main( argc, argv, LMBUNIT_DEBUG_STREAM);
  
  LMBUNIT_ASSERT_EQUAL( status, 0);
}


static void testWrongParameters()
{
  
  try 
  {
    const char* argv[] = {"svmtl", "train", "--hurz", "123", "/tmp/simple.train"};
    int argc = 5;
    svt::StDataCmdLine cmdline( argc, argv);
    svt::SVMApplicationWithDefaults sa;
    sa.main( cmdline, LMBUNIT_DEBUG_STREAM);
    LMBUNIT_WRITE_FAILURE( "wrong parameters must throw exception");
  }
  catch( svt::CmdLineError& err)
  {
    // Okay
  }
  
  try 
  {
    const char* argv[] = {"svmtl", "classify", 
                          "--hurz", "123", 
                          "--modelfile", "/tmp/simple.model",
                          "/tmp/simple.test"};
    int argc = 7;
    svt::StDataCmdLine cmdline( argc, argv);
    svt::SVMApplicationWithDefaults sa;
    sa.main( cmdline, LMBUNIT_DEBUG_STREAM);
    LMBUNIT_WRITE_FAILURE( "wrong parameters must throw exception");
  }
  catch( svt::CmdLineError& err)
  {
    // Okay
  }
  
  try 
  {
    const char* argv[] = {"svmtl", "crossval", "--hurz", "123", 
                          "/tmp/simple.train"};
    int argc = 5;
    svt::StDataCmdLine cmdline( argc, argv);
    svt::SVMApplicationWithDefaults sa;
    sa.main( cmdline, LMBUNIT_DEBUG_STREAM);
    LMBUNIT_WRITE_FAILURE( "wrong parameters must throw exception");
  }
  catch( svt::CmdLineError& err)
  {
    // Okay
  }
}


int main( int argc, char** argv)
{

  LMBUNIT_WRITE_HEADER();
  
  LMBUNIT_RUN_TEST_NOFORK( testHelp() );
  LMBUNIT_RUN_TEST_NOFORK( testHelpTrain());
  LMBUNIT_RUN_TEST_NOFORK( testHelpClassify());
  LMBUNIT_RUN_TEST_NOFORK( testHelpCrossVal());
  LMBUNIT_RUN_TEST_NOFORK( testHelpGridSearch());
  LMBUNIT_RUN_TEST_NOFORK( testTrain());
  LMBUNIT_RUN_TEST_NOFORK( testClassify());
  LMBUNIT_RUN_TEST_NOFORK( testCrossValidation());
  LMBUNIT_RUN_TEST_NOFORK( testCrossValidationSparse());
  LMBUNIT_RUN_TEST_NOFORK( testCrossValidationShortKeys());
  LMBUNIT_RUN_TEST_NOFORK( testWrongParameters());
  LMBUNIT_RUN_TEST_NOFORK( testGridSearch());
  

  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}
