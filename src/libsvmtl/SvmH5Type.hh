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

#ifndef SVMHDF5TYPE_HH
#define SVMHDF5TYPE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <vector>
#include <limits>

namespace svt 
{
//! Type identifiers
enum TypeIdentifier {
    NO_TYPE,
    TYPE_CHAR,
    TYPE_UCHAR,
    TYPE_SHORT,
    TYPE_USHORT,
    TYPE_INT,
    TYPE_UINT,
    TYPE_LONG,
    TYPE_ULONG,
    TYPE_LLONG,
    TYPE_ULLONG,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_STRING
};

/*======================================================================*/
/*!
 *  \struct BlitzH5Type SvmH5Type.hh "libsvmtl/SvmH5Type.hh"
 *
 *  Default type trait.
 */
/*======================================================================*/
template< typename T>
struct BlitzH5Type {
  static const TypeIdentifier dataType = NO_TYPE;
  static H5::DataType h5DataType()
        {
          SVMError err;
          err << "BlitzH5Type::NO_TYPE: Data type not supported.";
          //throw(err);
          return H5::DataType();
          
        }
  static hid_t h5DataTypeId()
        {
          SVMError err;
          err << "BlitzH5Type::NO_TYPE: Data type not supported.";
          //throw(err);
          return H5::DataType();
        }
  static bool isVectorial()
        {
          SVMError err;
          err << "BlitzH5Type::NO_TYPE: Data type not supported.";
          //throw(err);
          return H5::DataType();
        }
};

//! bool version
template<>
struct BlitzH5Type<bool>
{
  static const TypeIdentifier dataType = TYPE_UCHAR;
  static H5::DataType h5DataType()
        {
          return H5::PredType::NATIVE_UCHAR;
        }
  static hid_t h5DataTypeId()
        {
          return H5Tcopy(H5T_NATIVE_UCHAR);
        }
  static bool isVectorial()
        {
          return false;
        }
};

//! char version
template<>
struct BlitzH5Type<signed char>
{
  static const TypeIdentifier dataType = TYPE_CHAR;
  static H5::DataType h5DataType()
        {
          return H5::PredType::NATIVE_CHAR;
        }
  static hid_t h5DataTypeId()
        {
          return H5Tcopy(H5T_NATIVE_CHAR);
        }
  static bool isVectorial()
        {
          return false;
        }
};

template<>
struct BlitzH5Type<char>
{
  static const TypeIdentifier dataType = TYPE_CHAR;
  static H5::DataType h5DataType()
        {
          return H5::PredType::NATIVE_CHAR;
        }
  static hid_t h5DataTypeId()
        {
          return H5Tcopy(H5T_NATIVE_CHAR);
        }
  static bool isVectorial()
        {
          return false;
        }
};

//! uchar version
template<>
struct BlitzH5Type<unsigned char>
{
  static const TypeIdentifier dataType = TYPE_UCHAR;
  static H5::DataType h5DataType()
        {
          return H5::PredType::NATIVE_UCHAR;
        }
  static hid_t h5DataTypeId()
        {
          return H5Tcopy(H5T_NATIVE_UCHAR);
        }
  static bool isVectorial()
        {
          return false;
        }
};

//! short version
template<>
struct BlitzH5Type<short>
{
  static const TypeIdentifier dataType = TYPE_SHORT;
  static H5::DataType h5DataType()
        {
          return H5::PredType::NATIVE_SHORT;
        }
  static hid_t h5DataTypeId()
        {
          return H5Tcopy(H5T_NATIVE_SHORT);
        }
  static bool isVectorial()
        {
          return false;
        }
};

//! ushort version
template<>
struct BlitzH5Type<unsigned short>
{
  static const TypeIdentifier dataType = TYPE_USHORT;
  static H5::DataType h5DataType()
        {
          return H5::PredType::NATIVE_USHORT;
        }
  static hid_t h5DataTypeId()
        {
          return H5Tcopy(H5T_NATIVE_USHORT);
        }
  static bool isVectorial()
        {
          return false;
        }
};

//! int version
template<>
struct BlitzH5Type<int>
{
  static const TypeIdentifier dataType = TYPE_INT;
  static H5::DataType h5DataType()
        {
          return H5::PredType::NATIVE_INT;
        }
  static hid_t h5DataTypeId()
        {
          return H5Tcopy(H5T_NATIVE_INT);
        }
  static bool isVectorial()
        {
          return false;
        }
};

//! uint version
template<>
struct BlitzH5Type<unsigned int>
{
  static const TypeIdentifier dataType = TYPE_UINT;
  static H5::DataType h5DataType()
        {
          return H5::PredType::NATIVE_UINT;
        }
  static hid_t h5DataTypeId()
        {
          return H5Tcopy(H5T_NATIVE_UINT);
        }
  static bool isVectorial()
        {
          return false;
        }
};

//! long version
template<>
struct BlitzH5Type<long>
{
  static const TypeIdentifier dataType = TYPE_LONG;
  static H5::DataType h5DataType()
        {
          return H5::PredType::NATIVE_LONG;
        }
  static hid_t h5DataTypeId()
        {
          return H5Tcopy(H5T_NATIVE_LONG);
        }
  static bool isVectorial()
        {
          return false;
        }
};

//! ulong version
template<>
struct BlitzH5Type<unsigned long>
{
  static const TypeIdentifier dataType = TYPE_ULONG;
  static H5::DataType h5DataType()
        {
          return H5::PredType::NATIVE_ULONG;
        }
  static hid_t h5DataTypeId()
        {
          return H5Tcopy(H5T_NATIVE_ULONG);
        }
  static bool isVectorial()
        {
          return false;
        }
};

//! llong version
template<>
struct BlitzH5Type<long long>
{
  static const TypeIdentifier dataType = TYPE_LLONG;
  static H5::DataType h5DataType()
        {
          return H5::PredType::NATIVE_LLONG;
        }
  static hid_t h5DataTypeId()
        {
          return H5Tcopy(H5T_NATIVE_LLONG);
        }
  static bool isVectorial()
        {
          return false;
        }
};

//! ullong version
template<>
struct BlitzH5Type<unsigned long long>
{
  static const TypeIdentifier dataType = TYPE_ULLONG;
  static H5::DataType h5DataType()
        {
          return H5::PredType::NATIVE_ULLONG;
        }
  static hid_t h5DataTypeId()
        {
          return H5Tcopy(H5T_NATIVE_ULLONG);
        }
  static bool isVectorial()
        {
          return false;
        }
};

//! float version
template<>
struct BlitzH5Type<float>
{
  static const TypeIdentifier dataType = TYPE_FLOAT;
  static H5::DataType h5DataType()
        {
          return H5::PredType::NATIVE_FLOAT;
        }
  static hid_t h5DataTypeId()
        {
          return H5Tcopy(H5T_NATIVE_FLOAT);
        }
  static bool isVectorial()
        {
          return false;
        }
};

//! double version
template<>
struct BlitzH5Type<double>
{
  static const TypeIdentifier dataType = TYPE_DOUBLE;
  static H5::DataType h5DataType()
        {
          return H5::PredType::NATIVE_DOUBLE;
        }
  static hid_t h5DataTypeId()
        {
          return H5Tcopy(H5T_NATIVE_DOUBLE);
        }
  static bool isVectorial()
        {
          return false;
        }
};

// //! complex version
// template<typename T>
// struct BlitzH5Type< std::complex<T> >
// {
//   static const TypeIdentifier dataType = BlitzH5Type<T>::dataType;
//   static H5::DataType h5DataType()
//         {
//           return BlitzH5Type<T>::h5DataType();
//         }
//   static hid_t h5DataTypeId()
//         {
//           return BlitzH5Type<T>::h5DataTypeId();
//         }
//   static bool isVectorial()
//         {
//           return true;
//         }
// };

// //! TinyVector version
// template<typename T, int Dim>
// struct BlitzH5Type<blitz::TinyVector<T,Dim> >
// {
//   static const TypeIdentifier dataType = BlitzH5Type<T>::dataType;
//   static H5::DataType h5DataType()
//         {
//           return BlitzH5Type<T>::h5DataType();
//         }
//   static hid_t h5DataTypeId()
//         {
//           return BlitzH5Type<T>::h5DataTypeId();
//         }
//   static bool isVectorial()
//         {
//           return true;
//         }
// };

// //! TinyMatrix version
// template<typename T, int N, int M>
// struct BlitzH5Type<blitz::TinyMatrix<T,N,M> >
// {
//   static const TypeIdentifier dataType = BlitzH5Type<T>::dataType;
//   static H5::DataType h5DataType()
//         {
//           return BlitzH5Type<T>::h5DataType();
//         }
//   static hid_t h5DataTypeId()
//         {
//           return BlitzH5Type<T>::h5DataTypeId();
//         }
//   static bool isVectorial()
//         {
//           return true;
//         }
// };

//! std::vector version
template<typename T>
struct BlitzH5Type<std::vector<T> >
{
  static const TypeIdentifier dataType = BlitzH5Type<T>::dataType;
  static H5::DataType h5DataType()
        {
          return BlitzH5Type<T>::h5DataType();
        }
  static hid_t h5DataTypeId()
        {
          return BlitzH5Type<T>::h5DataTypeId();
        }
  static bool isVectorial()
        {
          return true;
        }
};

//! std::valarray version
template<typename T>
struct BlitzH5Type<std::valarray<T> >
{
  static const TypeIdentifier dataType = BlitzH5Type<T>::dataType;
  static H5::DataType h5DataType()
        {
          return BlitzH5Type<T>::h5DataType();
        }
  static hid_t h5DataTypeId()
        {
          return BlitzH5Type<T>::h5DataTypeId();
        }
  static bool isVectorial()
        {
          return true;
        }
};

//! string version
// From http://www.zib.de/benger/hdf5/Datatypes.html
//      3.6 Character and String Datatype Issues
// it seems that a good way to store strings is as an array
// of single H5T_C_S1 characters.
template<>
struct BlitzH5Type<std::string>
{
  static const TypeIdentifier dataType = TYPE_STRING;
  static H5::DataType h5DataType()
        {
          // old style 'variable length' strings where:
          //H5::StrType datatype(H5::PredType::C_S1, H5T_VARIABLE);
          H5::StrType datatype(H5::PredType::C_S1);
          datatype.setStrpad(H5T_STR_NULLTERM);
          //datatype.setStrpad(H5T_STR_NULLPAD);
          return datatype;
        }
  static hid_t h5DataTypeId()
        {
          hid_t datatype = H5Tcopy(H5T_C_S1);
          // old style 'variable length' strings where:
          //H5Tset_size(datatype, H5T_VARIABLE);
          H5Tset_strpad(datatype, H5T_STR_NULLTERM);
          //H5Tset_strpad(datatype, H5T_STR_NULLPAD);
          return datatype;
        }
  static bool isVectorial()
        {
          return true;
        }
};
/*
template<>
struct BlitzH5Type<char[]>
{
  static const TypeIdentifier dataType = TYPE_STRING;
  static H5::DataType h5DataType()
        {
          H5::StrType datatype(H5::PredType::C_S1);
          datatype.setStrpad(H5T_STR_NULLPAD);
          return datatype;
        }
  static hid_t h5DataTypeId()
        {
          hid_t datatype = H5Tcopy(H5T_C_S1);
          H5Tset_strpad(datatype, H5T_STR_NULLPAD);
          return datatype;
        }
  static bool isVectorial()
        {
          return true;
        }
};
*/
//! std::vector<std::string> version
template<>
struct BlitzH5Type<std::vector<std::string> >
{
  static const TypeIdentifier dataType = NO_TYPE;
  static H5::DataType h5DataType()
        {
          SVMError err;
          err << "std::vector<std::string> not supported yet.";
          throw(err);
        }
  static hid_t h5DataTypeId()
        {
          SVMError err;
          err << "std::vector<std::string> not supported yet.";
          throw(err);
        }
  static bool isVectorial()
        {
          SVMError err;
          err << "std::vector<std::string> not supported yet.";
          throw(err);
        }
};


/*-------------------------------------------------------------------------
 *  some common type comparison operators, for the getAndConvert method
 *-------------------------------------------------------------------------*/
template< typename WantedType, typename DataFileType >
struct BlitzH5TypeConversion
{
  static bool exists()
  {
    return sizeof(WantedType) >= sizeof(DataFileType);
  }
  static bool sameType()
  {
    return false;
  }
  static bool sameSign()
  {
    // ToDo: Ugly workaround: Without the cast the compiler throws warnings
    //       for unsigned types, the cast to double remedies that, but
    //       might have bad side effects
    return
        ((static_cast<double>(std::numeric_limits<DataFileType>::min()) < 0 &&
            static_cast<double>(std::numeric_limits<WantedType>::min()) < 0) ||
         (static_cast<double>(std::numeric_limits<DataFileType>::min()) >= 0 &&
            static_cast<double>(std::numeric_limits<WantedType>::min()) >= 0));
  }
  static WantedType scaleFactor()
  {
    WantedType maxWanted = std::numeric_limits<WantedType>::max();
    DataFileType maxDataFile = std::numeric_limits<DataFileType>::max();
    if( maxWanted < maxDataFile)
    {
      return std::numeric_limits<WantedType>::max();
    }
    return WantedType(1);
  }
  static WantedType wantedTypeMin()
  {
    return std::numeric_limits<WantedType>::min();
  }

  static WantedType wantedTypeMax()
  {
    return std::numeric_limits<WantedType>::max();
  }
};


/*-------------------------------------------------------------------------
 *  some common type comparison operators, specialization for equal types
 *-------------------------------------------------------------------------*/
template< typename DataType >
struct BlitzH5TypeConversion<DataType,DataType>
{
  static bool exists()
  {
    return true;
  }
  static bool sameType()
  {
    return true;
  }
  static bool sameSign()
  {
    return true;
  }
  static DataType scaleFactor()
  {
    return DataType(1);
  }
  static DataType wantedTypeMin()
  {
    return std::numeric_limits<DataType>::min();
  }

  static DataType wantedTypeMax()
  {
    return std::numeric_limits<DataType>::max();
  }
};


// /*-------------------------------------------------------------------------
//  *  some common type comparison operators, specialization for TinyVector
//  *-------------------------------------------------------------------------*/
// template< typename WantedType, typename DataFileType, int N>
// struct BlitzH5TypeConversion<WantedType, blitz::TinyVector<DataFileType,N> >
// {
//   static bool exists()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::exists();
//   }
//   static bool sameType()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::sameType();
//   }
//   static bool sameSign()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::sameSign();
//   }
//   static WantedType scaleFactor()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::scaleFactor();
//   }
//   static WantedType wantedTypeMin()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::wantedTypeMin();
//   }

//   static WantedType wantedTypeMax()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::wantedTypeMax();
//   }
// };


// /*-------------------------------------------------------------------------
//  *  some common type comparison operators, specialization for TinyVector
//  *-------------------------------------------------------------------------*/
// template< typename WantedType, typename DataFileType, int N>
// struct BlitzH5TypeConversion<blitz::TinyVector<WantedType,N>, DataFileType>
// {
//   static bool exists()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::exists();
//   }
//   static bool sameType()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::sameType();
//   }
//   static bool sameSign()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::sameSign();
//   }
//   static WantedType scaleFactor()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::scaleFactor();
//   }
//   static WantedType wantedTypeMin()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::wantedTypeMin();
//   }

//   static WantedType wantedTypeMax()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::wantedTypeMax();
//   }
// };


// /*-------------------------------------------------------------------------
//  *  some common type comparison operators, specialization for std::complex
//  *-------------------------------------------------------------------------*/
// template< typename WantedType, typename DataFileType>
// struct BlitzH5TypeConversion<WantedType, std::complex<DataFileType> >
// {
//   static bool exists()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::exists();
//   }
//   static bool sameType()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::sameType();
//   }
//   static bool sameSign()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::sameSign();
//   }
//   static WantedType scaleFactor()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::scaleFactor();
//   }
//   static WantedType wantedTypeMin()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::wantedTypeMin();
//   }
//   static WantedType wantedTypeMax()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::wantedTypeMax();
//   }
// };


// /*-------------------------------------------------------------------------
//  *  some common type comparison operators, specialization for std::complex
//  *-------------------------------------------------------------------------*/
// template< typename WantedType, typename DataFileType>
// struct BlitzH5TypeConversion<std::complex<DataFileType>, WantedType>
// {
//   static bool exists()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::exists();
//   }
//   static bool sameType()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::sameType();
//   }
//   static bool sameSign()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::sameSign();
//   }
//   static WantedType scaleFactor()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::scaleFactor();
//   }
//   static WantedType wantedTypeMin()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::wantedTypeMin();
//   }
//   static WantedType wantedTypeMax()
//   {
//     return BlitzH5TypeConversion<WantedType, DataFileType>::wantedTypeMax();
//   }
// };
}
#endif

