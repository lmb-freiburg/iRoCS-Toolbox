/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 2825 $$Name$
**       $Date: 2009-09-15 17:04:15 +0200 (Tue, 15 Sep 2009) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2004/09/13 10:03:00  ronneber
**  - documentation update
**
**  Revision 1.2  2004/09/08 14:28:20  ronneber
**  - adapted to new ParamInfo class
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/
#ifndef HELPEXTRACTOR_HH
#define HELPEXTRACTOR_HH

#include <string>
#include <map>
#include <set>

#include "TList.hh"
#include "ParamInfo.hh"

namespace svt
{

  /*======================================================================*/
  /*!
   *  \class  HelpExtractor
   *  \ingroup factory
   *  \brief The HelpExtractor class contains methods to extract the
   *         ParamInfo's and name() / Descriptions form all elements
   *         in a TList   
   *
   *  \param LIST a TypeList with classes, that provide a getParamInfos()
   *         method or name() and description() methods
   */
  /*======================================================================*/
  template<typename LIST>
  struct HelpExtractor
  {

    /*======================================================================*/
    /*! 
     *   recursive getParamInfos() for TList items
     *
     *   \param help   (in/out) add Help texts for parameters 
     *                 of current and following TList
     *                 items to given map
     */
    /*======================================================================*/
    static void collectParamInfos( std::vector<ParamInfo>& help)
          {
            /*-------------------------------------------------------------
             *  Extract help from current item
             *-------------------------------------------------------------*/
            LIST::val_t::getParamInfos( help);
            
            /*-------------------------------------------------------------
             *  recursive call HelpExtractor::collectParamInfos for next item
             *-------------------------------------------------------------*/
            HelpExtractor<typename LIST::next_t>::collectParamInfos( 
                help);
          }

    /*======================================================================*/
    /*! 
     *   recursive creating of ParamInfo - Alternatives from name() and
     *   description() methods from all elements in given Type list. 
     *
     *   \param longKey longKey   entry for each created ParamInfo
     *   \param shortKey shortKey entry for each created ParamInfo
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    static ParamInfo createParamInfoFromNamesDescriptions( 
        std::string longKey, std::string shortKey)
          {
            ParamInfo help( longKey, shortKey);
            HelpExtractor<LIST>::getNamesDescriptions( help);
            return help;
          }
    
            


    /*======================================================================*/
    /*! 
     *   recursive extract name() and description() for TList items
     *
     *   \param help  (in/out) add name() and description() of current
     *                TList item to given ParamInfo using
     *                addAlternative() 
     */
    /*======================================================================*/
     static void getNamesDescriptions( ParamInfo& help)
          {
            
            /*-------------------------------------------------------------
             *  Extract name and description from current item
             *-------------------------------------------------------------*/
            std::string name         = LIST::val_t::name();
            std::string description  = LIST::val_t::description();
            help.addAlternative( name, description);
                        
            /*-------------------------------------------------------------
             *  recursive call HelpExtractor::getNamesDescriptions
             *  for next item
             *-------------------------------------------------------------*/
            HelpExtractor<typename LIST::next_t>::
                getNamesDescriptions( help);
          }
  };
  
  
  
  /*-----------------------------------------------------------------------
   *  partial specialization for TList's end
   *-----------------------------------------------------------------------*/
  template<>
  struct HelpExtractor<svt::TList_end>
  {
    static void collectParamInfos( std::vector<ParamInfo>&) 
          {}
    
    static ParamInfo createParamInfoFromNamesDescriptions( 
        std::string longKey, std::string shortKey)
          {
            return ParamInfo( longKey,shortKey, "xxx", "no algorithms available" );
          }

    static void getNamesDescriptions( ParamInfo&)
          {}

     
  };

  /*-----------------------------------------------------------------------
   *  partial specialization for TTList items
   *-----------------------------------------------------------------------*/
  template<template<typename> class X, typename Y>
  struct HelpExtractor<svt::TTList<X,Y> >
  {
    
    /*======================================================================*/
    /*! 
     *   recursive getParamInfos() for TList items
     *
     *   \param help   (in/out) add Help texts for parameters 
     *                 of current and following TList
     *                 items to given map
     */
    /*======================================================================*/
    static void collectParamInfos(  std::vector<ParamInfo>& help)
          {
            /*-------------------------------------------------------------
             *  Extract help from current item
             *-------------------------------------------------------------*/
            svt::TTList<X,Y>::template Traits<int>::val_t::getParamInfos( help);
            
            /*-------------------------------------------------------------
             *  recursive call HelpExtractor::collectParamInfos for next item
             *-------------------------------------------------------------*/
            HelpExtractor<typename svt::TTList<X,Y>::next_t>::collectParamInfos(
                help);
          }


    /*======================================================================*/
    /*! 
     *   recursive creating of ParamInfo - Alternatives from name() and
     *   description() methods from all elements in given Type list. 
     *
     *   \param longKey longKey   entry for each created ParamInfo
     *   \param shortKey shortKey entry for each created ParamInfo
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    static ParamInfo createParamInfoFromNamesDescriptions( 
        std::string longKey, std::string shortKey)
          {
            ParamInfo help( longKey, shortKey);
            HelpExtractor<svt::TTList<X,Y> >::getNamesDescriptions( help);
            return help;
          }
    
            
    /*======================================================================*/
    /*! 
     *   recursive extract name() and description() for TList items
     *
     *   \param help  (in/out) add name() and description() of current
     *                TList item to given ParamInfo using
     *                addAlternative() 
     */
    /*======================================================================*/
     static void getNamesDescriptions( ParamInfo& help)
          {
            
            /*-------------------------------------------------------------
             *  Extract name and description from current item
             *-------------------------------------------------------------*/
            std::string name = svt::TTList<X,Y>::template Traits<int>::val_t::name();
            std::string desc = svt::TTList<X,Y>::template Traits<int>::val_t::description();
            help.addAlternative( name, desc);
                        
            /*-------------------------------------------------------------
             *  recursive call HelpExtractor::getNamesDescriptions
             *  for next item
             *-------------------------------------------------------------*/
            HelpExtractor<typename svt::TTList<X,Y>::next_t>::
                getNamesDescriptions( help);
          }
  };

  


}
#endif
