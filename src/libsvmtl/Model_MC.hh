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
**       Title: base class for all multi class models
**    $RCSfile$
**   $Revision: 2824 $$Name$
**       $Date: 2009-09-14 09:30:46 +0200 (Mon, 14 Sep 2009) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2005/02/24 13:45:49  fehr
**  enable SV access through SVMAdapter
**
**  Revision 1.2  2005/02/24 12:57:50  fehr
**  some more 64-bit netcdf bugfixing
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef MODEL_MC_HH
#define MODEL_MC_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "SVMError.hh"

#include "svt_check/RequireMinimalTCModel.hh"

namespace svt
{
  
template< typename TCModel>
class Model_MC
{
  CHECK_CLASS_TEMPLATE1( svt_check::RequireMinimalTCModel<TCModel> )

public:
  
  typedef typename TCModel::FV_type FV;


  Model_MC()
          :_owningSupportVectors(false),
           _maxUniqueID( 0)
        {}
  
  ~Model_MC()
        {
          freeCollectedSupportVectors();
        }

  Model_MC( const Model_MC<TCModel>& orig)
          :_collectedSupportVectors( orig._collectedSupportVectors),
           _classIndexToLabel( orig._classIndexToLabel),
           _owningSupportVectors( false),   
           _maxUniqueID( orig._maxUniqueID)
        {
        }
  
  Model_MC<TCModel>& operator=( const Model_MC<TCModel>& orig)
        {
          _collectedSupportVectors = orig._collectedSupportVectors;
          _classIndexToLabel       = orig._classIndexToLabel;
          _owningSupportVectors    = false;
          _maxUniqueID             = orig._maxUniqueID;
        }
  


  void resizeCollectedSupportVectors( unsigned int size)
        {
          freeCollectedSupportVectors();
          if( size != _collectedSupportVectors.size())
          {
            _collectedSupportVectors.resize( size);
          }
          
        }

 
  void freeCollectedSupportVectors()
        {
          if( _owningSupportVectors)
          {
            for( unsigned int i = 0; i < _collectedSupportVectors.size(); ++i)
            {
              delete _collectedSupportVectors[i];
              _collectedSupportVectors[i] = 0;
            }
          }
          _owningSupportVectors = false;
        }
  


  const std::vector<FV*>& collectedSupportVectors() const
        {
          return _collectedSupportVectors;
        }


  
  unsigned int maxUniqueID() const
        {
          return _maxUniqueID;
        }

  void setClassIndizesToLabels( const std::vector<double>& v)
        {
          _classIndexToLabel = v;
        }
  
  double classIndexToLabel( unsigned int classIndex) const
        {
          SVM_ASSERT( classIndex < _classIndexToLabel.size());
          return _classIndexToLabel[classIndex];
        }

    /*======================================================================*/
    /*!  
     *   check, if this model is affected by specified left out
     *   vectors, which means, that it needs to be retrained, if these
     *   vectors are left out from the training data set
     *
     *   \param leaveOutFlagsByUID leave out flag for each feature
     *                    vector, accessed via its uniqueID (true
     *                    means leave this vector out from training)
     *
     *   \return true or false
     */
    /*======================================================================*/
  bool isModelAffectedByLeftOutVectors( 
      const std::vector<char>& leaveOutFlagsByUID) const
        {
          for( unsigned int i = 0; i < _collectedSupportVectors.size(); ++i)
          {
            unsigned int uid = _collectedSupportVectors[i]->uniqueID();
            SVM_ASSERT( uid < leaveOutFlagsByUID.size());
            
            if( leaveOutFlagsByUID[uid] == 1) return true;
          }
          return false;
        }
  

  std::vector<FV*> getSupportVectors() const
  {
      return _collectedSupportVectors;
  }

protected:
  template< typename ForwardIter>
  void collectSupportVectorsFromTCModels( const ForwardIter& modelsBegin, 
                                          const ForwardIter& modelsEnd);


  template<typename STDATA>
  void saveSupportVectorsAndClassLabels( STDATA& stData) const;
  
  template<typename STDATA>
  void loadSupportVectorsAndClassLabels( STDATA& stData);
  
 template< typename ForwardIter, typename STDATA>
 void calcTrainingStatisticsFromTCModels(
     const ForwardIter& modelsBegin, const ForwardIter& modelsEnd,
     STDATA& statistics);
  
 
          

private:
  std::vector<FV*>    _collectedSupportVectors;
  std::vector<double> _classIndexToLabel;
  bool                _owningSupportVectors;
  unsigned int        _maxUniqueID;
  
};
  
 
}

#include "Model_MC.icc"
#endif
