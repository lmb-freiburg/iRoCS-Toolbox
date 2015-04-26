/**************************************************************************
**       Title: test strcutured data container for commandline arguments
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
**  Revision 1.2  2004/09/08 14:45:49  ronneber
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
#include "lmbunit.hh"

#include <libsvmtl/StDataCmdLine.hh>


static void testParse1()
{
  const char* argv[] = {"svmtl", "train", "simple.train"};
  int argc = 3;
  svt::StDataCmdLine params;
  params.parseCommandline( argc, argv);
  
  LMBUNIT_ASSERT_EQUAL( params.mode(), "train");
  LMBUNIT_ASSERT_EQUAL( params.filenames().size(), 1);
  LMBUNIT_ASSERT_EQUAL( params.filenames()[0], "simple.train");
  
}

static void testParse2()
{
  const char* argv[] = {
      "svmtl", "classify",
      "--gamma", "123",
      "--coef0", "42",
      "--kalle", "blomquist",
      "infile1",
      "infile2",
      "outfile"};
  int argc = 11;
  svt::StDataCmdLine params;
  params.parseCommandline( argc, argv);
  
  LMBUNIT_ASSERT_EQUAL( params.mode(), "classify");
  LMBUNIT_ASSERT_EQUAL( params.filenames().size(), 3);
  LMBUNIT_ASSERT_EQUAL( params.filenames()[0], "infile1");
  LMBUNIT_ASSERT_EQUAL( params.filenames()[1], "infile2");
  LMBUNIT_ASSERT_EQUAL( params.filenames()[2], "outfile");
  std::string val;
  params.getValue( "gamma", val);
  LMBUNIT_ASSERT_EQUAL( val, "123");
  params.getValue( "coef0", val);
  LMBUNIT_ASSERT_EQUAL( val, "42");
  params.getValue( "kalle", val);
  LMBUNIT_ASSERT_EQUAL( val, "blomquist");
  
}


static void testUnusedParameters()
{
  const char* argv[] = {
      "svmtl", "classify",
      "--gamma", "123",
      "--coef0", "42",
      "--kelle", "blomquist",
      "infile1",
      "infile2",
      "outfile"};

  int argc = 11;
  svt::StDataCmdLine params;
  params.parseCommandline( argc, argv);

  std::string val;
  params.getValue( "gamma", val);
  params.getValue( "coef0", val);
  params.getValue( "kalle", val);


  LMBUNIT_ASSERT_EQUAL( params.keyOccurenceMap().size(), 3);
  

  std::vector<std::string> unusedParameters;
  params.findUnusedParameters( unusedParameters);
  
  LMBUNIT_ASSERT_EQUAL( unusedParameters.size(), 1);
  LMBUNIT_ASSERT_EQUAL( unusedParameters[0], "kelle");
  
}

static void testHelp()
{
  const char* argv[] = {
      "svmtl",
      "--help"};
  int argc = 2;
  svt::StDataCmdLine params;
  params.parseCommandline( argc, argv);

  LMBUNIT_ASSERT_EQUAL( params.mode(), "");
  LMBUNIT_ASSERT_EQUAL( params.helpRequested(), true);

  
}

static void testHelp2()
{
  const char* argv[] = {
      "svmtl",
      "train",
      "--help"};
  int argc = 3;
  svt::StDataCmdLine params;
  params.parseCommandline( argc, argv);

  LMBUNIT_ASSERT_EQUAL( params.mode(), "train");
  LMBUNIT_ASSERT_EQUAL( params.helpRequested(), true);  
}

static void testShortCutTable()
{
  svt::StDataCmdLine cmdline;
  
  const char* argv[] = {
      "svmtl", "train",
      "-g", "1",
      "-c", "2",
      "-e", "3",
      "-tc", "c_svc" };
  int argc = 10;
  cmdline.parseCommandline( argc, argv);
  
  std::vector<svt::ParamInfo> params;
  params.push_back( svt::ParamInfo( "gamma", "g", "value", "hurz"));
  params.push_back( svt::ParamInfo( "coef0", "c", "value", "hurz"));
  params.push_back( svt::ParamInfo( "epsilon", "e", "value", "hurz"));
  params.push_back( svt::ParamInfo( "two_class_type", "tc", "value", "hurz"));
  cmdline.updateShortCutTable( params);
  cmdline.debugPrintShortCutTable(LMBUNIT_DEBUG_STREAM);
  cmdline.translateShortKeys();
  


  LMBUNIT_ASSERT_EQUAL( cmdline.asString( "gamma"), "1");
  LMBUNIT_ASSERT_EQUAL( cmdline.asString( "coef0"), "2");
  LMBUNIT_ASSERT_EQUAL( cmdline.asString( "epsilon"), "3");
  LMBUNIT_ASSERT_EQUAL( cmdline.asString( "two_class_type"), "c_svc");
}

  
static void testShortCutTableException1()
{
  /*---------------------------------------------------------------------
   *  must not throw exception if same shortKey-->longKey
   *---------------------------------------------------------------------*/
  svt::StDataCmdLine cmdline;

  const char* argv[] = {
      "svmtl", "train",
      "-c", "1"};
  int argc = 4;
  cmdline.parseCommandline( argc, argv);

  std::vector<svt::ParamInfo> params;
  params.push_back( svt::ParamInfo( "gamma", "g", "value", "hurz"));
  params.push_back( svt::ParamInfo( "coef0", "c", "value", "hurz"));
  params.push_back( svt::ParamInfo( "epsilon", "e", "value", "hurz"));
  params.push_back( svt::ParamInfo( "coef0", "c", "value", "hurz"));
  cmdline.updateShortCutTable( params);
  cmdline.debugPrintShortCutTable(LMBUNIT_DEBUG_STREAM);
  cmdline.translateShortKeys();
  
}


static void testShortCutTableException2()
{
  try
  {
    /*---------------------------------------------------------------------
     *  must throw exception if ambioug shortKey-->longKey mapping is
     *  applied and short key is used
     *---------------------------------------------------------------------*/
    svt::StDataCmdLine cmdline;
    const char* argv[] = {
        "svmtl", "train",
        "-c", "1"};
    int argc = 4;
    cmdline.parseCommandline( argc, argv);

    std::vector<svt::ParamInfo> params;
    params.push_back( svt::ParamInfo( "gamma", "g", "value", "hurz"));
    params.push_back( svt::ParamInfo( "coef0", "c", "value", "hurz"));
    params.push_back( svt::ParamInfo( "cost", "c", "value", "hurz"));
    
    cmdline.updateShortCutTable( params);
    cmdline.debugPrintShortCutTable(LMBUNIT_DEBUG_STREAM);
    cmdline.translateShortKeys();

    LMBUNIT_WRITE_FAILURE( "expected exception 'svt::AmbiguousShortKeyError'");
  }
  catch( svt::ParseCmdLineError& err)
  {
    return;
  }
}





int main( int argc, char** argv)
{

  LMBUNIT_WRITE_HEADER();
  
  LMBUNIT_RUN_TEST( testParse1());
  LMBUNIT_RUN_TEST( testParse2());
  LMBUNIT_RUN_TEST( testUnusedParameters());
  LMBUNIT_RUN_TEST( testHelp() );
  LMBUNIT_RUN_TEST( testHelp2() );
  LMBUNIT_RUN_TEST( testShortCutTable() );
  LMBUNIT_RUN_TEST( testShortCutTableException1() );
  LMBUNIT_RUN_TEST( testShortCutTableException2() );
  

  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}
