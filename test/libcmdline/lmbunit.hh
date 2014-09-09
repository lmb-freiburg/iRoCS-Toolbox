/**************************************************************************
**       Title: simple test suite framework
**    $RCSfile$
**   $Revision: 54 $$Name$
**       $Date: 2002-04-11 10:44:23 +0200 (Thu, 11 Apr 2002) $
**   Copyright: LGPL $Author: ronneber $
** Description:
**//*!
**  \mainpage lmbunit: Test suite for C++
**  \section intro Introduction
**  "lmbunit" defines some macros to write simple but powerful
**  test suites for your classes (refer to "Extreme Programming" docs,
**  e.g. http://www.extremeprogramming.org, if you don't know, how and why
**  to test).  
**  
**  "lmbunit" offers nearly the same functionality like CppUnit (http://cppunit.sourceforge.net/), but it is
**  much more simple to use and understand, and the output is designed to
**  be interpreted within emacs 'M-x compile' buffer. This allows you to
**  jump directly to the source code line of the failed test, just by
**  clicking with the middle mouse button onto the failure message.
**  
**  \section install Installation 
**  Just copy the file lmbunit.hh somewhere
**  into your source-tree and deliver it with your source-code. So anyone
**  who uses your sources may immediately run your tests, without having
**  to install an extra library like CppUnit.
**  
**  \section doc Documentation
**  All Macros are documented (with examples) in lmbunit.hh
**
**  \section usage Usage
**  Each Test suite becomes an individual .cc file with an own main
**  funcition, wherein each test is a small 'static' function. A simple
**  example for testing your 'MyComplex' class may look like this (testMyComplex.cc)
**  \code
**  // example test for MyComplex class
**  //
**  #include "lmbunit.hh"
**  #include "MyComplex.hh"
**  
**  // test if Constructor works
**  //
**  static void testConstructor()
**  {
**    MyComplex a;
**    LMBUNIT_ASSERT( a.imag() == 0);
**  }
**  
**  // test if integer addition works
**  // 
**  static void testIntegerAddition()
**  {
**    MyComplex a( 21, 0);
**    MyComplex b( 42, 0);
**    LMBUNIT_ASSERT_EQUAL( a+a, b);
**  }
**  
**  // main programm calling all tests and writing statistics
**  // 
**  int main( int argc, char** argv)
**  {
**    LMBUNIT_WRITE_HEADER( std::cout);
**    LMBUNIT_RUN_TEST( testConstructor() );
**    LMBUNIT_RUN_TEST( testIntegerAddition());
**    LMBUNIT_WRITE_STATISTICS( std::cout);
**  
**    return _nFails;
**  }
**  \endcode
**
**  The output of this program (for an incomplete MyComplex class of
course) is the following
**  \verbatim
-------------------------------------------
 Running Test Suite "testMyComplex.cc"

testMyComplex.cc:11: testConstructor(): assertion 'a.imag() == 0' failed
testMyComplex.cc:20: testIntegerAddition(): assertion 'a+a == b' failed, because 'a+a' is '(21,0)' and 'b' is '(42,0)'

 number of tests/failures:     2/2
--------------------------------------------\endverbatim
**  \section further Further Information
**  For a complete example
**  and new versions have a look to lmbunit's homepage at
**   http://lmb.informatik.uni-freiburg.de/lmbsoft/lmbunit
**/  
/**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2002/04/11 08:44:23  ronneber
**  - local copy of lmbunit (to make "check" working everywhere)
**
**  Revision 1.2  2002/03/19 09:31:20  ronneber
**  - now LMBUNIT_RUN_TEST() uses fork() to be robust against segmentation
**    faults and other bad things in the test units. The method without fork
**    is called LMBUNIT_RUN_TEST_NOFORK()
**  - uses std::cout everywhere (no more passing of stream to
**    LMBUNIT_WRITE_HEADER() and LMBUNIT_WRITE_STATISTICS()
**
**  Revision 1.1.1.1  2002/03/13 16:20:41  ronneber
**  inital revision
**
**
**
**************************************************************************/

#ifndef LMBUNIT_HH
#define LMBUNIT_HH

#include <iostream>
#include <sys/types.h>  // for fork()
#include <unistd.h>     // for fork()
#include <sys/wait.h>   // for waitpid()

/*=========================================================================
 *  Modul global Variables
 *========================================================================*/
static int _nFails = 0;
static int _nTests = 0;
static const char* _actualFunctionName = "";

/*======================================================================*/
/*!
 *   Write failure message with preceding sourcefile-name, line number
 *   and function name suitable for emacs-compilation buffer
 *   parsing. Usually this macro is only used directly for complex
 *   tests, like exception catching (see exmaple below). For simpler
 *   Tests use LMBUNIT_ASSERT(), LMBUNIT_ASSERT_EQUAL() and
 *   LMBUNIT_ASSERT_EQUAL_DELTA()
 *
 *   \param message  anything that can be written behind a 'cout <<'.
 *                   E.g., it may include additional '<<'
 *   \par Example:
 *   \code
 *   static void testDivisionByZero()
 *   {
 *     try
 *     {
 *       MyComplex a(1,0);
 *       MyComplex b = a / 0;
 *       LMBUNIT_WRITE_FAILURE( "expected exception 'MyComplex::DivByZero'");
 *     }
 *     catch( MyComplex::DivByZero e)
 *     {
 *       return;
 *     }
 *   }
 *   \endcode
 *   resulting output may be:
 *  \verbatim testMyComplex.cc:47: expected exception 'MyComplex::DivByZero'\endverbatim
 */
/*======================================================================*/
#define LMBUNIT_WRITE_FAILURE( message)                                 \
{                                                                       \
  std::cout << "FAILED!\n"                                              \
            << __FILE__ << ":" << __LINE__ << ": "                      \
            /*<< _actualFunctionName << ": "*/ << message << std::endl;     \
  _nFails++;                                                            \
}

/*======================================================================*/
/*!
 *   write failure message if condition is not fulfilled
 *
 *   \param condition  any expression, that evaluates to true or false
 *
 *   \par Example:
 *   \code
 *   static void testConstructor()
 *   {
 *     MyComplex a;
 *     LMBUNIT_ASSERT( a.imag() == 0);
 *   }
 *   \endcode
 *   resulting output may be:
 *   \verbatim testMyComplex.cc:24: assertion 'a.imag() == 0' failed \endverbatim
 *
 *
 */
/*======================================================================*/
#define LMBUNIT_ASSERT( condition)                                      \
if (!(condition))                                                       \
{                                                                       \
  LMBUNIT_WRITE_FAILURE("assertion '" << (#condition) << "' failed");   \
}

/*======================================================================*/
/*!
 *   write failure message if the two given expressions are not eqal
 *   (compared with the '==' operator).  example:
 *   \param actual  any expression. result of this expression must be
 *                  comparable with the '==' operator to result of
 *                  'expected' and must be printable with '<<'.
 *
 *   \param expected  any expression. result of this expression must be
 *                  comparable with the '==' operator to result of
 *                  'actual' and must be printable with '<<'
 *
 *   \warning If the assertion failes, the given parameters are
 *            evaluated twice!
 *   \par Example:
 *   \code
 *   static void testIntegerAddition()
 *   {
 *     MyComplex a( 21, 0);
 *     MyComplex b( 42, 0);
 *     LMBUNIT_ASSERT_EQUAL( a+a, b);
 *   }
 *   \endcode
 *   resulting output may be:
 *  \verbatim testMyComplex.cc:31: assertion 'a+a == b' failed, because 'a+a' is '(21,0)' and 'b' is '(42,0)' \endverbatim
 *
 */
/*======================================================================*/
#define LMBUNIT_ASSERT_EQUAL( actual, expected)                             \
if (!((actual)==(expected)))                                                \
{                                                                           \
  LMBUNIT_WRITE_FAILURE("assertion '" << (#actual) << " == " << (#expected) \
                        << "' failed, because '"                            \
                        << (#actual) << "' is '" << (actual) << "' and '"   \
                        << (#expected) <<"' is '" << (expected) << "'");    \
}

/*======================================================================*/
/*!
 *   write failure message if the two given expressions are not eqal
 *   within the alowed delta.
 *
 *   \param actual  any expression. result of this expression must be
 *                  comparable with the '<' operator to the result of
 *                  'expected+delta' and 'expected-delta' and must be
 *                  printable with '<<'.
 *
 *   \param expected any expression. It must be posiible to evaluate
 *                  'expression-delta' and 'expression+delta'. The
 *                  Result must be comparable with the '<' operator
 *                  to actual. must be printable with '<<'.
 *
 *   \param delta  any expression. It must be posible to evaluate
 *                  'expression-delta' and 'expression+delta'. The
 *                  Result must be comparable with the '<' operator
 *                  to actual. must be printable with '<<'.
 *
 *   \warning each given parameter is evaluated twice, when the test
 *            succeeds. When the test fails, 'actual' and 'expression'
 *            are evaluated once more
 *
 *   \par Example:
 *   \code
 *   static void testFloatAddition()
 *   {
 *     MyComplex a( 1,0);
 *     MyComplex b( 0.2, 0);
 *     LMBUNIT_ASSERT_EQUAL_DELTA( a, b+b+b+b+b, 0.00000001);
 *   }\endcode
 *  resulting output may be:
 *  \verbatim testMyComplex.cc:38: assertion 'a within b+b+b+b+b +/- 0.00000001' failed, because 'a' is '(1,0)' and 'b+b+b+b+b' is '(0.2,0)'\endverbatim
 *
 */
/*======================================================================*/
#define LMBUNIT_ASSERT_EQUAL_DELTA( actual, expected, delta)              \
if ( ((actual) < (expected)-(delta)) ||  ((expected)+(delta) < (actual)))     \
{                                                                         \
  LMBUNIT_WRITE_FAILURE("assertion '" << (#actual) << " within " <<       \
                        (#expected)<< " +/- " << (#delta)                 \
                        << "' failed, because '"                          \
                        << (#actual) << "' is '" << (actual) << "' and '" \
                        << (#expected) <<"' is '" << (expected) << "'");  \
}

/*======================================================================*/
/*!
 *   write a nice header containing the filename of the testsuite to
 *   given stream
 *
 *   \param os output stream
 *
 *   \par Example:
 *   \code
 *   int main( int argc, char** argv)
 *   {
 *      LMBUNIT_WRITE_HEADER( std::cout);
 *      ...
 *   \endcode
 */
/*======================================================================*/
#define LMBUNIT_WRITE_HEADER()                                  \
{                                                               \
  std::cout <<  "-------------------------------------------\n" \
      " Running Test Suite \"" << __FILE__ << "\"\n\n";         \
}


/*======================================================================*/
/*!
 *   Run a test-function. the given function_call can be any function
 *   call that is allowed in a  C++ program (including passing
 *   parameters etc.). This macro is responsible for counting the
 *   number of tests.
 *
 *   \param function_call any function call
 *
 *   \par Hint
 *   define all test function as 'static'. Then 'g++ -Wall' will
 *   complain about missing calls to that functions
 *
 *   \par Example
 *   \code
 *   LMBUNIT_RUN_TEST( testConstructor() );
 *   LMBUNIT_RUN_TEST( testPrintOut( a, "1.000") );
 *   LMBUNIT_RUN_TEST( xyz::mytest() );
 *   \endcode
 */
/*======================================================================*/
#define LMBUNIT_RUN_TEST( function_call)                                                        \
{                                                                                               \
  _actualFunctionName=(#function_call);                                                         \
  _nTests++;                                                                                    \
  pid_t pid = fork();                                                                           \
  if(  pid == 0)                                                                                \
  {                                                                                             \
    /* this is the child */                                                                     \
    int oldNFails = _nFails;                                                                    \
    try                                                                                         \
    {                                                                                           \
      std::cout << " " << _actualFunctionName                                                   \
                << "... " << std::flush;                                                        \
      function_call;                                                                            \
    }                                                                                           \
    catch(...)                                                                                  \
    {                                                                                           \
      LMBUNIT_WRITE_FAILURE( "caught exception");                                               \
    }                                                                                           \
    if( oldNFails == _nFails)                                                                   \
    {                                                                                           \
      std::cout << "PASSED\n";                                                                  \
    }                                                                                           \
    exit( _nFails - oldNFails);                                                                 \
    /* This is end of child */                                                                  \
  }                                                                                             \
  else                                                                                          \
  {                                                                                             \
    /* this is the parent */                                                                    \
    int status;                                                                                 \
    waitpid( pid, &status, 0);   \
    if( WTERMSIG(status) != 0)                                                                  \
    {                                                                                           \
                                                                                                \
      switch( WTERMSIG(status))                                                                 \
      {                                                                                         \
      case SIGQUIT: LMBUNIT_WRITE_FAILURE( "Quit from keyboard");                               \
        break;                                                                                  \
      case SIGILL:  LMBUNIT_WRITE_FAILURE( "Illegal Instruction");                              \
        break;                                                                                  \
      case SIGABRT: LMBUNIT_WRITE_FAILURE( "Abort signal from abort(3)");                       \
        break;                                                                                  \
      case SIGFPE:  LMBUNIT_WRITE_FAILURE( "Floating point exception");                         \
        break;                                                                                  \
      case SIGKILL: LMBUNIT_WRITE_FAILURE( "Kill signal");                                      \
        break;                                                                                  \
      case SIGSEGV: LMBUNIT_WRITE_FAILURE( "Segmentation violation");                           \
        break;                                                                                  \
      case SIGBUS:  LMBUNIT_WRITE_FAILURE( "Bus error (bad memory access)");                    \
        break;                                                                                  \
      case SIGSYS:  LMBUNIT_WRITE_FAILURE( "Bad argument to routine (SVID)");                   \
        break;                                                                                  \
      default:      LMBUNIT_WRITE_FAILURE( "unknown signal (" <<WTERMSIG(status)<<") ");        \
      }                                                                                         \
    }                                                                                           \
    else                                                                                        \
    {                                                                                           \
      _nFails += WEXITSTATUS(status);                                                           \
    }                                                                                           \
  }                                                                                             \
}

#define LMBUNIT_RUN_TEST_NOFORK( function_call)                        \
{                                                               \
  _nTests++;                                                    \
  int oldNFails = _nFails;                                      \
  try                                                           \
  {                                                             \
    _actualFunctionName=(#function_call);                       \
    std::cout << " " << _actualFunctionName         \
              << "... " << std::flush;                          \
    function_call;                                              \
  }                                                             \
  catch(...)                                                    \
  {                                                             \
    LMBUNIT_WRITE_FAILURE( "caught exception");                 \
  }                                                             \
                                                                \
  if( oldNFails == _nFails)                                     \
  {                                                             \
    std::cout << "PASSED\n";                                        \
  }                                                             \
}

/*======================================================================*/
/*!
 *   write the collected statistics for this testsuite to given stream
 *
 *   \param os output stream
 *
 *   \par Example:
 *   \code
 *   int main( int argc, char** argv)
 *   {
 *      // ...
 *      LMBUNIT_WRITE_STATISTICS( std::cout);
 *      return _nFails;
 *   }
 *   \endcode
 */
/*======================================================================*/
inline void LMBUNIT_WRITE_STATISTICS()
{
  if( _nFails == 0)
  {
    std::cout << "\n All " << _nTests << " tests passed\n"
              << "--------------------------------------------\n";
  }
  else
  {
    std::cout << "\n " <<_nFails <<" of " << _nTests << " tests failed!\n"
              << "--------------------------------------------\n";
  }
  
}


#endif
