/**************************************************************************
**       Title: test pretty option printer
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

#include "lmbunit.hh"
#include <libsvmtl/PrettyOptionPrinter.hh>

static void testPrettyPrint()
{
  std::ostringstream os;
  svt::PrettyOptionPrinter pop(os);
  pop.setIndentColumn( 20);
  
  pop.printDefListParagraph( "--test", "test");
  LMBUNIT_ASSERT_EQUAL( os.str(), 
                        "\033[1m--test\033[0m              test\n");

  std::ostringstream os2;
  svt::PrettyOptionPrinter pop2( os2);
  pop2.setIndentColumn( 20);
  pop2.printDefListParagraph( "--test", "this is a test");
  LMBUNIT_ASSERT_EQUAL( os2.str(), 
                        "\033[1m--test\033[0m              this is a test\n");

  std::ostringstream os3;
  svt::PrettyOptionPrinter pop3( os3);
  pop3.setIndentColumn( 20);
  pop3.printDefListParagraph( "--test", "this is a test this is a test this is a test this is a test this is a test this is a test this is a test this is a test");
  LMBUNIT_ASSERT_EQUAL( os3.str(), 
                        "\033[1m--test\033[0m              this is a test this is a test this is a test this is a\n"
                        "                    test this is a test this is a test this is a test this is\n"
                        "                    a test\n");
}


static void testStrangeError()
{
  std::vector<svt::ParamInfo> localParams;
  localParams.push_back( svt::ParamInfo("param1", "p1"));
  localParams.back().addAlternative( "<keyname>:<from>,add<step>,<to>", 
                                     "specify linear range for row"
                                     "axis. e.g., 'cost:1,add1,5' results "
                                     "in 1,2,3,4,5" );
  localParams.back().addAlternative( "<keyname>:<from>,mul<factor>,<to>", 
                                     "specify logarithmic range for row "
                                     "axis. e.g., 'gamma:1,mul2,16' results "
                                     "in 1,2,4,8,16" );
  localParams.back().addAlternative( "<keyname>:<v1>,<v2>,<v3>,...", 
                                     "specify list of values for row"
                                     "axis. e.g. 'cost:-2,5,42,3'" );

  svt::PrettyOptionPrinter pop( LMBUNIT_DEBUG_STREAM);
  pop.printOptions( "grid search", localParams);
}


int main( int argc, char** argv)
{

  LMBUNIT_WRITE_HEADER();
  testStrangeError();
  
  LMBUNIT_RUN_TEST_NOFORK( testPrettyPrint() );
  LMBUNIT_RUN_TEST_NOFORK( testStrangeError() );

  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}
