/**************************************************************************
**       Title: structured data class which stores all data as strings
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
**  Revision 1.3  2005/10/26 07:37:05  ronneber
**  - made all get/read-functions const. They were non-const, because the
**    derived StDataCmdLine stores, wether parameters were accessed
**    not. Making the functions const and store these informations in a
**    mutable member seems to be the cleaner way
**
**  Revision 1.2  2004/09/13 10:04:04  ronneber
**  - documentation update
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef STDATA_ASCII
#define STDATA_ASCII

#include <string>
#include <map>
#include <algorithm>
#include <sstream>
#include <limits>

#include "SVMError.hh"
#include "BasicFV.hh"
#include "SparseFV.hh"

namespace svt
{
  /*======================================================================*/
/*!
 *  \class StDataASCII
 *  \ingroup stdata
 *  \brief The StDataASCII class is a container for "structured data",
 *  that is kept completly in memory  
 *
 *  internally it uses a std::map<std::string, std::string> to store
 *  the data
 */
/*======================================================================*/
  class StDataASCII
  {
  public:

    StDataASCII()
            :_exceptionFlag( false),
             _separatorChar(',')
          {}
    
/*-------------------------------------------------------------------------
 *  Required Methods (must be implemented in each StData... class)
 *-------------------------------------------------------------------------*/


    /*======================================================================*/
    /*! 
     *   Specify, if getValue() should throw expcetions for unknown keys
     *
     *   \param f true: getValue() should throw expcetions for unknown keys
     *
     */
    /*======================================================================*/
    void setExceptionFlag( bool f)
          {
            _exceptionFlag = f;
          }
    

    /*======================================================================*/
    /*! 
     *   \return value of exceptionFlag (see setExceptionFlag())
     */
    /*======================================================================*/
    bool exceptionFlag() const
          {
            return _exceptionFlag;
          }


    /*======================================================================*/
    /*! 
     *   check wether a value exists for requested key
     *
     *   \param key the key
     *
     *   \return true or false
     */
    /*======================================================================*/
    bool valueExists(  const std::string& key) const
          {
            return (_map.find( key) != _map.end());
          }
    
    


    /*======================================================================*/
    /*! 
     *   setValue writes/adds a value (with arbitrary type) to the
     *   internal map<string,string>. 
     *
     *   \param key     the key for that value
     *   \param value   the value itself
     *
     */
    /*======================================================================*/
    template<typename T>
    void setValue( const std::string& key, const T& value);

    /*======================================================================*/
    /*! 
     *   setArray writes/adds an array of values (with arbitrary type)
     *   to the internal map<string,string>. 
     *
     *   \param key      the key for that value
     *   \param arrBegin begin of container
     *   \param size     number of elements
     *
     */
    /*======================================================================*/
    template<typename ForwardIter>
    void setArray( const std::string& key, 
                   const ForwardIter& arrBegin,
                   size_t size);
    
    
    /*======================================================================*/
    /*! 
     *   setFVPList writes/adds a list (or a 1D array) of feature vectors 
     *   to the internal map<string,string>. arrBegin must point to
     *   the begin of linear array with _pointers_ to the feature
     *   vectors, e.g. std::vector<BasicFV*>  
     *
     *   \param key      the key for that value
     *   \param arrBegin begin of container
     *   \param size     number of elements in the container
     *
     */
    /*======================================================================*/
    template<typename ForwardIter>
    void setFVArray( const std::string& key, 
                     const ForwardIter& arrBegin,
                     size_t size);
    
    
    /*======================================================================*/
    /*! 
     *   get a value (with arbitrary type) from the internal
     *   map<string,string> specified by the given key. If key is not
     *   found, the value remains unchanged and depending on
     *   exceptionFlag() a KeyNotFoundError exception is thrown.
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
    void getValue( const std::string& key, T& value) const;
    

    /*======================================================================*/
    /*! 
     *   get size of Array associated with given key.
     *
     *   \param key      the key for that value
     *
     *   \return  number of elements
     */
    /*======================================================================*/
    size_t getArraySize( std::string key) const;

    /*======================================================================*/
    /*! 
     *   get number of Feature vectors in the list associated with
     *   that kay
     *
     *   \param key      the key for that value
     *
     *   \return  number of elements
     *
     */
    /*======================================================================*/
    size_t getFVArraySize( std::string key) const; 


    /*======================================================================*/
    /*! 
     *   getArray. The values must be separated
     *   by separatorChar(), e.g. mymap["nr_sv"] = "150 120". See
     *   getValue() above for further details.
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
     */
    /*======================================================================*/
    template<typename ForwardIter> 
    void getArray( const std::string& key, const ForwardIter& arrBegin, 
                   int containerSize=-1) const;
    
    /*======================================================================*/
    /*! 
     *   get a list (or a 1D array) of feature vectors. The Iterator
     *   must point to a container with Feature vector _pointers_. The
     *   pointers must point to already allocated feature vectors
     *
     *   \param key     the key for the requested value
     *   \param arrBegin iterator pointing to your container, that has
     *                  enough space to store all values. You can
     *                  query the required size with
     *                  getFVArraySize(). 
     *   \param containerSize optional parameter to tell the size of
     *                  your container, so that an error can be
     *                  thrown, wenn it would be exeeded.
     *
     *   \exception KeyNotFoundError the requested key does not exist
     *   \exception InvalidDataTypeError the value string couldn't be
     *              converted to the requested type
     */
    /*======================================================================*/
    template<typename ForwardIter> 
    void getFVArray( const std::string& key, const ForwardIter& arrBegin, 
                    int containerSize=-1) const;
    


/*-------------------------------------------------------------------------
 *  Optional Methods that need not to be implemented in each
 *  StData... class
 *-------------------------------------------------------------------------*/

    /*======================================================================*/
    /*! 
     *   set character to separate elements of vectors within the
     *   value-string. Default is ','
     */
    /*======================================================================*/
    void setSeparatorChar( char c)
          {
            _separatorChar = c;
          }
    
    char separatorChar() const
          {
            return _separatorChar;
          }


    /*======================================================================*/
    /*! 
     *   return the associated string to the given key directly
     *
     *   \param key     the key for the requested value
     *
     *   \exception KeyNotFoundError the requested key does not exist
     *
     *   \return const reference to string
     */
    /*======================================================================*/
    const std::string& asString( const std::string& key) const
          {
            std::map<std::string, std::string>::const_iterator 
                p = _map.find( key);

            if (p == _map.end())
            {
              KeyNotFoundError e;
              e << "Key '" << key << "' not found.";
              throw e;
            }
            return p->second;
          }



    double asDouble( const std::string& key) const
          {
            double tmp = 0;
            getValue( key, tmp);
            return tmp;
          }
    
    
    unsigned int asUint( const std::string& key) const
          {
            unsigned int tmp = 0;
            getValue( key, tmp);
            return tmp;
          }
    
    
    /*======================================================================*/
    /*! 
     *   convert any std::string to any type using an std::istringstream. Has
     *   special overload for converting string to string, which does not
     *   split it at the first white space
     *
     *   \param s   string 
     *   \param val (output) value
     *
     *   \exception InvalidDataTypeError if conversion failed
     */
    /*======================================================================*/
    template<typename T>
    void stringToValue( const std::string& s, T& value) const
          {
            std::istringstream iss(s);
            if( !(iss >> value))
            {
              InvalidDataTypeError e;
              e << "string '" << s
                << "' could not be converted to requested type";
              throw e;
            }
          }
    
    // optimized version for strings
    void stringToValue( const std::string& s, std::string& value) const
          {
            value = s;
          }

    void readArraySizeFromStream( std::istream& is, size_t& size) const;
    

    /*======================================================================*/
    /*! 
     *   const access to internal map
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    std::map<std::string, std::string>::const_iterator begin() const
          {
            return _map.begin();
          }
    
    std::map<std::string, std::string>::const_iterator end() const
          {
            return _map.end();
          }

    /*======================================================================*/
    /*! 
     *   for debugging print whole internal map
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    void debugPrint( std::ostream& os) const
          {
            for(  std::map<std::string,std::string>::const_iterator
                      p = _map.begin(); p != _map.end(); ++p)
            {
              os << p->first << ' ' << p->second << std::endl;
            }
          }
            

    
  private:
    /*======================================================================*/
    /*! 
     *   find key in map and return iterator to it. If key does not
     *   exist an exception is thrown, depending on _exceptionFlag
     *
     *   \param key the key
     *
     *   \exception KeyNotFoundError
     *
     *   \return iterator to key or _map.end()
     */
    /*======================================================================*/

    std::map<std::string, std::string>::const_iterator 
    findKey( const std::string& key) const
          {
            std::map<std::string, std::string>::const_iterator p = 
                _map.find(key);
            if (p == _map.end())
            {
              if( _exceptionFlag)
              {
                KeyNotFoundError e;
                e << "Key '" << key << "' not found.";
                throw e;
              }
            }
            return p;
          }
    

    template< typename T>
    void setPrecisionForType( std::ostream& os, const T&) const
          {
            os.precision( std::numeric_limits<float>::digits10);
          }
    
    void setPrecisionForType( std::ostream& os, const double&) const
          { 
            os.precision( std::numeric_limits<double>::digits10);
          }
    
    
  protected:
    std::map<std::string, std::string> _map;
    bool _exceptionFlag;
    char _separatorChar;
  };
  
}


#include "StDataASCII.icc"
#endif
