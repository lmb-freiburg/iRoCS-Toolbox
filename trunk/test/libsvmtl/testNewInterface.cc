/**************************************************************************
**       Title: test unit for new train() interface 
**    $RCSfile$
**   $Revision: 476 $$Name$
**       $Date: 2004-08-26 10:36:59 +0200 (Thu, 26 Aug 2004) $
**   Copyright: GPL $Author: ronneber $
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
**  Revision 1.3  2003/05/19 11:41:54  ronneber
**  - converted from MapTools to ParamMapWrapper
**
**  Revision 1.2  2003/02/10 07:38:43  ronneber
**  - added dummy loadFromMap and saveParameters
**
**  Revision 1.1  2002/06/07 12:28:08  ronneber
**  initial revision
**
**
**
**************************************************************************/


#include "lmbunit.hh"
#include "../libsvmtl/TwoClassSVMc.hh"
#include "../libsvmtl/TwoClassSVMnu.hh"


class LinearVec3Kern
{
public:
  double k_function( const float* fv1,
                     const float* fv2) const
        {
          return (fv1[0] * fv2[0] +
                  fv1[1] * fv2[1] +
                  fv1[2] * fv2[2]);
        }

    void loadFromMap( const std::map<std::string, std::string>& theMap)
          {
            // nothing to do
          };
    
    void saveToMap( std::map<std::string, std::string>& theMap) const
          {
            // nothing to do
          };
};


static void testTwoClassSVMc()
{
  typedef float* FV;
  
  FV fv1 = new float[3];
  FV fv2 = new float[3];
  
  fv1[0] = 0;
  fv1[1] = 0;
  fv1[2] = 0;
  
  fv2[0] = 1;
  fv2[1] = 0;
  fv2[2] = 0;
  
  FV* positiveFVs[] = { &fv1};
  FV* negativeFVs[] = { &fv2};

  svt::TwoClassSVMc< LinearVec3Kern> svm;
  svt::Model<FV> model;
 
  svm.setCost( 100);
  
  svm.train( positiveFVs, positiveFVs + 1,
             negativeFVs, negativeFVs + 1,
             model);
 
  LMBUNIT_ASSERT_EQUAL( model.size(), 2);
  LMBUNIT_ASSERT_EQUAL_DELTA( svm.classify( fv1, model), +1,
                              0.000001);
  LMBUNIT_ASSERT_EQUAL_DELTA( svm.classify( fv2, model), -1,
                              0.000001);
}


int main( int argc, char** argv)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST( testTwoClassSVMc() );

  LMBUNIT_WRITE_STATISTICS();

  return _nFails;
}


