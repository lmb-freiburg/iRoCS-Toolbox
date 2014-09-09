#include "lmbunit.hh"

#include <typeinfo>

#if defined HAVE_CONFIG_H && !defined PACKAGE
#include <config.hh>
#endif

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

#include <libBlitzHdf5/BlitzHdf5Light.hh>

template<typename DataT>
struct test_traits
{
  static hid_t h5Type;
  static std::string name;
};

template<>
hid_t test_traits<unsigned char>::h5Type = H5T_NATIVE_UCHAR;
template<>
std::string test_traits<unsigned char>::name = "uchar";

template<>
hid_t test_traits<char>::h5Type = H5T_NATIVE_CHAR;
template<>
std::string test_traits<char>::name = "char";

template<>
hid_t test_traits<unsigned short>::h5Type = H5T_NATIVE_USHORT;
template<>
std::string test_traits<unsigned short>::name = "ushort";

template<>
hid_t test_traits<short>::h5Type = H5T_NATIVE_SHORT;
template<>
std::string test_traits<short>::name = "short";

template<>
hid_t test_traits<unsigned int>::h5Type = H5T_NATIVE_UINT;
template<>
std::string test_traits<unsigned int>::name = "uint";

template<>
hid_t test_traits<int>::h5Type = H5T_NATIVE_INT;
template<>
std::string test_traits<int>::name = "int";

template<>
hid_t test_traits<unsigned long>::h5Type = H5T_NATIVE_ULONG;
template<>
std::string test_traits<unsigned long>::name = "ulong";

template<>
hid_t test_traits<long>::h5Type = H5T_NATIVE_LONG;
template<>
std::string test_traits<long>::name = "long";

template<>
hid_t test_traits<unsigned long long>::h5Type = H5T_NATIVE_ULLONG;
template<>
std::string test_traits<unsigned long long>::name = "ullong";

template<>
hid_t test_traits<long long>::h5Type = H5T_NATIVE_LLONG;
template<>
std::string test_traits<long long>::name = "llong";

template<>
hid_t test_traits<float>::h5Type = H5T_NATIVE_FLOAT;
template<>
std::string test_traits<float>::name = "float";

template<>
hid_t test_traits<double>::h5Type = H5T_NATIVE_DOUBLE;
template<>
std::string test_traits<double>::name = "double";

template<>
hid_t test_traits<long double>::h5Type = H5T_NATIVE_LDOUBLE;
template<>
std::string test_traits<long double>::name = "ldouble";

template<typename DatasetT, typename ArrayT>
static void testWriteDatasetSimple2D()
{
  std::string datasetName = std::string("/testWriteDatasetSimple2d/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  blitz::Array<DatasetT,2> data(3, 4);
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          data(y, x) = static_cast<DatasetT>(
              128.0 * static_cast<double>(std::rand()) /
              static_cast<double>(RAND_MAX));
  
  try
  {
    BlitzH5File outFile("testWriteDatasetSimple.h5", BlitzH5File::Replace);
    outFile.writeDataset(data, datasetName);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught BlitzH5Error during write: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught std::exception during write: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error during write");
  }

  blitz::Array<ArrayT,2> loaded;
  try
  {
    BlitzH5File inFile("testWriteDatasetSimple.h5");  
    inFile.readDataset(loaded, datasetName);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught BlitzH5Error during read: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught std::exception during read: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error during read");
  }

  int nErrors = 0;
  LMBUNIT_DEBUG_STREAM
      << "data.shape() = " << data.shape() << " == "
      << loaded.shape() << " = loaded.shape()" << std::endl;
  if (blitz::any(data.shape() != loaded.shape()))
  {
    LMBUNIT_WRITE_FAILURE("Data shape mismatch after loading... bailing out");
    return;
  }
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          if (static_cast<ArrayT>(data(y, x)) != loaded(y, x)) nErrors++;
  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");
}

template<typename DatasetT, typename ArrayT>
static void testWriteDatasetSimple2DVec()
{
  std::string datasetName = std::string("/testWriteDatasetSimple2dVec/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  blitz::Array<blitz::TinyVector<DatasetT,3>,2> data(3, 4);
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          for (int d = 0; d < 3; ++d)
              data(y, x)(d) = static_cast<DatasetT>(
                  128.0 * static_cast<double>(std::rand()) /
                  static_cast<double>(RAND_MAX));

  try
  {
    BlitzH5File outFile("testWriteDatasetSimple.h5", BlitzH5File::Replace);
    outFile.writeDataset(data, datasetName);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught BlitzH5Error during write: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught std::exception during write: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error during write");
  }

  blitz::Array<blitz::TinyVector<ArrayT,3>,2> loaded;
  try
  {
    BlitzH5File inFile("testWriteDatasetSimple.h5");  
    inFile.readDataset(loaded, datasetName);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught BlitzH5Error during read: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught std::exception during read: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error during read");
  }
  
  int nErrors = 0;
  LMBUNIT_DEBUG_STREAM
      << "data.shape() = " << data.shape() << " == "
      << loaded.shape() << " = loaded.shape()" << std::endl;
  if (blitz::any(data.shape() != loaded.shape()))
  {
    LMBUNIT_WRITE_FAILURE("Data shape mismatch after loading... bailing out");
    return;
  }
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          if (blitz::all(blitz::TinyVector<ArrayT,3>(data(y, x)) !=
                         loaded(y, x))) nErrors++;
  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");
}

template<typename DatasetT, typename ArrayT>
static void testWriteDatasetSimple3D()
{
  std::string datasetName = std::string("/testWriteDatasetSimple3d/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  blitz::Array<DatasetT,3> data(2, 3, 4);
  for (int z = 0; z < data.extent(0); ++z)
      for (int y = 0; y < data.extent(1); ++y)
          for (int x = 0; x < data.extent(2); ++x)
              data(z, y, x) = static_cast<DatasetT>(
                  128.0 * static_cast<double>(std::rand()) /
                  static_cast<double>(RAND_MAX));

  try
  {
    BlitzH5File outFile("testWriteDatasetSimple.h5", BlitzH5File::Replace);
    outFile.writeDataset(data, datasetName);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught BlitzH5Error during write: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught std::exception during write: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error during write");
  }

  blitz::Array<ArrayT,3> loaded;
  try
  {
    BlitzH5File inFile("testWriteDatasetSimple.h5");  
    inFile.readDataset(loaded, datasetName);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught BlitzH5Error during read: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught std::exception during read: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error during read");
  }

  int nErrors = 0;
  LMBUNIT_DEBUG_STREAM
      << "data.shape() = " << data.shape() << " == "
      << loaded.shape() << " = loaded.shape()" << std::endl;
  if (blitz::any(data.shape() != loaded.shape()))
  {
    LMBUNIT_WRITE_FAILURE("Data shape mismatch after loading... bailing out");
    return;
  }
  for (int z = 0; z < data.extent(0); ++z)
      for (int y = 0; y < data.extent(1); ++y)
          for (int x = 0; x < data.extent(2); ++x)
              if (static_cast<ArrayT>(data(z, y, x)) != loaded(z, y, x))
                  nErrors++;
  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");
}
   
template<typename DatasetT, typename ArrayT>
static void testWriteDatasetSimple3DVec()
{
  std::string datasetName = std::string("/testWriteDatasetSimple3dVec/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  blitz::Array<blitz::TinyVector<DatasetT,3>,3> data(2, 3, 4);
  for (int z = 0; z < data.extent(0); ++z)
      for (int y = 0; y < data.extent(1); ++y)
          for (int x = 0; x < data.extent(2); ++x)
              for (int d = 0; d < 3; ++d)
                  data(z, y, x)(d) = static_cast<DatasetT>(
                      128.0 * static_cast<double>(std::rand()) /
                      static_cast<double>(RAND_MAX));

  try
  {
    BlitzH5File outFile("testWriteDatasetSimple.h5", BlitzH5File::Replace);
    outFile.writeDataset(data, datasetName);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught BlitzH5Error during write: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught std::exception during write: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error during write");
  }

  blitz::Array<blitz::TinyVector<ArrayT,3>,3> loaded;
  try
  {
    BlitzH5File inFile("testWriteDatasetSimple.h5");  
    inFile.readDataset(loaded, datasetName);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught BlitzH5Error during read: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught std::exception during read: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error during read");
  }
  
  int nErrors = 0;
  LMBUNIT_DEBUG_STREAM
      << "data.shape() = " << data.shape() << " == "
      << loaded.shape() << " = loaded.shape()" << std::endl;
  if (blitz::any(data.shape() != loaded.shape()))
  {
    LMBUNIT_WRITE_FAILURE("Data shape mismatch after loading... bailing out");
    return;
  }
  for (int z = 0; z < data.extent(0); ++z)
      for (int y = 0; y < data.extent(1); ++y)
          for (int x = 0; x < data.extent(2); ++x)
              if (blitz::all(blitz::TinyVector<ArrayT,3>(data(z, y, x)) !=
                             loaded(z, y, x))) nErrors++;
  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");
}

static void testWriteStringDataset()
{
  std::string datasetName = std::string("/testWriteStringDataset/data");
  std::string data = "My test string";

  try
  {
    BlitzH5File outFile("testWriteDatasetSimple.h5", BlitzH5File::Replace);
    outFile.writeDataset(data, datasetName);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught BlitzH5Error during write: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught std::exception during write: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error during write");
  }

  std::string loaded;

  try
  {
    BlitzH5File inFile("testWriteDatasetSimple.h5");  
    inFile.readDataset(loaded, datasetName);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught BlitzH5Error during read: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught std::exception during read: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error during read");
  }

  LMBUNIT_ASSERT_EQUAL(data, loaded);
}

template<typename DatasetT, typename ArrayT>
static void testOverwriteDatasetEqual()
{
  std::string datasetName = "/testWriteDatasetOverwrite/data";

  blitz::Array<blitz::TinyVector<DatasetT,3>,3> data(2, 3, 4);
  for (int z = 0; z < data.extent(0); ++z)
      for (int y = 0; y < data.extent(1); ++y)
          for (int x = 0; x < data.extent(2); ++x)
              for (int d = 0; d < 3; ++d)
                  data(z, y, x)(d) = static_cast<DatasetT>(
                      128.0 * static_cast<double>(std::rand()) /
                      static_cast<double>(RAND_MAX));

  try
  {
    BlitzH5File outFile("testWriteDatasetSimple.h5", BlitzH5File::Replace);
    outFile.writeDataset(data, datasetName);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught BlitzH5Error during write: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught std::exception during write: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error during write");
  }
  
  blitz::Array<blitz::TinyVector<ArrayT,3>,3> newData(2, 3, 4);
  for (int z = 0; z < data.extent(0); ++z)
      for (int y = 0; y < data.extent(1); ++y)
          for (int x = 0; x < data.extent(2); ++x)
              for (int d = 0; d < 3; ++d)
                  data(z, y, x)(d) = static_cast<DatasetT>(
                      128.0 * static_cast<double>(std::rand()) /
                      static_cast<double>(RAND_MAX));  

  try
  {
    BlitzH5File outFile("testWriteDatasetSimple.h5", BlitzH5File::Write);
    outFile.writeDataset(newData, datasetName);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught BlitzH5Error during write: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught std::exception during write: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error during write");
  }
  
  blitz::Array<blitz::TinyVector<ArrayT,3>,3> loaded;
  try
  {
    BlitzH5File inFile("testWriteDatasetSimple.h5");  
    inFile.readDataset(loaded, datasetName);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught BlitzH5Error during read: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught std::exception during read: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error during read");
  }
  
  int nErrors = 0;
  LMBUNIT_DEBUG_STREAM
      << "data.shape() = " << data.shape() << " == "
      << loaded.shape() << " = loaded.shape()" << std::endl;
  if (blitz::any(data.shape() != loaded.shape()))
  {
    LMBUNIT_WRITE_FAILURE("Data shape mismatch after loading... bailing out");
    return;
  }
  for (int z = 0; z < data.extent(0); ++z)
      for (int y = 0; y < data.extent(1); ++y)
          for (int x = 0; x < data.extent(2); ++x)
              if (blitz::all(blitz::TinyVector<ArrayT,3>(newData(z, y, x)) !=
                             loaded(z, y, x))) nErrors++;
  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");  
}

template<typename DatasetT, typename ArrayT>
static void testOverwriteDatasetDifferentShape()
{
  std::string datasetName = "/testWriteDatasetOverwrite/data";

  blitz::Array<blitz::TinyVector<DatasetT,3>,3> data(2, 3, 4);
  for (int z = 0; z < data.extent(0); ++z)
      for (int y = 0; y < data.extent(1); ++y)
          for (int x = 0; x < data.extent(2); ++x)
              for (int d = 0; d < 3; ++d)
                  data(z, y, x)(d) = static_cast<DatasetT>(
                      128.0 * static_cast<double>(std::rand()) /
                      static_cast<double>(RAND_MAX));

  try
  {
    BlitzH5File outFile("testWriteDatasetSimple.h5", BlitzH5File::Replace);
    outFile.writeDataset(data, datasetName);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught BlitzH5Error during write: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught std::exception during write: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error during write");
  }
  
  blitz::Array<blitz::TinyVector<ArrayT,3>,3> newData(5, 4, 3);
  for (int z = 0; z < data.extent(0); ++z)
      for (int y = 0; y < data.extent(1); ++y)
          for (int x = 0; x < data.extent(2); ++x)
              for (int d = 0; d < 3; ++d)
                  data(z, y, x)(d) = static_cast<DatasetT>(
                      128.0 * static_cast<double>(std::rand()) /
                      static_cast<double>(RAND_MAX));  

  try
  {
    BlitzH5File outFile("testWriteDatasetSimple.h5", BlitzH5File::Write);
    outFile.writeDataset(newData, datasetName);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught BlitzH5Error during write: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught std::exception during write: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error during write");
  }
  
  blitz::Array<blitz::TinyVector<ArrayT,3>,3> loaded;
  try
  {
    BlitzH5File inFile("testWriteDatasetSimple.h5");  
    inFile.readDataset(loaded, datasetName);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught BlitzH5Error during read: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught std::exception during read: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error during read");
  }
  
  int nErrors = 0;
  LMBUNIT_DEBUG_STREAM
      << "data.shape() = " << data.shape() << " == "
      << loaded.shape() << " = loaded.shape()" << std::endl;
  if (blitz::any(newData.shape() != loaded.shape()))
  {
    LMBUNIT_WRITE_FAILURE("Data shape mismatch after loading... bailing out");
    return;
  }
  for (int z = 0; z < newData.extent(0); ++z)
      for (int y = 0; y < newData.extent(1); ++y)
          for (int x = 0; x < newData.extent(2); ++x)
              if (blitz::all(blitz::TinyVector<ArrayT,3>(newData(z, y, x)) !=
                             loaded(z, y, x))) nErrors++;
  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");  
}

int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST((testWriteDatasetSimple2D<float,float>()));
  LMBUNIT_RUN_TEST((testWriteDatasetSimple2D<unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST((testWriteDatasetSimple2D<double,unsigned int>()));
  LMBUNIT_RUN_TEST((testWriteDatasetSimple2D<unsigned int,double>()));

  LMBUNIT_RUN_TEST((testWriteDatasetSimple2DVec<float,float>()));
  LMBUNIT_RUN_TEST(
      (testWriteDatasetSimple2DVec<unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST((testWriteDatasetSimple2DVec<double,unsigned int>()));
  LMBUNIT_RUN_TEST((testWriteDatasetSimple2DVec<unsigned int,double>()));

  LMBUNIT_RUN_TEST((testWriteDatasetSimple3D<float,float>()));
  LMBUNIT_RUN_TEST((testWriteDatasetSimple3D<unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST((testWriteDatasetSimple3D<double,unsigned int>()));
  LMBUNIT_RUN_TEST((testWriteDatasetSimple3D<unsigned int,double>()));

  LMBUNIT_RUN_TEST((testWriteDatasetSimple3DVec<float,float>()));
  LMBUNIT_RUN_TEST(
      (testWriteDatasetSimple3DVec<unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST((testWriteDatasetSimple3DVec<double,unsigned int>()));
  LMBUNIT_RUN_TEST((testWriteDatasetSimple3DVec<unsigned int,double>()));

  LMBUNIT_RUN_TEST(testWriteStringDataset());

  LMBUNIT_RUN_TEST((testOverwriteDatasetEqual<float,float>()));
  LMBUNIT_RUN_TEST((testOverwriteDatasetEqual<float,char>()));
  LMBUNIT_RUN_TEST((testOverwriteDatasetDifferentShape<float,float>()));
  LMBUNIT_RUN_TEST((testOverwriteDatasetDifferentShape<float,char>()));

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
