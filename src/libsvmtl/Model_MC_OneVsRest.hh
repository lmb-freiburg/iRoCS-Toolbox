/**************************************************************************
 *
 * Copyright (C) 2004-2015 Olaf Ronneberger, Florian Pigorsch, Jörg Mechnich,
 *                         Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

/**************************************************************************
**       Title:  Model for MultiClassSVMOneVsRest SVM
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
**  Revision 1.2  2004/09/13 10:04:04  ronneber
**  - documentation update
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef MODEL_MC_ONEVSREST_HH
#define MODEL_MC_ONEVSREST_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>
#include "Model_MC.hh"

// requirements of template parameters
#include "svt_check/RequireStData.hh"
#include "svt_check/RequireStDataFVArray.hh"
#include "svt_check/RequireMinimalTCModel.hh"

namespace svt
{
  
  
template< typename TCModel>
class Model_MC_OneVsRest : public Model_MC<TCModel>
{
  CHECK_CLASS_TEMPLATE1( svt_check::RequireMinimalTCModel<TCModel>)

public:
  
  typedef typename TCModel::FV_type FV;

  void resizeModelsVector( unsigned int nClasses)
        {
          _modelsVector.resize( nClasses);
        }

  unsigned int nClasses() const
        {
          return static_cast<unsigned int>(_modelsVector.size());
        }

  unsigned int nTwoClassModels() const
        {
          return static_cast<unsigned int>(_modelsVector.size());
        }
  
  TCModel& twoClassModel( unsigned int classIndex)
        {
          return _modelsVector[classIndex];
        }
  
  const TCModel& twoClassModel( unsigned int classIndex) const
        {
          return _modelsVector[classIndex];
        }

  void collectSupportVectorsFromTCModels()
        {
          Model_MC<TCModel>::collectSupportVectorsFromTCModels(
              _modelsVector.begin(), _modelsVector.end());
        }
  
  template< typename STDATA>
  void saveTCTrainingInfos( STDATA& statistics)
        {
          CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);

          /*---------------------------------------------------------------
           *  Run throgh all two-class models and store their  training infos
           *---------------------------------------------------------------*/
          for( unsigned int classIndex = 0; classIndex < _modelsVector.size();
               ++classIndex)
          {
            std::ostringstream prefix_os;
            prefix_os << classIndex << "vsRest_";
            _modelsVector[classIndex].saveTrainingInfo( statistics, 
                                                        prefix_os.str());
          }
        }
  

  template< typename STDATA>
  void saveTrainingInfoStatistics( STDATA& statistics)
        {
          CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);

          this->calcTrainingStatisticsFromTCModels( 
              _modelsVector.begin(), _modelsVector.end(), statistics);
        }
  
  /*======================================================================*/
  /*! 
   *   save all data from two-class models to Map, where the keys of
   *   each model will be prefixed by its position in the models vector
   *   (e.g. "2vsRest_alphas"). The support vectors in each model are
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
   *   each model are prefixed by its position in the models vector
   *   (e.g. "2vsRest_alphas").
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
  std::vector< TCModel> _modelsVector;
  
};
  
 
}


#include "Model_MC_OneVsRest.icc"

#endif
