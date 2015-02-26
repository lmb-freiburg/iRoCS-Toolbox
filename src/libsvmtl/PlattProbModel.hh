/**************************************************************************
**       Title: Model Wrapper for Platts Probability estimates
**    $RCSfile$
**   $Revision: 604 $$Name$
**       $Date: 2005-02-11 13:26:53 +0100 (Fri, 11 Feb 2005) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2005/02/11 12:26:53  ronneber
**  - initial revision
**
**
**
**************************************************************************/

#ifndef PLATTPROBMODEL_HH
#define PLATTPROBMODEL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>


namespace svt
{
  template< typename WrappedModel>
  class PlattProbModel : public WrappedModel
  {
  public:

    PlattProbModel()
            :_probA(0),
             _probB(0)
          {}
    

    void setProbAB( double a, double b)
          {
            _probA = a;
            _probB = b;
          }
    
    double probA() const
          {
            return _probA;
          }
    

    double probB() const
          {
            return _probB;
          }
    
    /*======================================================================*/
    /*! 
     *   save the model data to given structured data. 
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
                         const std::string& prefix = "") const
          {
            stData.setValue( prefix+"probA", _probA);
            stData.setValue( prefix+"probB", _probB);
            WrappedModel::saveParameters( stData, prefix);
         }
    
    /*======================================================================*/
    /*! 
     *   load the model data from given structured data. 
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
                         const std::string& prefix = "")
          {
            WrappedModel::loadParameters( stData, prefix);
            stData.getValue( prefix+"probA", _probA);
            stData.getValue( prefix+"probB", _probB);
          }
    
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
                                 const std::string& prefix = "") const
          {
            stData.setValue( prefix+"probA", _probA);
            stData.setValue( prefix+"probB", _probB);
            WrappedModel::saveParametersWithUIDs( stData, prefix);
         }

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
    template<typename STDATA, typename FV>
    void loadParametersWithUIDs( STDATA& stData,
                                 const std::vector<FV*>& fvsByUID,
                                 const std::string& prefix = "")
          {
            WrappedModel::loadParametersWithUIDs( stData, fvsByUID, prefix);
            stData.getValue( prefix+"probA", _probA);
            stData.getValue( prefix+"probB", _probB);
          }
    
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
            // No optimization yet, return true everey time!
            return true;
          }
   
  private:
    double _probA;  /* offset of fitted sigmoid */
    double _probB;  /* scale factor for dec_value, before using 
                       as sigmoid argument */
  };
  

}

#endif
