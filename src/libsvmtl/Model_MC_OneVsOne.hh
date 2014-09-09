/**************************************************************************
**       Title: Model for MultiClassSVMOneVsOne SVM
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
**  Revision 1.2  2006/01/25 08:53:51  fehr
**  added HistIntersect to Kmatrix Kernels and new BasicSVMAdapter interface to TwoClassModels
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/


#ifndef MODEL_MC_ONEVSONE_HH
#define MODEL_MC_ONEVSONE_HH

#include "SVMError.hh"
#include "TriangularMatrix.hh"
#include "Model_MC.hh"

// requirements of template parameters
#include "svt_check/RequireStData.hh"
#include "svt_check/RequireStDataFVArray.hh"
#include "svt_check/RequireMinimalTCModel.hh"

namespace svt
{
  
template< typename TCModel>
class Model_MC_OneVsOne : public Model_MC<TCModel>
{
  CHECK_CLASS_TEMPLATE1( svt_check::RequireMinimalTCModel<TCModel>)

public:
  
  typedef typename TCModel::FV_type FV;

  void resizeTriangularMatrix( unsigned int nClasses)
        {
          _triangMat.resizeWidth( nClasses);
        }

  unsigned int nClasses() const
        {
          return static_cast<unsigned int>(_triangMat.width());
        }

  unsigned int nTwoClassModels() const
        {
          return static_cast<unsigned int>(_triangMat.nElements());
        }
  
  
  
  TCModel& twoClassModel( unsigned int class1, unsigned int class2)
        {
          return _triangMat(class1, class2);
        }
  
  const TCModel& twoClassModel( unsigned int class1, unsigned int class2) const
        {
          return _triangMat(class1, class2);
        }
 
  const TCModel& twoClassModel( unsigned int class1) const
	{
	 return _triangMat[class1];
	}
  
  TCModel& twoClassModel( unsigned int class1)
	{
	 return _triangMat[class1];
	}

  void collectSupportVectorsFromTCModels()
        {
          Model_MC<TCModel>::collectSupportVectorsFromTCModels(
              _triangMat.begin(), _triangMat.end());
        }
  
  template< typename STDATA>
  void saveTCTrainingInfos( STDATA& statistics)
        {
          CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);

          /*---------------------------------------------------------------
           *  Run throgh all two-class models and store their training infos
           *---------------------------------------------------------------*/
          for( unsigned int firstClass = 0; 
               firstClass < _triangMat.width()-1; ++firstClass)
          {
            for( unsigned int secondClass = firstClass+1; 
                 secondClass < _triangMat.width(); ++secondClass)
            {
              std::ostringstream prefix_os;
              prefix_os << firstClass << "vs" << secondClass << "_";
              _triangMat( firstClass ,secondClass).saveTrainingInfo(
                  statistics, prefix_os.str());
            }
          }
        }

  
  template< typename STDATA>
  void saveTrainingInfoStatistics( STDATA& statistics)
        {
          CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);

          this->calcTrainingStatisticsFromTCModels( 
              _triangMat.begin(), _triangMat.end(), statistics);
        }

  

  /*======================================================================*/
  /*! 
   *   save all data from two-class models to Map, where the keys of
   *   each model will be prefixed by its position in the triangular
   *   matrix (e.g. "1vs4_alphas"). The support vectors in each model are
   *   referenced by their uniqueID, and a list of all used feature
   *   vectors is stored with the key "sv_values"
   *
   *   \param 
   *
   *   \exception 
   *
   *   \return
   */
  /*======================================================================*/
  template<typename STDATA>
  void saveParameters( STDATA& stData) const;


  /*======================================================================*/
  /*! 
   *   load all data from two-class models to Map, where the keys of
   *   each model are prefixed by its position in the triangular
   *   matrix (e.g. "1vs4_alphas").
   *
   *   \param 
   *
   *   \exception 
   *
   *   \return
   */
  /*======================================================================*/
  template<typename STDATA>
  void loadParameters( STDATA& stData);




private:
  TriangularMatrix< TCModel> _triangMat;
  
};
  
 
}


#include "Model_MC_OneVsOne.icc"

#endif
