/**************************************************************************
**       Title: 
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
#include <map>

#include "lmbunit.hh"
#include <libsvmtl/Kernel_LINEAR.hh>
#include <libsvmtl/Kernel_POLY.hh>
#include <libsvmtl/Kernel_RBF.hh>
#include <libsvmtl/Kernel_SIGMOID.hh>
#include <libsvmtl/Kernel_MATRIX.hh>
#include <libsvmtl/TList.hh>
#include <libsvmtl/TwoClassSVMc.hh>
#include <libsvmtl/TwoClassSVMnu.hh>
#include <libsvmtl/MultiClassSVMOneVsOne.hh>
#include <libsvmtl/MultiClassSVMOneVsRest.hh>
#include <libsvmtl/HelpExtractor.hh>

/*-------------------------------------------------------------------------
 *  specify multi-class Algorithms
 *-------------------------------------------------------------------------*/
typedef TTLIST_2( svt::MultiClassSVMOneVsOne, 
                  svt::MultiClassSVMOneVsRest) MyMultiClassList;

/*-------------------------------------------------------------------------
 *  specify two-class Algorithms
 *-------------------------------------------------------------------------*/
typedef TTLIST_2( svt::TwoClassSVMc, 
                  svt::TwoClassSVMnu) MyTwoClassList;

/*-------------------------------------------------------------------------
 *  specify kernel functions
 *-------------------------------------------------------------------------*/
typedef TLIST_8( svt::Kernel_LINEAR, 
                 svt::Kernel_RBF, 
                 svt::Kernel_POLY, 
                 svt::Kernel_SIGMOID,
                 svt::Kernel_MATRIX<svt::Kernel_LINEAR>,
                 svt::Kernel_MATRIX<svt::Kernel_RBF>,
                 svt::Kernel_MATRIX<svt::Kernel_POLY>,
                 svt::Kernel_MATRIX<svt::Kernel_SIGMOID> ) MyKernelList;




template< typename LIST>
static void testHelpExtractor( unsigned int nParameters, 
                               unsigned int nElements)
{
  std::vector<svt::ParamInfo> help;

  svt::HelpExtractor< LIST>::collectParamInfos( help);
  svt::sortAndRemoveDuplicates( help);

  for( std::vector<svt::ParamInfo>::const_iterator p = 
           help.begin(); p != help.end(); ++p)
  {
    p->debugPrint( LMBUNIT_DEBUG_STREAM);
    LMBUNIT_DEBUG_STREAM << "\n";
    
  }

  LMBUNIT_ASSERT_EQUAL( help.size(), nParameters);
  
  svt::ParamInfo param = 
      svt::HelpExtractor< LIST>::createParamInfoFromNamesDescriptions("list",
                                                                      "l");
  param.debugPrint( LMBUNIT_DEBUG_STREAM);
  LMBUNIT_ASSERT_EQUAL( param.nAlternatives(), nElements);
   
}




int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();
  LMBUNIT_RUN_TEST( testHelpExtractor<MyMultiClassList>(0,2) );
  LMBUNIT_RUN_TEST( testHelpExtractor<MyTwoClassList>(6,2) );
  LMBUNIT_RUN_TEST( testHelpExtractor<MyKernelList>(6,8) );
  
  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}

