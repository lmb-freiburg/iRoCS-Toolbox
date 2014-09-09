/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 765 $$Name$
**       $Date: 2005-10-26 09:39:45 +0200 (Wed, 26 Oct 2005) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.4  2005/10/26 07:39:45  ronneber
**  - made all get/read-functions const. They were non-const, because the
**    StDataCmdLine stores, wether parameters were accessed
**    not. Making the functions const and store these informations in a
**    mutable member seems to be the cleaner way
**
**  Revision 1.3  2005/01/28 14:03:30  fehr
**  some include bugfixing for external library use
**
**  Revision 1.2  2004/09/08 14:39:35  ronneber
**  - added updateShortCutTable() and translateShortKeys() for short key, e.g.,
**    "-g" processing
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef STDATACMDLINE_HH
#define STDATACMDLINE_HH

#include "StDataASCII.hh"
#include "ParamInfo.hh"
#include "SVMError.hh"

namespace svt
{

  class StDataCmdLine : public StDataASCII
  {
  public:

    /*======================================================================*/
    /*! 
     *   Constructors
     */
    /*======================================================================*/
    StDataCmdLine()
            :_helpRequested( false)
          {
          }
    
    StDataCmdLine( int argc, const char** argv)
            :_helpRequested( false)
          {
            parseCommandline( argc, argv);
          }
    
    
     /*======================================================================*/
    /*! 
     *   parse given commandline. It is interpreted as 'mode' (the first
     *   argument, accessible via mode()), after that a list of "--key
     *   value" pairs (where the '--' before the keys is removed,
     *   accessible via getValue() or getArray()) and a list of
     *   filenames (accessible via fileNames()), that are
     *   recognized by having no "-" as first character. 
     *   "Two-minus" parameters are translated directly, e.g.
     *
     *       "--nfold 8"  -->  internalmap["nfold"] = "8";
     *       "--multi_class_type one_vs_one" 
     *                   -->  internalmap["multi_class_type"] = "one_vs_one";
     * 
     *   "Single-minus" arguments (like "-g") are store unprocessed and must
     *   be translated later with translateShortKeys(). Of course you
     *   should updateShortCutTable() before that translation.
     *
     *   Special treatment for "--help": it can occur as first
     *   argument after filename (instead of mode), no additional
     *   value must be given and the helpRequested() flag will be set
     *   
     *   \param argc number of arguments including program name
     *   \param argv array of c-style-strings containing arguments
     *
     *   \exception ParseCmdLineError
     *
     */
    /*======================================================================*/
    void parseCommandline( int argc, const char** argv);
   

    /*======================================================================*/
    /*! 
     *   update shortcut table. This table maps short options (like
     *   "-mc") to their approriate long options
     *   (e.g. "--multi_class_type").
     *   For the given params vector it iterate through all elements
     *   and adds their shorKey and LongKey to internal translation
     *   map. Multiple occurences of the same shortKey-->longKey
     *   mapping are allowed. If you try to map an existing shortKey
     *   to a different longKey, the shortKey is marked as ambiguos and
     *   will throw an exception if it occures in translateShortKeys()
     *
     *   \param params  array of ParamInfos, where the shortKey() and
     *                  longKey() of each ParamInfo is used
     */
    /*======================================================================*/
    void updateShortCutTable( const std::vector<ParamInfo>& params);
    

    /*======================================================================*/
    /*! 
     *   translate short keys (like "-g") to long keys ("gamma") using
     *   the internal short cut table. You can modify it with
     *   updateShortCutTable() 
     *
     *   \exception ParseCmdLineError unknown short key or ambiguos
     *                                short key
     */
    /*======================================================================*/
    void translateShortKeys();
    
    /*======================================================================*/
    /*! 
     *   return the mode (1st Command line argument) that was
     *   extracted by parseCommandline()
     *
     *   \return mode (e.g. "train", "classify", etc.
     */
    /*======================================================================*/
    const std::string& mode() const
          {
            return _mode;
          }
    
    /*======================================================================*/
    /*! 
     *   return the filenames that were
     *   extracted by parseCommandline()
     *
     *   \return filenames
     */
    /*======================================================================*/
    const std::vector<std::string> filenames() const
          {
            return _filenames;
          }
    


    /*======================================================================*/
    /*! 
     *   get a value (with arbitrary type) from the internal
     *   map<string,string> specified by the given key. If key is not
     *   found, the value remains unchanged and depending on
     *   exceptionFlag() a KeyNotFoundError exception is thrown.
     *   In addition to getValue from StDataASCII, each requested key
     *   is stored in a second map. This map can be accessed later by
     *   keyOccurenceMap()  
     *
     *   \param key     the key for the requested value
     *   \param value   (output) returned value.  
     *
     *   \exception KeyNotFoundError the requested key does not exist
     *   \exception InvalidDataTypeError the value string couldn't be
     *              converted to the requested type
     */
    /*======================================================================*/
    template<typename T> 
    void getValue( const std::string& key, T& value) const
          {
            _keyOccurenceMap[key]++;
            StDataASCII::getValue( key, value);
          }
    
    /*======================================================================*/
    /*! 
     *   getArray. The values must be separated
     *   by separatorChar(), e.g. mymap["nr_sv"] = "150 120". See
     *   getValue() above for further details.
     *   In addition to getArray from StDataASCII, each requested key
     *   is stored in a second map. This map can be accessed later by
     *   keyOccurenceMap()  
     *
     *   \param key     the key for the requested value
     *   \param arrBegin iterator pointing to your container, that has
     *                  enough space to store all values. You can
     *                  query the required size with
     *                  getArraySize(). 
     *   \param containerSize optional parameter to tell the size of
     *                  your container, so that an error can be
     *                  thrown, wenn it would be exeeded.
     *
     *   \exception KeyNotFoundError the requested key does not exist
     *   \exception InvalidDataTypeError the value string couldn't be
     *              converted to the requested type
     *   
     */
    /*======================================================================*/
    template<typename ForwardIter> 
    void getArray( const std::string& key, const ForwardIter& arrBegin, 
                   int containerSize=-1) const
          {
            _keyOccurenceMap[key]++;
            StDataASCII::getArray( key, arrBegin, containerSize);
          }
            
    

    /*======================================================================*/
    /*! 
     *   return the key Occurence map. In this map each access to a
     *   certain key via getValue or getArray is counted
     *
     *   \return reference to the keyOccurenceMap
     */
    /*======================================================================*/
    std::map<std::string, int>& keyOccurenceMap()
          {
            return _keyOccurenceMap;
          }
    
    
    /*======================================================================*/
    /*! 
     *   find unused parameters (these are parameters, that were
     *   extracted from command line ba parseCommandline()) but not
     *   yet accessed via getValue()
     *
     *   unusedParameters  (output) list of unused parameters
     *
     */
    /*======================================================================*/
    void findUnusedParameters( std::vector<std::string>& unusedParameters) const
          {
            std::map<std::string, std::string>::const_iterator 
                itParam = StDataASCII::begin();
            std::map<std::string,int>::const_iterator
                itOccu = _keyOccurenceMap.begin();
            while( itParam != StDataASCII::end()
                   && itOccu != _keyOccurenceMap.end())
            {
              if( itParam->first == itOccu->first)
              {
                ++itParam;
                ++itOccu;
              }
              else if( itParam->first < itOccu->first)
              {
                // keys in the commandline, that were not requested
                unusedParameters.push_back( itParam->first);
                ++itParam;
              }
              else
              {
                // requested values, that where not in the commandline
                ++itOccu;
              }
            }

            // There may be remaining parameters in parameter map
            for( ; itParam != StDataASCII::end(); ++itParam)
            {
              unusedParameters.push_back( itParam->first);
            }
            
            
          }
    
    /*======================================================================*/
    /*! 
     *   check if the parameter "--help" was given
     *
     *   \return true or false
     */
    /*======================================================================*/
    bool helpRequested() const
          {
            return _helpRequested;
          }
    

    void debugPrintShortCutTable( std::ostream& os) const
          {
            for( std::map<std::string, std::string>::const_iterator
                     p = _shortToLongKeyMap.begin(); 
                 p != _shortToLongKeyMap.end(); ++p)
            {
              os << "'" << p->first << "' --> '" << p->second << "'\n";
            }
          }
    

  private:
    mutable std::map<std::string, int> _keyOccurenceMap;
    std::string                        _mode;
    std::vector<std::string>           _filenames;
    std::map<std::string, std::string> _shortToLongKeyMap;
    bool                               _helpRequested;
    
  };
  
    



}
#endif

