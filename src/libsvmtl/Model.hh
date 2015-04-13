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
**       Title: Model
**    $RCSfile$
**   $Revision: 5097 $$Name$
**       $Date: 2013-12-05 16:20:39 +0100 (Thu, 05 Dec 2013) $
**   Copyright: LGPL $Author: tschmidt $
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
**  Revision 1.2  2005/02/01 12:34:45  ronneber
**  - fixed comment
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.9  2003/10/01 09:25:59  ronneber
**  - cosmetic changes
**
**  Revision 1.8  2003/05/19 10:57:23  ronneber
**  - now stores SolutionInfo in Model to have later access to several
**    training parameters
**
**  Revision 1.7  2002/10/28 16:00:52  mechnich
**  fixed compatibility problems with gcc3
**
**  Revision 1.6  2002/06/07 11:37:46  ronneber
**  - added detachFromTrainingDataSet()
**
**  Revision 1.5  2002/06/07 07:34:19  ronneber
**  - added public typedef for Support Vector List type, to make
**    classifier more flexible (e.g. when using a slightly different but
**    compatible Feature Vector class)
**
**  Revision 1.4  2002/05/21 18:31:11  ronneber
**  - SupportVector.hh must not be included in angle brackets!
**
**  Revision 1.3  2002/05/13 16:20:40  ronneber
**  - moved SupportVector class to SupportVector.hh
**  - made all attributes private and added accessing methods
**  - removed coutPlus and countMinus -- they could  easily be calculated
**    later by the library-user
**
**  Revision 1.2  2002/05/06 12:22:52  ronneber
**  - removed Parameter struct
**
**  Revision 1.1  2002/03/26 12:44:02  ronneber
**  restructured for autoconf
**
**  Revision 1.4  2002/03/08 11:42:38  pigorsch
**  * introduced new Model and SupportVector classes
**
**  Revision 1.3  2002/01/21 13:47:15  pigorsch
**  * child class Parameters of class Model is now separate class
**  * removed parameter "params" from constructor of CachedKernel
**
**  Revision 1.2  2001/12/17 13:18:30  pigorsch
**  *** empty log message ***
**
**  Revision 1.1  2001/12/11 11:03:00  pigorsch
**  Initial Revision
**
**
**
**************************************************************************/

#ifndef MODEL_HH
#define MODEL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>
#include "StDataASCII.hh"

// requirements of template parameters
#include "svt_check/RequireStData.hh"
#include "svt_check/RequireStDataFVArray.hh"
#include "svt_check/RequireFeatureVectorUniqueID.hh"

// Disable Warnings about usage of unsafe functions
#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

namespace svt
{
  template<typename FV>
  class Model
  {

  public:

    typedef FV FV_type;

    Model()
            :_rho(0),
             _supportVectors(0),
             _alphas(0),
             _size(0),
             _owningSupportVectors(false)
          {}
   

    Model( int nSupportVectors)
             :_rho(0),
              _supportVectors(0),
              _alphas(0),
              _size(0),
              _owningSupportVectors(false)
          {
            resize( nSupportVectors);
          }

    ~Model()
          {
            free_memory();
          }

    /*======================================================================*/
    /*! 
     *   Copy contructor
     *
     *   \param orig
     */
    /*======================================================================*/
    Model( const Model<FV>& orig)
            :_rho( 0),
             _supportVectors( 0),
             _alphas(0),
             _size(0),
             _owningSupportVectors(false)
          {
            *this = orig;
          }
    
    /*======================================================================*/
    /*! 
     *   operator=
     *
     *   \param orig
     */
    /*======================================================================*/
    Model& operator=( const Model& orig)
          {
            _rho = orig._rho;
            resize( orig._size);
            _owningSupportVectors = false;
            std::copy( orig._supportVectors, orig._supportVectors+orig._size,
                       _supportVectors);
            std::copy( orig._alphas, orig._alphas + orig._size,
                       _alphas);
            _trainingInfo = orig._trainingInfo;
            return *this;
          }
    

    

    void free_memory();
    
    
    void resize( size_t nSupportVectors);
    


    size_t size() const
          { return _size; }
    
    void setRho( double rho) 
          { _rho = rho; }

    double rho() const         
          { return _rho; }

    void setSupportVector( unsigned int i, FV* fv, double alpha)
          {
            _supportVectors[i] = fv;
            _alphas[i] = alpha;
          }
    
    
    const FV* supportVector( unsigned int i) const
          { return _supportVectors[i]; }

    FV* supportVector( unsigned int i) 
          { return _supportVectors[i]; }

    double alpha( unsigned int i) const
          { return _alphas[i]; }
    
 

    FV** allSupportVectors()       
          { return _supportVectors; }

    FV* const *  allSupportVectors() const 
          { return _supportVectors; }
    
    

    double* allAlphas()
          { return _alphas; }

    const double* allAlphas() const 
          { return _alphas; }

   
    void copySVCoefsToSupportVectors();

    void detachFromTrainingDataSet()
          {
            if( _owningSupportVectors == true) return;
            
            for( unsigned int i = 0; i < _size; ++i)
            {
              _supportVectors[i] = new FV( *_supportVectors[i]);
            }
            _owningSupportVectors = true;
            
          }

    template<typename T>
    void setTrainingInfoValue( const std::string& key, const T& value )
          { 
            _trainingInfo.setValue( key, value);
          }
    
    double getTrainingInfoValue( const std::string& key)
          { 
            return _trainingInfo.asDouble( key);
          }
    
    std::string trainingInfoPlainText()
          {
            std::ostringstream oss;
            for( std::map<std::string, std::string>::const_iterator 
                     p = _trainingInfo.begin(); p != _trainingInfo.end(); ++p)
            {
              oss << p->first << ": " << p->second << std::endl;
            }
            return oss.str();
          }
    
    template<typename STDATA>
    void saveTrainingInfo( STDATA& stData, 
                           const std::string& prefix = "")
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            for( std::map<std::string, std::string>::const_iterator 
                     p = _trainingInfo.begin(); p != _trainingInfo.end(); ++p)
            {
              double val = 0;
              _trainingInfo.getValue( p->first, val);
              stData.setValue( prefix + p->first, val);
            }
          }
    

    /*======================================================================*/
    /*! 
     *   save the model data to given structured data. The support vectors
     *   are stored  as full vectors in "SV" 
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    template<typename STDATA>
    void saveParameters( STDATA& stData, 
                         const std::string& prefix = "") const;
    

    /*======================================================================*/
    /*! 
     *   load the model data from given structured data. Use this method
     *   only if the feature vectors are stored as full vectors with
     *   key "SV". If there are only their uniqueID's in thw key
     *   "sv_uids", you must use the loadParametersWithUIDs() method
     *   instead 
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    template<typename STDATA>
    void loadParameters( STDATA& stData, 
                         const std::string& prefix = "");
    
    
    /*======================================================================*/
    /*! 
     *   save the model data to string-string map. only the uniqueID's
     *   of the support vectors are stored  in "sv_uids"
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    template<typename STDATA>
    void saveParametersWithUIDs( STDATA& stData, 
                                 const std::string& prefix = "") const;
    

    /*======================================================================*/
    /*! 
     *   (description)
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    template<typename STDATA>
    void loadParametersWithUIDs( STDATA& stData,
                                 const std::vector<FV*>& fvsByUID,
                                 const std::string& prefix = "");
    
    
    /*======================================================================*/
    /*!  
     *   check, if this model is affected by specified left out
     *   vectors, which means, that it needs to be retrained, if these
     *   vectors are left out from the training data set
     *
     *   \param leaveOutFlagsByUID leave out flag for each feature
     *                    vector, accessed via its uniqueID 
     *                    (1 means leave this vector out from training)
     *
     *   \return true or false
     */
    /*======================================================================*/
    bool isModelAffectedByLeftOutVectors( 
        const std::vector<char>& leaveOutFlagsByUID) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireFeatureVectorUniqueID<FV>);
            
            for( unsigned int i = 0; i < _size; ++i)
            {
              unsigned int uid = _supportVectors[i]->uniqueID();
              SVM_ASSERT( uid < leaveOutFlagsByUID.size());
              
              if( leaveOutFlagsByUID[uid] == 1) return true;
            }
            return false;
          }
   
  
  private:
    double  _rho;
    FV**    _supportVectors;   // array of Feature Vector pointers
    double* _alphas;
    size_t  _size;
    bool    _owningSupportVectors;
    
    StDataASCII _trainingInfo;


  };

}

#include "Model.icc"

#endif
