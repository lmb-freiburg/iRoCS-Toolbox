#ifndef BLITZH5TRAITS_HH
#define BLITZH5TRAITS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <hdf5.h>
#include <blitz/array.h>

#include <vector>

template<typename InnerT>
struct BlitzH5Traits
{
  
  typedef InnerT DataT;
  typedef InnerT BasicT;
  
  static std::vector<hsize_t> h5Dims(DataT const &array);
  static hid_t h5Type();
  static void *data(DataT &array);
  static void const *data(DataT const &array);
  
};

template<>
struct BlitzH5Traits<unsigned char>
{
  typedef unsigned char DataT;
  typedef unsigned char BasicT;
  static std::vector<hsize_t> h5Dims(DataT const &);
  static hid_t h5Type();
  static void *data(DataT &array);
  static void const *data(DataT const &array);
};

template<>
struct BlitzH5Traits<char>
{
  typedef char DataT;
  typedef char BasicT;
  static std::vector<hsize_t> h5Dims(DataT const &);
  static hid_t h5Type();
  static void *data(DataT &array);
  static void const *data(DataT const &array);
};

template<>
struct BlitzH5Traits<unsigned short>
{
  typedef unsigned short DataT;
  typedef unsigned short BasicT;
  static std::vector<hsize_t> h5Dims(DataT const &);
  static hid_t h5Type();
  static void *data(DataT &array);
  static void const *data(DataT const &array);
};

template<>
struct BlitzH5Traits<short>
{
  typedef short DataT;
  typedef short BasicT;
  static std::vector<hsize_t> h5Dims(DataT const &);
  static hid_t h5Type();
  static void *data(DataT &array);
  static void const *data(DataT const &array);
};

template<>
struct BlitzH5Traits<unsigned int>
{
  typedef unsigned int DataT;
  typedef unsigned int BasicT;
  static std::vector<hsize_t> h5Dims(DataT const &);
  static hid_t h5Type();
  static void *data(DataT &array);
  static void const *data(DataT const &array);
};

template<>
struct BlitzH5Traits<int>
{
  typedef int DataT;
  typedef int BasicT;
  static std::vector<hsize_t> h5Dims(DataT const &);
  static hid_t h5Type();
  static void *data(DataT &array);
  static void const *data(DataT const &array);
};

template<>
struct BlitzH5Traits<unsigned long>
{
  typedef unsigned long DataT;
  typedef unsigned long BasicT;
  static std::vector<hsize_t> h5Dims(DataT const &);
  static hid_t h5Type();
  static void *data(DataT &array);
  static void const *data(DataT const &array);
};

template<>
struct BlitzH5Traits<long>
{
  typedef long DataT;
  typedef long BasicT;
  static std::vector<hsize_t> h5Dims(DataT const &);
  static hid_t h5Type();
  static void *data(DataT &array);
  static void const *data(DataT const &array);
};

template<>
struct BlitzH5Traits<unsigned long long>
{
  typedef unsigned long long DataT;
  typedef unsigned long long BasicT;
  static std::vector<hsize_t> h5Dims(DataT const &);
  static hid_t h5Type();
  static void *data(DataT &array);
  static void const *data(DataT const &array);
};

template<>
struct BlitzH5Traits<long long>
{
  typedef long long DataT;
  typedef long long BasicT;
  static std::vector<hsize_t> h5Dims(DataT const &);
  static hid_t h5Type();
  static void *data(DataT &array);
  static void const *data(DataT const &array);
};

template<>
struct BlitzH5Traits<float>
{   
  typedef float DataT;
  typedef float BasicT;
  static std::vector<hsize_t> h5Dims(DataT const &);
  static hid_t h5Type();
  static void *data(DataT &array);
  static void const *data(DataT const &array);
};

template<>
struct BlitzH5Traits<double>
{
  typedef double DataT;
  typedef double BasicT;
  static std::vector<hsize_t> h5Dims(DataT const &);
  static hid_t h5Type();
  static void *data(DataT &array);
  static void const *data(DataT const &array);
};

template<>
struct BlitzH5Traits<long double>
{
  typedef long double DataT;
  typedef long double BasicT;
  static std::vector<hsize_t> h5Dims(DataT const &);
  static hid_t h5Type();
  static void *data(DataT &array);
  static void const *data(DataT const &array);
};

template<typename InnerT>
struct BlitzH5Traits< std::vector<InnerT> >
{
    
  typedef std::vector<InnerT> DataT;
  typedef InnerT BasicT;

  static std::vector<hsize_t> h5Dims(DataT const &array)
        {
          std::vector<hsize_t> dims(1);
          dims[0] = array.size();
          return dims;
        }
    
  static hid_t h5Type()
        {
          return BlitzH5Traits<InnerT>::h5Type();
        }

  static void *data(DataT &array)
        {
          return reinterpret_cast<void*>(&array[0]);
        }

  static void const *data(DataT const &array)
        {
          return reinterpret_cast<void const*>(&array[0]);
        }

};

template<typename InnerT, int Dim>
struct BlitzH5Traits< blitz::TinyVector<InnerT,Dim> >
{
    
  typedef blitz::TinyVector<InnerT,Dim> DataT;
  typedef InnerT BasicT;

  static std::vector<hsize_t> h5Dims(DataT const &)
        {
          std::vector<hsize_t> dims(1);
          dims[0] = Dim;
          return dims;
        }
    
  static hid_t h5Type()
        {
          return BlitzH5Traits<InnerT>::h5Type();
        }

  static void *data(DataT &array)
        {
          return reinterpret_cast<void*>(array.data());
        }

  static void const *data(DataT const &array)
        {
          return reinterpret_cast<void const*>(array.data());
        }

};

template<typename InnerT, int NRows, int NCols>
struct BlitzH5Traits< blitz::TinyMatrix<InnerT,NRows,NCols> >
{
    
  typedef blitz::TinyMatrix<InnerT,NRows,NCols> DataT;
  typedef InnerT BasicT;

  static std::vector<hsize_t> h5Dims(DataT const &)
        {
          std::vector<hsize_t> dims(2);
          dims[0] = NRows;
          dims[1] = NCols;
          return dims;
        }
    
  static hid_t h5Type()
        {
          return BlitzH5Traits<InnerT>::h5Type();
        }

  static void *data(DataT &array)
        {
          return reinterpret_cast<void*>(array.data());
        }

  static void const *data(DataT const &array)
        {
          return reinterpret_cast<void const*>(array.data());
        }

};

template<typename InnerT>
struct BlitzH5Traits< std::complex<InnerT> >
{
    
  typedef std::complex<InnerT> DataT;
  typedef InnerT BasicT;

  static std::vector<hsize_t> h5Dims(DataT const &)
        {
          std::vector<hsize_t> dims(1);
          dims[0] = 2;
          return dims;
        }
    
  static hid_t h5Type()
        {
          return BlitzH5Traits<InnerT>::h5Type();
        }

  static void *data(DataT &array)
        {
          return reinterpret_cast<void*>(array.data());
        }

  static void const *data(DataT const &array)
        {
          return reinterpret_cast<void const*>(array.data());
        }

};

template<typename InnerT, int Dim>
struct BlitzH5Traits< blitz::Array<InnerT,Dim> >
{
    
  typedef blitz::Array<InnerT,Dim> DataT;
  typedef InnerT BasicT;

  static std::vector<hsize_t> h5Dims(DataT const &array)
        {
          std::vector<hsize_t> dims(Dim);
          for (int d = 0; d < Dim; ++d) dims[d] = array.extent(d);
          return dims;
        }
    
  static hid_t h5Type()
        {
          return BlitzH5Traits<InnerT>::h5Type();
        }

  static void *data(DataT &array)
        {
          return reinterpret_cast<void*>(array.data());
        }

  static void const *data(DataT const &array)
        {
          return reinterpret_cast<void const*>(array.data());
        }

};

template<typename InnerT, int Dim>
struct BlitzH5Traits<
    blitz::Array<std::complex<InnerT>,Dim> >
{
    
  typedef blitz::Array<std::complex<InnerT>,Dim> DataT;
  typedef InnerT BasicT;

  static std::vector<hsize_t> h5Dims(DataT const &array)
        {
          std::vector<hsize_t> dims(Dim + 1);
          for (int d = 0; d < Dim; ++d) dims[d] = array.extent(d);
          dims[Dim] = 2;
          return dims;
        }
    
  static hid_t h5Type()
        {
          return BlitzH5Traits<InnerT>::h5Type();
        }

  static void *data(DataT &array)
        {
          return reinterpret_cast<void*>(array.data());
        }

  static void const *data(DataT const &array)
        {
          return reinterpret_cast<void const*>(array.data());
        }
};

template<typename InnerT, int InnerDim, int OuterDim>
struct BlitzH5Traits<
    blitz::Array<blitz::TinyVector<InnerT,InnerDim>,OuterDim> >
{
    
  typedef blitz::Array<blitz::TinyVector<InnerT,InnerDim>,OuterDim> DataT;
  typedef InnerT BasicT;

  static std::vector<hsize_t> h5Dims(DataT const &array)
        {
          std::vector<hsize_t> dims(OuterDim + 1);
          for (int d = 0; d < OuterDim; ++d) dims[d] = array.extent(d);
          dims[OuterDim] = InnerDim;
          return dims;
        }
    
  static hid_t h5Type()
        {
          return BlitzH5Traits<InnerT>::h5Type();
        }

  static void *data(DataT &array)
        {
          return reinterpret_cast<void*>(array.data());
        }

  static void const *data(DataT const &array)
        {
          return reinterpret_cast<void const*>(array.data());
        }
};

template<typename InnerT, int InnerDim1, int InnerDim2, int OuterDim>
struct BlitzH5Traits<
    blitz::Array<blitz::TinyMatrix<InnerT,InnerDim1,InnerDim2>,OuterDim> >
{
    
  typedef blitz::Array<blitz::TinyMatrix<InnerT,InnerDim1,InnerDim2>,OuterDim>
  DataT;
  typedef InnerT BasicT;

  static std::vector<hsize_t> h5Dims(DataT const &array)
        {
          std::vector<hsize_t> dims(OuterDim + 1);
          for (int d = 0; d < OuterDim; ++d) dims[d] = array.extent(d);
          dims[OuterDim] = InnerDim1 * InnerDim2;
          return dims;
        }
    
  static hid_t h5Type()
        {
          return BlitzH5Traits<InnerT>::h5Type();
        }

  static void *data(DataT &array)
        {
          return reinterpret_cast<void*>(array.data());
        }

  static void const *data(DataT const &array)
        {
          return reinterpret_cast<void const*>(array.data());
        }
};

template<>
struct BlitzH5Traits<std::string>
{
    
  typedef std::string DataT;
  typedef std::string BasicT;

  static std::vector<hsize_t> h5Dims(DataT const &array);    
  static hid_t h5Type();
  static void *data(DataT &array);
  static void const *data(DataT const &array);

};

#endif
