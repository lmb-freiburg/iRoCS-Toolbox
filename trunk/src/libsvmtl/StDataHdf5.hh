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
**       Title: 
**    $RCSfile: $
**   $Revision: $$Name:  $
**       $Date: $
**   Copyright: GPL $Author: $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: $
**
**
**************************************************************************/

#ifndef STDATAHDF5_HH
#define STDATAHDF5_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <string>
#include <cstring>
#include <valarray>

#include <hdf5.h>
#include <H5Cpp.h>

#include "SVMError.hh"
#include "SvmH5Type.hh"
#include "svt_check/RequireHelpers.hh"  // for svt_check::avoidUnunsedVariableWarning();


namespace svt
{

  /*-----------------------------------------------------------------------
   *  conversion of c-data-types to Hdf5 data types
   *-----------------------------------------------------------------------*/
  template< typename DATATYPE>
  struct MyHdf5TypeTraits
  {
    typedef void value_type;
    //NcType ncType() { return ncNoType; }
  };
  
  // native types
  template<> struct MyHdf5TypeTraits<unsigned char>
  {
    typedef unsigned char value_type;
    //NcType ncType() { return ncByte; }
  };
  
  template<> struct MyHdf5TypeTraits<char>
  {
    typedef char value_type;
    //NcType ncType() { return ncChar; }
  };
  
  template<> struct MyHdf5TypeTraits<short>
  {
    typedef short value_type;
    //NcType ncType() { return ncShort; }
  };
  
  template<> struct MyHdf5TypeTraits<int>
  {
    typedef int value_type;
    //NcType ncType() { return ncInt; }
  };

  template<> struct MyHdf5TypeTraits<long>
  {
    typedef int value_type;
    //NcType ncType() { return ncInt; }
  };

  template<> struct MyHdf5TypeTraits<float>
  {
    typedef float value_type;
    //NcType ncType() { return ncFloat; }
  };
  
  template<> struct MyHdf5TypeTraits<double>
  {
    typedef double value_type;
    //NcType ncType() { return ncDouble; }
  };
  
  // mappings of non-native netCDF types
  template<> struct MyHdf5TypeTraits< unsigned int>
  {
    typedef int value_type;
    //NcType ncType() { return ncInt; }
  };
  
  template<> struct MyHdf5TypeTraits< bool>
  {
    typedef unsigned char value_type;
    //NcType ncType() { return ncByte; }
  };
  
  /*-------------------------------------------------------------------------
   *  set vec data
   *-------------------------------------------------------------------------*/
  template<typename T1, typename T2>
  bool _setVecData(T1& , T2 )  {
    return false;
  }
  template<>
  inline bool _setVecData(char*& vect, char* rs)  {
    vect = (char*)malloc(strlen(rs));
    if( vect == NULL) return false;
    strcpy( vect, rs);
    return true;
  }
  template<>
  inline bool _setVecData(std::string& vect, char* rs) {
    vect = std::string(rs);
    return true;
  }
  template<>
  inline bool _setVecData(char& value, char* rs) {
    std::istringstream iss(rs);
    iss >> value;
    return !(iss.bad() || iss.fail());
  }
  template<>
  inline bool _setVecData(short& value, char* rs) {
    std::istringstream iss(rs);
    iss >> value;
    return !(iss.bad() || iss.fail());
  }
  template<>
  inline bool _setVecData(unsigned short& value, char* rs) {
    std::istringstream iss(rs);
    iss >> value;
    return !(iss.bad() || iss.fail());
  }
  template<>
  inline bool _setVecData(int& value, char* rs) {
    std::istringstream iss(rs);
    iss >> value;
    return !(iss.bad() || iss.fail());
  }
  template<>
  inline bool _setVecData(unsigned int& value, char* rs) {
    std::istringstream iss(rs);
    iss >> value;
    return !(iss.bad() || iss.fail());
  }
  template<>
  inline bool _setVecData(long& value, char* rs) {
    std::istringstream iss(rs);
    iss >> value;
    return !(iss.bad() || iss.fail());
  }
  template<>
  inline bool _setVecData(unsigned long& value, char* rs) {
    std::istringstream iss(rs);
    iss >> value;
    return !(iss.bad() || iss.fail());
  }
  template<>
  inline bool _setVecData(long long& value, char* rs) {
    std::istringstream iss(rs);
    iss >> value;
    return !(iss.bad() || iss.fail());
  }
  inline bool _setVecData(unsigned long long& value, char* rs) {
    std::istringstream iss(rs);
    iss >> value;
    return !(iss.bad() || iss.fail());
  }
  template<>
  inline bool _setVecData(float& value, char* rs) {
    std::istringstream iss(rs);
    iss >> value;
    return !(iss.bad() || iss.fail());
  }
  template<>
  inline bool _setVecData(double& value, char* rs) {
    std::istringstream iss(rs);
    iss >> value;
    return !(iss.bad() || iss.fail());
  }
  template<>
  inline bool _setVecData(long double& value, char* rs) {
    std::istringstream iss(rs);
    iss >> value;
    return !(iss.bad() || iss.fail());
  }
   
  
  /*======================================================================*/
  /*!
   *  \class StDataHdf5
   *  \ingroup stdata
   *  \brief The StDataHdf5 class provies the "structured data"
   *         interface to Hdf5 files 
   *
   */
  /*======================================================================*/
  template<typename Hdf5FilePolicy>
  class StDataHdf5Templ : public Hdf5FilePolicy
  {
  public:
    
    /*======================================================================*/
    /*! 
     *   constructor. Tries to open the specified Hdf5 and throws an
     *   error, if it could not be opened
     *
     *   \param path filename of Hdf5 file
     *   \param fm   open mode (StDataHdf5::ReadOnly,
     *               StDataHdf5::Write, StDataHdf5::Replace,
     *               StDataHdf5::New) see Hdf5 documentaion for details
     *
     *   \exception LoadError can not open file
     *
     */
    /*======================================================================*/
    StDataHdf5Templ( const char * path, 
                  unsigned int fm = H5F_ACC_RDONLY);
    // H5F_ACC_RDWR write access
    
    /*======================================================================*/
    /*! 
     *   this constructor is only for compilation of some template
     *   classes. It just throws a runtime error. Never Use it!
     */
    /*======================================================================*/
    StDataHdf5Templ()
            :H5::H5File("",H5F_ACC_RDONLY)
          {
            svt::SVMAssertionFailed err;      
            err << "runtime error: you must not use StDataHdf5() "
                "constructor without arguments!";
            throw err;
            
          }
    

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
            return _existsAttribute(key,"");
          }

    /*======================================================================*/
    /*! 
     *   setValue writes/adds a value (with arbitrary type) to the
     *   netCDF File as attribute. 
     *
     *   \param key     the key for that value
     *   \param value   the value itself
     *
     */
    /*======================================================================*/
    template<typename T>
    void setValue( const std::string& key, const T& value);

    void setValue( const std::string& key, const std::string& value);
    void setValue( const std::string& key, const char* value)
          {
            setValue(key,std::string(value));
          }
    void setValue( const char* key, const char* value)
          {
            setValue(std::string(key),std::string(value));
          }
    void setValue( const char* key, const std::string& value)
          {
            setValue(std::string(key),std::string(value));
          }
    void setValue( const std::string& key, const unsigned int& value);
    void setValue( const std::string& key, const unsigned long& value);

    /*======================================================================*/
    /*! 
     *   setArray writes/adds an array of values (with arbitrary type)
     *   to the netCDF File as attribute. 
     *
     *   \param key      the key for that value
     *   \param arrBegin begin of container
     *   \param size     number of elements
     *
     */
    /*======================================================================*/
    void setArray(
        const std::string& key, 
        const std::vector<std::string>::iterator& arrBegin,
        size_t size);
    void setArray(
        const std::string& key, 
        const std::vector<std::string>::const_iterator& arrBegin,
        size_t size);

    template<typename ForwardIter>
    void setArray( const std::string& key, 
                   const ForwardIter& arrBegin,
                   size_t size);
    
    /*======================================================================*/
    /*! 
     *   setFVPList writes/adds a list (or a 1D array) of feature vectors 
     *   to the Hdf5 file as 2D Variable. arrBegin must point to
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
     *   get a value (with arbitrary type) from the Hdf5 attribute
     *   specified by the given key. If key is not  
     *   found, the value remains unchanged and depending on
     *   exceptionFlag() a KeyNotFoundError exception is thrown.
     *
     *   \param key     the key for the requested value
     *   \param value   (output) returned value.  
     *
     *   \exception KeyNotFoundError the requested key does not exist
     *   \exception InvalidDataTypeError the value couldn't be
     *              converted to the requested type
     */
    /*======================================================================*/
    template<typename T> 
    void getValue( const std::string& key, T& value) const;

    void getValue( const std::string& key, std::string& value) const;
    

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
    size_t getAttrArraySize( std::string key) const;

    /*======================================================================*/
    /*! 
     *   get number of Feature vectors in the list associated with
     *   that key
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
     *   \exception InvalidDataTypeError the value couldn't be
     *              converted to the requested type
     */
    /*======================================================================*/
    void getArray(
        const std::string& key, 
        const std::vector<std::string>::iterator& arrBegin,
        int containerSize=-1) const;
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

  protected:

    template<typename DataT>
    void
    _writeAttribute( const DataT& val,
                     const std::string& attName,
                     const std::string& dsName);

    template<typename DataT>
    void
    _readAttribute( DataT& val,
                    const std::string& attName,
                    const std::string& dsName) const;

    void _deleteAttribute(const std::string &attName,
                          const std::string &dsName)
          {
            if (dsName != "") // dataset attribute
            {
              hid_t dataset = _getDataSet(dsName);
              int status = H5Adelete(dataset, attName.c_str());
              H5Dclose(dataset);
              if (status < 0)
              {
                SaveError e;
                e << "Could not delete Attribute " 
                  << attName << " from dataset "
                  << dsName;
                throw(e);
              }
            }
            else // global attribute
            {
              hid_t group = _getGroup(dataSetGroup(attName));
              int status = H5Adelete(group, dataSetName(attName).c_str());
              H5Gclose(group);
              if (status < 0)
              {
                SaveError e;
                e << "Could not delete global Attribute " << attName;
                throw(e);
              }
            }
          }

    bool _existsAttribute(const std::string &attName,
                          const std::string &dsName) const
          {
            bool exists = false;
            try
            {
              hid_t attribute = _getAttribute(attName, dsName);
              H5Aclose(attribute);
              exists = true;
            }
            catch (LoadError&)
            {}

            return exists;
          }

    bool _existsDataSet(const std::string &absName) const
          {
            bool found = false;
            try
            {
              hid_t dataset = _getDataSet(absName);
              H5Dclose(dataset);
              found = true;
            }
            catch (LoadError&)
            {}
            return found;
          }

    hid_t _getGroup(const std::string &groupName) const
          {
            hid_t group = H5Gopen2(
                this->getLocId(), groupName.c_str(), H5P_DEFAULT);
            if (group < 0)
            {
              LoadError e;
              e << "Could not open group " << group;
              throw(e);
            }
            return group;
          }

    hid_t _getDataSet(const std::string &dsName) const
          {
            hid_t dataset = H5Dopen2(
                this->getLocId(), dsName.c_str(), H5P_DEFAULT);
            if (dataset < 0)
            {
              LoadError e;
              e << "Could not open dataset " << dsName;
              throw(e);
            }
            return dataset;
          }

    hid_t _getAttribute(const std::string &attName,
                        const std::string &dsName) const
          {
            // get a file handle
            hid_t attribute = -1;

            if (dsName != "") // Dataset attribute
            {
              hid_t dataset = -1;
              try
              {
                dataset = _getDataSet(dsName);
                attribute = H5Aopen_name(dataset, attName.c_str());
              }
              catch(...)
              {
                attribute = -1;
              }
              H5Dclose(dataset);
              if (attribute < 0)
              {
                LoadError e;
                e << "Could not retrieve attribute " 
                  << attName << " from dataset "
                  << dsName << ". Attribute " << attName << " doesn't exist!";
                throw(e);
              }
            }
            else // global attribute
            {
              hid_t group = -1;
              try
              {
                group = _getGroup(dataSetGroup(attName));
                attribute = H5Aopen_name(group, dataSetName(attName).c_str());
              }
              catch(...)
              {
                attribute = -1;
              }
              H5Gclose(group);
              if (attribute < 0)
              {
                LoadError e;
                e << "Could not retrieve global attribute " << attName;
                throw(e);
              }
            }
            return attribute;
          }

    std::string dataSetGroup(const std::string& dataSetDescriptor) const
          {
            // Dataset descriptor is free of group information
            if (dataSetDescriptor.find("/") == std::string::npos)
            {
              return std::string("/");
            }

            // Strip dataset name
            return dataSetDescriptor.substr(0, 
                                            dataSetDescriptor.rfind("/") + 1);
          }

    std::string dataSetName(const std::string& dataSetDescriptor) const
          {
            // Dataset descriptor is empty or ends with /
            if (dataSetDescriptor.size() == 0 ||
                dataSetDescriptor[dataSetDescriptor.size() - 1] == '/')
            {
              return std::string("");
            }

            // Dataset descriptor is free of group information
            if (dataSetDescriptor.find("/") == std::string::npos)
            {
              return dataSetDescriptor;
            }

            // Strip group information
            return dataSetDescriptor.substr(dataSetDescriptor.rfind("/") + 1);
          }

    void _createGroup(const std::string& groupName)
          {
            std::vector<std::string> groupHierarchy;

            size_t beginPos = 1;
            size_t delim = beginPos;
            while(beginPos < groupName.size())
            {
              delim = groupName.find("/", beginPos);
              std::string group = groupName.substr(0, delim + 1);
              groupHierarchy.push_back(group);
              beginPos = delim + 1;
            }

            // create groups recursively
            for(std::vector<std::string>::const_iterator
                    groupIt = groupHierarchy.begin(); 
                groupIt != groupHierarchy.end();
                ++groupIt)
            {
              if(!_existsGroup(groupIt->c_str()))
              {
                hid_t group = H5Gcreate2(
                    this->getLocId(), groupIt->c_str(), H5P_DEFAULT,
                    H5P_DEFAULT, H5P_DEFAULT);
                if( group < 0)
                {
                  SaveError e;
                  e << "Could not create group: '" 
                    << groupIt->c_str() << "'.\n";
                  throw(e);
                }
                H5Gclose(group);
              }
            }
          }

    bool _existsGroup(const std::string &groupName) const
          {
            bool exists = false;
            try
            {
              hid_t group = _getGroup(groupName);
              H5Gclose(group);
              exists = true;
            }
            catch (LoadError&)
            {}
            return exists;
          }

    static void getValue(
        std::valarray<double> const &att, bool& value, size_t index)
          {
            value = static_cast<bool>(att[index]);
          }

    static void getValue(
        std::valarray<double> const &att, char& value, size_t index)
          {
            value = static_cast<char>(att[index]);
          }
    
    static void getValue(
        std::valarray<double> const &att, short& value, size_t index)
          {
            value = static_cast<short>(att[index]);
          }
    
    static void getValue(
        std::valarray<double> const &att, int& value, long index)
          {
            value = static_cast<int>(att[index]);
          }
    
    static void getValue(
        std::valarray<double> const &att, unsigned int& value, long index)
          {
            value = static_cast<unsigned int>(att[index]);
          }
    
    static void getValue(
        std::valarray<double> const &att, float& value, long index)
          {
            value = static_cast<float>(att[index]);
          }
    
    static void getValue(
        std::valarray<double> const &att, double& value, long index)
          {
            value = static_cast<double>(att[index]);
          }

/*-------------------------------------------------------------------------
 *  size and data of std::vector
 *-------------------------------------------------------------------------*/
    template<typename T>
    inline hsize_t _getVecSize( const std::vector<T>& vect) const
          {
            return (hsize_t)vect.size();
          }

    template<typename T>
    inline void _setVecSize( std::vector<T>& vect, size_t size) const
          {
            vect.resize(size);
          }

    template<typename T>
    inline const T* _getVecData( const std::vector<T>& vect) const
          {
            return &(vect[0]);
          }

    template<typename T>
    inline T* _getVecData2( std::vector<T>& vect) const
          {
            return &(vect[0]);
          }
/*-------------------------------------------------------------------------
 *  size and data of std::valarray
 *-------------------------------------------------------------------------*/
    template<typename T>
    inline hsize_t _getVecSize( const std::valarray<T>& vect) const
          {
            return (hsize_t)vect.size();
          }

    template<typename T>
    inline void _setVecSize( std::valarray<T>& vect, size_t size) const
          {
            vect.resize(size);
          }

    template<typename T>
    inline const T* _getVecData( const std::valarray<T>& vect) const
          {
            return &(vect[0]);
          }

    template<typename T>
    inline T* _getVecData2( std::valarray<T>& vect) const
          {
            return &(vect[0]);
          }
/*-------------------------------------------------------------------------
 *  size and data of std::string
 *-------------------------------------------------------------------------*/
    inline hsize_t _getVecSize( const std::string& vect) const
          {
            return (hsize_t)vect.size();
          }

    inline void _setVecSize( std::string& vect, size_t size) const
          {
            vect.resize(size);
          }

    inline const char* _getVecData( const std::string& vect) const
          {
            return vect.c_str();
          }

    inline const char* _getVecData( const char*& vect) const
          {
            return vect;
          }


/*-------------------------------------------------------------------------
 *  size and data of integral types
 *-------------------------------------------------------------------------*/
    template<typename T>
    inline hsize_t _getVecSize( const T& ) const
          {
            return 1;
          }

    template<typename T>
    inline void _setVecSize( T& , size_t ) const
          {
            // do nothing
          }

    template<typename T>
    inline const T* _getVecData( const T& vect) const
          {
            return &vect;
          }

    template<typename T>
    inline T* _getVecData2( T& vect) const
          {
            return &vect;
          }

    inline
    std::string
    _getErrorMsg()
          {
            std::string errorMsg;

#if ! (defined(_WIN32) || defined(__APPLE__))
            char *bp;
            size_t size;
            FILE *stream;

            // open_memstream is probably only available on Linux :-(
            stream = open_memstream( &bp, &size);

            H5Eprint2(H5E_DEFAULT, stream);
            //fflush( stream);
            char ch;
            while( (ch = static_cast<char>(fgetc( stream))) != EOF)
                errorMsg += ch;
            fclose( stream);
#endif
            return errorMsg;
          }

    class Buffer {
    public:

      Buffer(const hid_t& dataSetType, const hsize_t size);
      ~Buffer();

      void* ptr();

      inline void castToType(std::string *dest);
      
      template<typename T>
      inline void castToType( T* dest);

    private:
      template<typename T1, typename T2>
      inline void _doRealCast( T1* dest);

      H5T_class_t _dataSetClass;
      size_t      _precision;
      H5T_sign_t  _isSigned;
      size_t      _size;
      size_t      _sizeOfbufferType;
      void*       _buffer_ptr;
    }; // class Buffer

//   /*-------------------------------------------------------------------------
//    *  set vec data
//    *-------------------------------------------------------------------------*/
// template<typename T1, typename T2>
// bool _setVecData(T1& , T2 )  {
//   return false;
// }
  private:
    bool _exceptionFlag;
    std::string _fileName;
    std::string _varName;
    
  };

  typedef StDataHdf5Templ<H5::H5File> StDataHdf5;
  
  
}

#include "StDataHdf5.icc"
#endif
