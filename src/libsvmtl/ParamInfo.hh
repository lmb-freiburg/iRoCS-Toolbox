/**************************************************************************
**       Title: 
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
**  Revision 1.1  2004/09/08 14:50:37  ronneber
**  - initial revision
**
**
**
**************************************************************************/


#ifndef PARAMINFO_HH
#define PARAMINFO_HH

#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include "SVMError.hh"

namespace svt
{
  /*-----------------------------------------------------------------------
   *  structure for sotring one alternative vor argument values
   *-----------------------------------------------------------------------*/
  struct AlternativeHelp
  {
    AlternativeHelp( const std::string& v, const std::string& h)
            :value( v),
             helpText( h)
          {}
    std::string value;
    std::string helpText;
  };

  /*======================================================================*/
  /*!
   *  \class ParamInfo ParamInfo.hh
   *  \brief The ParamInfo class contains informations about one
   *         parameter like key, help text, guiHints etc. 
   *
   *  (description)
   */
  /*======================================================================*/
  class ParamInfo
  {
    

  public:
    /*======================================================================*/
    /*! 
     *   Construtor for keys, that need only one helpText (e.g. "gamma")
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    ParamInfo( std::string longKey, std::string shortKey, 
               std::string value, std::string helpText)
            :_longKey( longKey),
             _shortKey( shortKey)
          {
            addAlternative( value, helpText);
          }

    /*======================================================================*/
    /*! 
     *   Construtor for keys, that need multiple alternative values,
     *   e.g. "kernel_type", where value may be "linear", "rbf",
     *   "poly", etc. Add the alternatives with addAlternative()
     *   method alter
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    ParamInfo( std::string longKey, std::string shortKey)
            :_longKey( longKey),
             _shortKey( shortKey)
          {}
    

    void addAlternative( const std::string& value, const std::string& helpText)
          {
            _alternatives.push_back( AlternativeHelp( value, helpText));
          }

    void addGuiHint( const std::string& key, const std::string& value)
          {
            _guiHints[key] = value;
          }
    
    
    const std::string& longKey() const          
          { return _longKey; }

    const std::string& shortKey() const         
          { return _shortKey; }

    size_t nAlternatives() const 
          {
            return _alternatives.size();
          }
    
    const AlternativeHelp& alternatives( size_t index) const 
          { 
            SVM_ASSERT( index < _alternatives.size());
            return _alternatives[index];
          }

    const std::map<std::string, std::string>& guiHints() const 
          {
            return _guiHints;
          }

    bool operator<( const ParamInfo& rhs) const
          {
            if( _longKey < rhs._longKey) return true;
            if( _longKey > rhs._longKey) return false;
            if( _shortKey < rhs._shortKey) return true;
            if( _shortKey > rhs._shortKey) return false;
            size_t nAlter =
                std::min(_alternatives.size(), rhs._alternatives.size());
            for( size_t i = 0; i < nAlter; ++i)
            {
              if( _alternatives[i].helpText < rhs._alternatives[i].helpText)
                  return true;
              if( _alternatives[i].helpText > rhs._alternatives[i].helpText)
                  return false;
            }
            return false;
          }
    
    bool operator==( const ParamInfo& rhs) const
          {
            if( _longKey  != rhs._longKey) return false;
            if( _shortKey != rhs._shortKey) return false;
            size_t nAlter =
                std::min( _alternatives.size(), rhs._alternatives.size());
            for(size_t i = 0; i < nAlter; ++i)
            {
              if( _alternatives[i].helpText != rhs._alternatives[i].helpText)
                  return false;
            }
            return true;
          }

    void debugPrint(std::ostream& os) const
          {
            os << "longKey                 = " << _longKey << "\n"
               << "shortKey                = " << _shortKey << "\n";
            for( unsigned int i = 0; i < _alternatives.size(); ++i)
            {
              os << "alternative[" << i << "].value    = " 
                 << _alternatives[i].value << "\n"
                 << "alternative[" << i << "].helpText = " 
                 << _alternatives[i].helpText << "\n";
            }
            for( std::map<std::string, std::string>::const_iterator 
                     p = _guiHints.begin(); p != _guiHints.end(); ++p)
            {
              os << "guiHint: " << p->first << " --> " << p->second << "\n";
            }
          }
    
                
    
  private:
    std::string _longKey;
    std::string _shortKey;
    std::vector< AlternativeHelp > _alternatives;
    std::map<std::string, std::string> _guiHints;
    
  };
  
  /*======================================================================*/
  /*! 
   *   sort list of ParamInfo's and remove doubles
   *
   *   \param params (in/out) list of ParamInfo's
   *
   */
  /*======================================================================*/
  inline void sortAndRemoveDuplicates( std::vector<ParamInfo>& params)
  {
    std::sort( params.begin(), params.end());
    params.erase( std::unique( params.begin(), params.end()), params.end());
  }
  




}
#endif
