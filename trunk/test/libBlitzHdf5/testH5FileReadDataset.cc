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

template<typename DatasetT, typename DataT>
static void testReadScalarDataset()
{
  std::string datasetName = std::string("/testReadDatasetSimpleScalar/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<DataT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");

  DatasetT data = 42;

  hid_t dataspaceId = H5Screate(H5S_SCALAR);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 H5P_DEFAULT, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, &data);
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  DataT loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
  LMBUNIT_ASSERT_EQUAL(static_cast<DataT>(data), loaded);
}

template<typename DatasetT, typename DataT>
static void testRead0DDataset()
{
  std::string datasetName = std::string("/testReadDatasetSimple0D/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<DataT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");

  DatasetT data = 42;

  hsize_t dims[] = { 1 };
  hid_t dataspaceId = H5Screate_simple(1, dims, NULL);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 H5P_DEFAULT, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, &data);
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  DataT loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
  LMBUNIT_ASSERT_EQUAL(static_cast<DataT>(data), loaded);
}

template<typename DatasetT, typename DataT, int Dim>
static void testReadTinyVectorDataset()
{
  std::string datasetName = std::string(
      "/testReadDatasetSimpleTinyVector/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<DataT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");

  blitz::TinyVector<DatasetT,Dim> data;
  for (int d = 0; d < Dim; ++d)
      data(d) = static_cast<DatasetT>(
          100 * static_cast<double>(std::rand()) / RAND_MAX);

  hsize_t dims[] = { Dim };
  hid_t dataspaceId = H5Screate_simple(1, dims, NULL);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 H5P_DEFAULT, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::TinyVector<DataT,Dim> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
  int nErrors = 0;
  for (int d = 0; d < Dim; ++d)
      if (static_cast<DataT>(data(d)) != loaded(d)) nErrors++;

  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");
}

template<typename DatasetT, typename DataT, int NRows, int NCols>
static void testReadVectorIntoTinyMatrixDataset()
{
  std::string datasetName = std::string(
      "/testReadDatasetSimpleTinyMatrix/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<DataT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");

  blitz::TinyVector<DatasetT,NRows*NCols> data;
  for (int d = 0; d < NRows * NCols; ++d)
      data(d) = static_cast<DatasetT>(
          100 * static_cast<double>(std::rand()) / RAND_MAX);

  hsize_t dims[] = { NRows * NCols };
  hid_t dataspaceId = H5Screate_simple(1, dims, NULL);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 H5P_DEFAULT, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::TinyMatrix<DataT,NRows,NCols> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
  int nErrors = 0;
  for (int r = 0; r < NRows; ++r)
      for (int c = 0; c < NCols; ++c)
          if (static_cast<DataT>(data(r * NCols + c)) != loaded(r, c))
              nErrors++;

  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");
}

template<typename DatasetT, typename DataT, int NRows, int NCols>
static void testReadTinyMatrixDataset()
{
  std::string datasetName = std::string(
      "/testReadDatasetSimpleTinyMatrix/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<DataT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");

  blitz::TinyMatrix<DatasetT,NRows,NCols> data;
  for (int r = 0; r < NRows; ++r)
      for (int c = 0; c < NCols; ++c)
          data(r, c) = static_cast<DatasetT>(
          100 * static_cast<double>(std::rand()) / RAND_MAX);

  hsize_t dims[] = { NRows, NCols };
  hid_t dataspaceId = H5Screate_simple(2, dims, NULL);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 H5P_DEFAULT, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::TinyMatrix<DataT,NRows,NCols> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
  int nErrors = 0;
  for (int r = 0; r < NRows; ++r)
      for (int c = 0; c < NCols; ++c)
          if (static_cast<DataT>(data(r, c)) != loaded(r, c)) nErrors++;

  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");
}

template<typename DatasetT, typename DataT, int Dim>
static void testReadStdVectorDataset()
{
  std::string datasetName = std::string(
      "/testReadDatasetSimpleTinyVector/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<DataT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");

  blitz::TinyVector<DatasetT,Dim> data;
  for (int d = 0; d < Dim; ++d)
      data(d) = static_cast<DatasetT>(
          100 * static_cast<double>(std::rand()) / RAND_MAX);

  hsize_t dims[] = { Dim };
  hid_t dataspaceId = H5Screate_simple(1, dims, NULL);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 H5P_DEFAULT, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  std::vector<DataT> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
  LMBUNIT_ASSERT_EQUAL(Dim, loaded.size());

  int nErrors = 0;
  for (int d = 0; d < Dim; ++d)
      if (static_cast<DataT>(data(d)) != loaded[d]) nErrors++;

  LMBUNIT_DEBUG_STREAM << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM << "loaded = ";
  for (int d = 0; d < Dim; ++d) LMBUNIT_DEBUG_STREAM << loaded[d] << " ";
  LMBUNIT_DEBUG_STREAM << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");
}

static void testReadStringDatasetS1Fixed()
{
  std::string datasetName = std::string(
      "/testReadDatasetSimpleS1FixedString/data");

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");

  std::string data("Teststring");

  hid_t dataspaceId = H5Screate(H5S_SCALAR);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
          
  hid_t datatypeId = H5Tcopy(H5T_C_S1);
  if (datatypeId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create string type");
  herr_t err = H5Tset_size(datatypeId, data.size());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not set string length");
  
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);

  hid_t datasetId = H5Dcreate2(
      fileId, datasetName.c_str(), datatypeId, dataspaceId,
      linkCreationPropertiesId, H5P_DEFAULT, H5P_DEFAULT);
  H5Pclose(linkCreationPropertiesId);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  
  char *buf = new char[data.size() + 1];
  for (size_t i = 0; i < data.size(); ++i) buf[i] = data[i];
  buf[data.size()] = 0;
  err = H5Dwrite(datasetId, datatypeId, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
  H5Dclose(datasetId);
  H5Tclose(datatypeId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);
  delete[] buf;
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not weite string dataset");

  BlitzH5File inFile("testReadDatasetSimple.h5");

  std::string loaded;
  inFile.readDataset(loaded, datasetName);
  
  LMBUNIT_ASSERT_EQUAL(data, loaded);
}

static void testReadStringDatasetCharArray()
{
  std::string datasetName = std::string(
      "/testReadDatasetSimpleCharArrayString/data");

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");

  std::string data("Teststring");

  hsize_t dims[] = { data.size() };
  hid_t dataspaceId = H5Screate_simple(1, dims, NULL);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
          
  hid_t datatypeId = H5Tcopy(H5T_C_S1);
  if (datatypeId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create string type");
  herr_t err = H5Tset_size(datatypeId, 1);
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not set string length");
  
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);

  hid_t datasetId = H5Dcreate2(
      fileId, datasetName.c_str(), datatypeId, dataspaceId,
      linkCreationPropertiesId, H5P_DEFAULT, H5P_DEFAULT);
  H5Pclose(linkCreationPropertiesId);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  
  char *buf = new char[data.size()];
  for (size_t i = 0; i < data.size(); ++i) buf[i] = data[i];
  err = H5Dwrite(datasetId, datatypeId, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
  H5Dclose(datasetId);
  H5Tclose(datatypeId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);
  delete[] buf;
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not weite string dataset");

   BlitzH5File inFile("testReadDatasetSimple.h5");

  std::string loaded;
  inFile.readDataset(loaded, datasetName);
  
  LMBUNIT_ASSERT_EQUAL(data, loaded);
}

template<typename DatasetT, typename ArrayT>
static void testReadDatasetSimple1D()
{
  std::string datasetName = std::string("/testReadDatasetSimple1d/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<DatasetT,1> data(1000);
  for (int x = 0; x < data.extent(0); ++x)
      data(x) = static_cast<DatasetT>(
          128.0 * static_cast<double>(std::rand()) /
          static_cast<double>(RAND_MAX));
  hsize_t currentSize = data.extent(0);
  hid_t dataspaceId = H5Screate_simple(1, &currentSize, NULL);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 H5P_DEFAULT, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::Array<ArrayT,1> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
  int nErrors = 0;
  LMBUNIT_DEBUG_STREAM
      << "data.shape() = " << data.shape() << " == "
      << loaded.shape() << " = loaded.shape()" << std::endl;
  if (blitz::any(data.shape() != loaded.shape()))
  {
    LMBUNIT_WRITE_FAILURE("Data shape mismatch after loading... bailing out");
    return;
  }
  for (int x = 0; x < data.extent(0); ++x)
      if (static_cast<ArrayT>(data(x)) != loaded(x)) nErrors++;
  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");
}

template<typename DatasetT, typename ArrayT>
static void testReadDatasetSimple1DChunked()
{
  std::string datasetName =
      std::string("/testReadDatasetSimple1dChunked/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<DatasetT,1> data(10);
  for (int x = 0; x < data.extent(0); ++x)
      data(x) = static_cast<DatasetT>(
          128.0 * static_cast<double>(std::rand()) /
          static_cast<double>(RAND_MAX));
  hsize_t currentSize = data.extent(0);
  hsize_t maxSize = H5S_UNLIMITED;
  hid_t dataspaceId = H5Screate_simple(1, &currentSize, &maxSize);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t datasetCreationPropertiesId = H5Pcreate(H5P_DATASET_CREATE);
  hsize_t chunkShape = 5;
  H5Pset_chunk(datasetCreationPropertiesId, 1, &chunkShape);
  H5Pset_deflate(datasetCreationPropertiesId, 1);
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 datasetCreationPropertiesId, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::Array<ArrayT,1> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
  int nErrors = 0;
  LMBUNIT_DEBUG_STREAM
      << "data.shape() = " << data.shape() << " == "
      << loaded.shape() << " = loaded.shape()" << std::endl;
  if (blitz::any(data.shape() != loaded.shape()))
  {
    LMBUNIT_WRITE_FAILURE("Data shape mismatch after loading... bailing out");
    return;
  }
  for (int x = 0; x < data.extent(0); ++x)
      if (static_cast<ArrayT>(data(x)) != loaded(x)) nErrors++;
  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");
}

template<typename DatasetT, typename ArrayT>
static void testReadDatasetSimple1DChunkedSuboptimalChunkSize()
{
  std::string datasetName =
      std::string("/testReadDatasetSimple1dChunked2/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<DatasetT,1> data(10);
  for (int x = 0; x < data.extent(0); ++x)
      data(x) = static_cast<DatasetT>(
          128.0 * static_cast<double>(std::rand()) /
          static_cast<double>(RAND_MAX));
  hsize_t currentSize = data.extent(0);
  hsize_t maxSize = H5S_UNLIMITED;
  hid_t dataspaceId = H5Screate_simple(1, &currentSize, &maxSize);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t datasetCreationPropertiesId = H5Pcreate(H5P_DATASET_CREATE);
  hsize_t chunkShape = 6;
  H5Pset_chunk(datasetCreationPropertiesId, 1, &chunkShape);
  H5Pset_deflate(datasetCreationPropertiesId, 1);
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 datasetCreationPropertiesId, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::Array<ArrayT,1> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
  int nErrors = 0;
  LMBUNIT_DEBUG_STREAM
      << "data.shape() = " << data.shape() << " == "
      << loaded.shape() << " = loaded.shape()" << std::endl;
  if (blitz::any(data.shape() != loaded.shape()))
  {
    LMBUNIT_WRITE_FAILURE("Data shape mismatch after loading... bailing out");
    return;
  }
  for (int x = 0; x < data.extent(0); ++x)
      if (static_cast<ArrayT>(data(x)) != loaded(x)) nErrors++;
  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");
}

template<typename DatasetT, typename ArrayT>
static void testReadDatasetSimple2D()
{
  std::string datasetName = std::string("/testReadDatasetSimple2d/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<DatasetT,2> data(3, 4);
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          data(y, x) = static_cast<DatasetT>(
              128.0 * static_cast<double>(std::rand()) /
              static_cast<double>(RAND_MAX));
  hsize_t currentSize[2];
  for (int d = 0; d < 2; ++d)
      currentSize[d] = data.extent(d);
  hid_t dataspaceId = H5Screate_simple(2, currentSize, NULL);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 H5P_DEFAULT, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::Array<ArrayT,2> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
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
static void testReadDatasetSimple2DChunkedContiguous()
{
  std::string datasetName = std::string(
      "/testReadDatasetSimple2dChunked/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<DatasetT,2> data(4, 3);
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          data(y, x) = static_cast<DatasetT>(
              128.0 * static_cast<double>(std::rand()) /
              static_cast<double>(RAND_MAX));
  hsize_t currentSize[2];
  hsize_t maxSize[2];
  for (int d = 0; d < 2; ++d)
  {
    currentSize[d] = data.extent(d);
    maxSize[d] = H5S_UNLIMITED;
  }
  hid_t dataspaceId = H5Screate_simple(2, currentSize, maxSize);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t datasetCreationPropertiesId = H5Pcreate(H5P_DATASET_CREATE);
  hsize_t chunkShape[2];
  chunkShape[0] = data.extent(0) / 2;
  chunkShape[1] = data.extent(1);
  H5Pset_chunk(datasetCreationPropertiesId, 2, chunkShape);
  H5Pset_deflate(datasetCreationPropertiesId, 1);
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 datasetCreationPropertiesId, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::Array<ArrayT,2> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
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
static void testReadDatasetSimple2DChunkedNonContiguous()
{
  std::string datasetName = std::string(
      "/testReadDatasetSimple2dChunked2/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<DatasetT,2> data(3, 4);
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          data(y, x) = static_cast<DatasetT>(
              128.0 * static_cast<double>(std::rand()) /
              static_cast<double>(RAND_MAX));
  hsize_t currentSize[2];
  hsize_t maxSize[2];
  for (int d = 0; d < 2; ++d)
  {
    currentSize[d] = data.extent(d);
    maxSize[d] = H5S_UNLIMITED;
  }
  hid_t dataspaceId = H5Screate_simple(2, currentSize, maxSize);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t datasetCreationPropertiesId = H5Pcreate(H5P_DATASET_CREATE);
  hsize_t chunkShape[2];
  chunkShape[0] = data.extent(0);
  chunkShape[1] = data.extent(1) / 2;
  H5Pset_chunk(datasetCreationPropertiesId, 2, chunkShape);
  H5Pset_deflate(datasetCreationPropertiesId, 1);
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 datasetCreationPropertiesId, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::Array<ArrayT,2> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
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
static void testReadDatasetSimple2DChunkedSuboptimalChunkSize()
{
  std::string datasetName = std::string(
      "/testReadDatasetSimple2dChunked3/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<DatasetT,2> data(3, 4);
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          data(y, x) = static_cast<DatasetT>(
              128.0 * static_cast<double>(std::rand()) /
              static_cast<double>(RAND_MAX));
  hsize_t currentSize[2];
  hsize_t maxSize[2];
  for (int d = 0; d < 2; ++d)
  {
    currentSize[d] = data.extent(d);
    maxSize[d] = H5S_UNLIMITED;
  }
  hid_t dataspaceId = H5Screate_simple(2, currentSize, maxSize);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t datasetCreationPropertiesId = H5Pcreate(H5P_DATASET_CREATE);
  hsize_t chunkShape[2];
  chunkShape[0] = 2;
  chunkShape[1] = 2;
  H5Pset_chunk(datasetCreationPropertiesId, 2, chunkShape);
  H5Pset_deflate(datasetCreationPropertiesId, 1);
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 datasetCreationPropertiesId, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::Array<ArrayT,2> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
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
static void testReadDatasetSimple2DVec()
{
  std::string datasetName = std::string("/testReadDatasetSimple2dVec/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<blitz::TinyVector<DatasetT,3>,2> data(3, 4);
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          for (int d = 0; d < 3; ++d)
              data(y, x)(d) = static_cast<DatasetT>(
                  128.0 * static_cast<double>(std::rand()) /
                  static_cast<double>(RAND_MAX));
  hsize_t currentSize[3];
  for (int d = 0; d < 2; ++d)
      currentSize[d] = data.extent(d);
  currentSize[2] = 3;
  hid_t dataspaceId = H5Screate_simple(3, currentSize, NULL);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 H5P_DEFAULT, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::Array<blitz::TinyVector<ArrayT,3>,2> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
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
static void testReadDatasetSimple2DVecChunked()
{
  std::string datasetName = std::string(
      "/testReadDatasetSimple2dVecChunked/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<blitz::TinyVector<DatasetT,3>,2> data(3, 4);
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          for (int d = 0; d < 3; ++d)
              data(y, x)(d) = static_cast<DatasetT>(
                  128.0 * static_cast<double>(std::rand()) /
                  static_cast<double>(RAND_MAX));
  hsize_t currentSize[3];
  hsize_t maxSize[3];
  for (int d = 0; d < 2; ++d)
  {
    currentSize[d] = data.extent(d);
    maxSize[d] = H5S_UNLIMITED;
  }
  currentSize[2] = 3;
  maxSize[2] = H5S_UNLIMITED;
  hid_t dataspaceId = H5Screate_simple(3, currentSize, maxSize);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t datasetCreationPropertiesId = H5Pcreate(H5P_DATASET_CREATE);
  hsize_t chunkShape[3];
  chunkShape[0] = data.extent(0);
  chunkShape[1] = data.extent(1);
  chunkShape[2] = 3;
  H5Pset_chunk(datasetCreationPropertiesId, 3, chunkShape);
  H5Pset_deflate(datasetCreationPropertiesId, 1);
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 datasetCreationPropertiesId, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::Array<blitz::TinyVector<ArrayT,3>,2> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
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
static void testReadDatasetSimple3D()
{
  std::string datasetName = std::string("/testReadDatasetSimple3d/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<DatasetT,3> data(10, 10, 10);
  for (int z = 0; z < data.extent(0); ++z)
      for (int y = 0; y < data.extent(1); ++y)
          for (int x = 0; x < data.extent(2); ++x)
              data(z, y, x) = static_cast<DatasetT>(
                  128.0 * static_cast<double>(std::rand()) /
                  static_cast<double>(RAND_MAX));
  hsize_t currentSize[3];
  for (int d = 0; d < 3; ++d)
      currentSize[d] = data.extent(d);
  hid_t dataspaceId = H5Screate_simple(3, currentSize, NULL);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 H5P_DEFAULT, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::Array<ArrayT,3> loaded;
  inFile.readDataset(loaded, datasetName);
  
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
static void testReadDatasetSimple3DChunked()
{
  std::string datasetName = std::string(
      "/testReadDatasetSimple3dChunked/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<DatasetT,3> data(10, 10, 10);
  for (int z = 0; z < data.extent(0); ++z)
      for (int y = 0; y < data.extent(1); ++y)
          for (int x = 0; x < data.extent(2); ++x)
              data(z, y, x) = static_cast<DatasetT>(
                  128.0 * static_cast<double>(std::rand()) /
                  static_cast<double>(RAND_MAX));
  hsize_t currentSize[3];
  hsize_t maxSize[3];
  for (int d = 0; d < 3; ++d)
  {
    currentSize[d] = data.extent(d);
    maxSize[d] = H5S_UNLIMITED;
  }
  hid_t dataspaceId = H5Screate_simple(3, currentSize, maxSize);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t datasetCreationPropertiesId = H5Pcreate(H5P_DATASET_CREATE);
  hsize_t chunkShape[3];
  chunkShape[0] = 1;
  chunkShape[1] = data.extent(1);
  chunkShape[2] = data.extent(2);
  H5Pset_chunk(datasetCreationPropertiesId, 3, chunkShape);
  H5Pset_deflate(datasetCreationPropertiesId, 1);
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 datasetCreationPropertiesId, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::Array<ArrayT,3> loaded;
  inFile.readDataset(loaded, datasetName);
  
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
static void testReadDatasetSimple3DVec()
{
  std::string datasetName = std::string("/testReadDatasetSimple3dVec/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<blitz::TinyVector<DatasetT,3>,3> data(10, 10, 10);
  for (int z = 0; z < data.extent(0); ++z)
      for (int y = 0; y < data.extent(1); ++y)
          for (int x = 0; x < data.extent(2); ++x)
              for (int d = 0; d < 3; ++d)
                  data(z, y, x)(d) = static_cast<DatasetT>(
                      128.0 * static_cast<double>(std::rand()) /
                      static_cast<double>(RAND_MAX));
  hsize_t currentSize[4];
  for (int d = 0; d < 3; ++d)
      currentSize[d] = data.extent(d);
  currentSize[3] = 3;
  hid_t dataspaceId = H5Screate_simple(4, currentSize, NULL);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 H5P_DEFAULT, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::Array<blitz::TinyVector<ArrayT,3>,3> loaded;
  inFile.readDataset(loaded, datasetName);
  
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

template<typename DatasetT, typename ArrayT>
static void testReadDatasetSimple3DVecChunked()
{
  std::string datasetName = std::string(
      "/testReadDatasetSimple3dVecChunked/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<blitz::TinyVector<DatasetT,3>,3> data(10, 10, 10);
  for (int z = 0; z < data.extent(0); ++z)
      for (int y = 0; y < data.extent(1); ++y)
          for (int x = 0; x < data.extent(2); ++x)
              for (int d = 0; d < 3; ++d)
                  data(z, y, x)(d) = static_cast<DatasetT>(
                      128.0 * static_cast<double>(std::rand()) /
                      static_cast<double>(RAND_MAX));
  hsize_t currentSize[4];
  hsize_t maxSize[4];
  for (int d = 0; d < 3; ++d)
  {
    currentSize[d] = data.extent(d);
    maxSize[d] = H5S_UNLIMITED;
  }
  currentSize[3] = 3;
  maxSize[3] = H5S_UNLIMITED;
  hid_t dataspaceId = H5Screate_simple(4, currentSize, maxSize);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t datasetCreationPropertiesId = H5Pcreate(H5P_DATASET_CREATE);
  hsize_t chunkShape[4];
  chunkShape[0] = 1;
  chunkShape[1] = data.extent(1);
  chunkShape[2] = data.extent(2);
  chunkShape[3] = 3;
  H5Pset_chunk(datasetCreationPropertiesId, 4, chunkShape);
  H5Pset_deflate(datasetCreationPropertiesId, 1);
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 datasetCreationPropertiesId, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::Array<blitz::TinyVector<ArrayT,3>,3> loaded;
  inFile.readDataset(loaded, datasetName);
  
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

template<typename DatasetT, typename ArrayT>
static void testReadDatasetSimple2DInto3DContainer()
{
  std::string datasetName = std::string("/testReadDatasetSimple2d/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<DatasetT,2> data(3, 4);
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          data(y, x) = static_cast<DatasetT>(
              128.0 * static_cast<double>(std::rand()) /
              static_cast<double>(RAND_MAX));
  hsize_t currentSize[2];
  for (int d = 0; d < 2; ++d)
      currentSize[d] = data.extent(d);
  hid_t dataspaceId = H5Screate_simple(2, currentSize, NULL);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 H5P_DEFAULT, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::TinyVector<int,3> wantedShape(1, data.extent(0), data.extent(1));
  blitz::Array<ArrayT,3> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
  int nErrors = 0;
  LMBUNIT_DEBUG_STREAM
      << "wanted shape = " << wantedShape << " == "
      << loaded.shape() << " = loaded.shape()" << std::endl;
  if (blitz::any(wantedShape != loaded.shape()))
  {
    LMBUNIT_WRITE_FAILURE("Data shape mismatch after loading... bailing out");
    return;
  }
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          if (static_cast<ArrayT>(data(y, x)) != loaded(0, y, x)) nErrors++;
  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");
}

template<typename DatasetT, typename ArrayT>
static void testReadDatasetSimple2DChunkedContiguousInto3DContainer()
{
  std::string datasetName = std::string(
      "/testReadDatasetSimple2dChunked/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<DatasetT,2> data(4, 3);
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          data(y, x) = static_cast<DatasetT>(
              128.0 * static_cast<double>(std::rand()) /
              static_cast<double>(RAND_MAX));
  hsize_t currentSize[2];
  hsize_t maxSize[2];
  for (int d = 0; d < 2; ++d)
  {
    currentSize[d] = data.extent(d);
    maxSize[d] = H5S_UNLIMITED;
  }
  hid_t dataspaceId = H5Screate_simple(2, currentSize, maxSize);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t datasetCreationPropertiesId = H5Pcreate(H5P_DATASET_CREATE);
  hsize_t chunkShape[2];
  chunkShape[0] = data.extent(0) / 2;
  chunkShape[1] = data.extent(1);
  H5Pset_chunk(datasetCreationPropertiesId, 2, chunkShape);
  H5Pset_deflate(datasetCreationPropertiesId, 1);
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 datasetCreationPropertiesId, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::TinyVector<int,3> wantedShape(1, data.extent(0), data.extent(1));
  blitz::Array<ArrayT,3> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
  int nErrors = 0;
  LMBUNIT_DEBUG_STREAM
      << "wanted shape = " << wantedShape << " == "
      << loaded.shape() << " = loaded.shape()" << std::endl;
  if (blitz::any(wantedShape != loaded.shape()))
  {
    LMBUNIT_WRITE_FAILURE("Data shape mismatch after loading... bailing out");
    return;
  }
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          if (static_cast<ArrayT>(data(y, x)) != loaded(0, y, x)) nErrors++;
  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");
}

template<typename DatasetT, typename ArrayT>
static void testReadDatasetSimple2DChunkedNonContiguousInto3DContainer()
{
  std::string datasetName = std::string(
      "/testReadDatasetSimple2dChunked2/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<DatasetT,2> data(3, 4);
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          data(y, x) = static_cast<DatasetT>(
              128.0 * static_cast<double>(std::rand()) /
              static_cast<double>(RAND_MAX));
  hsize_t currentSize[2];
  hsize_t maxSize[2];
  for (int d = 0; d < 2; ++d)
  {
    currentSize[d] = data.extent(d);
    maxSize[d] = H5S_UNLIMITED;
  }
  hid_t dataspaceId = H5Screate_simple(2, currentSize, maxSize);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t datasetCreationPropertiesId = H5Pcreate(H5P_DATASET_CREATE);
  hsize_t chunkShape[2];
  chunkShape[0] = data.extent(0);
  chunkShape[1] = data.extent(1) / 2;
  H5Pset_chunk(datasetCreationPropertiesId, 2, chunkShape);
  H5Pset_deflate(datasetCreationPropertiesId, 1);
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 datasetCreationPropertiesId, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::TinyVector<int,3> wantedShape(1, data.extent(0), data.extent(1));
  blitz::Array<ArrayT,3> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
  int nErrors = 0;
  LMBUNIT_DEBUG_STREAM
      << "wanted shape = " << wantedShape << " == "
      << loaded.shape() << " = loaded.shape()" << std::endl;
  if (blitz::any(wantedShape != loaded.shape()))
  {
    LMBUNIT_WRITE_FAILURE("Data shape mismatch after loading... bailing out");
    return;
  }
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          if (static_cast<ArrayT>(data(y, x)) != loaded(0, y, x)) nErrors++;
  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");
}

template<typename DatasetT, typename ArrayT>
static void testReadDatasetSimple2DChunkedSuboptimalChunkSizeInto3DContainer()
{
  std::string datasetName = std::string(
      "/testReadDatasetSimple2dChunked3/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<DatasetT,2> data(3, 4);
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          data(y, x) = static_cast<DatasetT>(
              128.0 * static_cast<double>(std::rand()) /
              static_cast<double>(RAND_MAX));
  hsize_t currentSize[2];
  hsize_t maxSize[2];
  for (int d = 0; d < 2; ++d)
  {
    currentSize[d] = data.extent(d);
    maxSize[d] = H5S_UNLIMITED;
  }
  hid_t dataspaceId = H5Screate_simple(2, currentSize, maxSize);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t datasetCreationPropertiesId = H5Pcreate(H5P_DATASET_CREATE);
  hsize_t chunkShape[2];
  chunkShape[0] = 2;
  chunkShape[1] = 2;
  H5Pset_chunk(datasetCreationPropertiesId, 2, chunkShape);
  H5Pset_deflate(datasetCreationPropertiesId, 1);
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 datasetCreationPropertiesId, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::TinyVector<int,3> wantedShape(1, data.extent(0), data.extent(1));
  blitz::Array<ArrayT,3> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
  int nErrors = 0;
  LMBUNIT_DEBUG_STREAM
      << "wanted shape = " << wantedShape << " == "
      << loaded.shape() << " = loaded.shape()" << std::endl;
  if (blitz::any(wantedShape != loaded.shape()))
  {
    LMBUNIT_WRITE_FAILURE("Data shape mismatch after loading... bailing out");
    return;
  }
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          if (static_cast<ArrayT>(data(y, x)) != loaded(0, y, x)) nErrors++;
  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");
}

template<typename DatasetT, typename ArrayT>
static void testReadDatasetSimple2DVecInto3DContainer()
{
  std::string datasetName = std::string("/testReadDatasetSimple2dVec/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<blitz::TinyVector<DatasetT,3>,2> data(3, 4);
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          for (int d = 0; d < 3; ++d)
              data(y, x)(d) = static_cast<DatasetT>(
                  128.0 * static_cast<double>(std::rand()) /
                  static_cast<double>(RAND_MAX));
  hsize_t currentSize[3];
  for (int d = 0; d < 2; ++d)
      currentSize[d] = data.extent(d);
  currentSize[2] = 3;
  hid_t dataspaceId = H5Screate_simple(3, currentSize, NULL);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 H5P_DEFAULT, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::TinyVector<int,3> wantedShape(1, data.extent(0), data.extent(1));
  blitz::Array<blitz::TinyVector<ArrayT,3>,3> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
  int nErrors = 0;
  LMBUNIT_DEBUG_STREAM
      << "wanted shape = " << wantedShape << " == "
      << loaded.shape() << " = loaded.shape()" << std::endl;
  if (blitz::any(wantedShape != loaded.shape()))
  {
    LMBUNIT_WRITE_FAILURE("Data shape mismatch after loading... bailing out");
    return;
  }
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          if (blitz::all(blitz::TinyVector<ArrayT,3>(data(y, x)) !=
                         loaded(0, y, x))) nErrors++;
  LMBUNIT_DEBUG_STREAM
      << "data = " << data << std::endl;
  LMBUNIT_DEBUG_STREAM
      << "loaded = " << loaded << std::endl;
  if (nErrors > 0)
      LMBUNIT_WRITE_FAILURE(
          "Read Array contains different values than written array");
}

template<typename DatasetT, typename ArrayT>
static void testReadDatasetSimple2DVecChunkedInto3DContainer()
{
  std::string datasetName = std::string(
      "/testReadDatasetSimple2dVecChunked/data_") +
      test_traits<DatasetT>::name + "_" + test_traits<ArrayT>::name;

  hid_t fileId = H5Fcreate(
      "testReadDatasetSimple.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
  blitz::Array<blitz::TinyVector<DatasetT,3>,2> data(3, 4);
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          for (int d = 0; d < 3; ++d)
              data(y, x)(d) = static_cast<DatasetT>(
                  128.0 * static_cast<double>(std::rand()) /
                  static_cast<double>(RAND_MAX));
  hsize_t currentSize[3];
  hsize_t maxSize[3];
  for (int d = 0; d < 2; ++d)
  {
    currentSize[d] = data.extent(d);
    maxSize[d] = H5S_UNLIMITED;
  }
  currentSize[2] = 3;
  maxSize[2] = H5S_UNLIMITED;
  hid_t dataspaceId = H5Screate_simple(3, currentSize, maxSize);
  if (dataspaceId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
  hid_t datasetCreationPropertiesId = H5Pcreate(H5P_DATASET_CREATE);
  hsize_t chunkShape[3];
  chunkShape[0] = data.extent(0);
  chunkShape[1] = data.extent(1);
  chunkShape[2] = 3;
  H5Pset_chunk(datasetCreationPropertiesId, 3, chunkShape);
  H5Pset_deflate(datasetCreationPropertiesId, 1);
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t datasetId =
      H5Dcreate2(fileId, datasetName.c_str(), test_traits<DatasetT>::h5Type,
                 dataspaceId, linkCreationPropertiesId,
                 datasetCreationPropertiesId, H5P_DEFAULT);
  if (datasetId < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not create dataset");
  herr_t err = H5Dwrite(datasetId, test_traits<DatasetT>::h5Type, H5S_ALL,
                        dataspaceId, H5P_DEFAULT, data.data());
  if (err < 0)
      LMBUNIT_WRITE_FAILURE("Internal error: Could not write dataset");
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId);

  BlitzH5File inFile("testReadDatasetSimple.h5");

  blitz::TinyVector<int,3> wantedShape(1, data.extent(0), data.extent(1));
  blitz::Array<blitz::TinyVector<ArrayT,3>,3> loaded;
  inFile.readDataset(loaded, datasetName.c_str());
  
  int nErrors = 0;
  LMBUNIT_DEBUG_STREAM
      << "wanted shape = " << wantedShape << " == "
      << loaded.shape() << " = loaded.shape()" << std::endl;
  if (blitz::any(wantedShape != loaded.shape()))
  {
    LMBUNIT_WRITE_FAILURE("Data shape mismatch after loading... bailing out");
    return;
  }
  for (int y = 0; y < data.extent(0); ++y)
      for (int x = 0; x < data.extent(1); ++x)
          if (blitz::all(blitz::TinyVector<ArrayT,3>(data(y, x)) !=
                         loaded(0, y, x))) nErrors++;
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

  LMBUNIT_RUN_TEST((testReadScalarDataset<unsigned char,unsigned char>()));
  LMBUNIT_RUN_TEST((testReadScalarDataset<char,char>()));
  LMBUNIT_RUN_TEST((testReadScalarDataset<unsigned short,unsigned short>()));
  LMBUNIT_RUN_TEST((testReadScalarDataset<short,short>()));
  LMBUNIT_RUN_TEST((testReadScalarDataset<unsigned int,unsigned int>()));
  LMBUNIT_RUN_TEST((testReadScalarDataset<int,int>()));  
  LMBUNIT_RUN_TEST((testReadScalarDataset<int,float>()));  
  LMBUNIT_RUN_TEST((testReadScalarDataset<int,double>()));  
  LMBUNIT_RUN_TEST((testReadScalarDataset<unsigned long,unsigned long>()));
  LMBUNIT_RUN_TEST((testReadScalarDataset<long,long>()));  
  LMBUNIT_RUN_TEST(
      (testReadScalarDataset<unsigned long long,unsigned long long>()));
  LMBUNIT_RUN_TEST((testReadScalarDataset<long long,long long>()));  
  LMBUNIT_RUN_TEST((testReadScalarDataset<float,float>()));
  LMBUNIT_RUN_TEST((testReadScalarDataset<float,double>()));
  LMBUNIT_RUN_TEST((testReadScalarDataset<double,double>()));  
  LMBUNIT_RUN_TEST((testReadScalarDataset<double,float>()));  
  LMBUNIT_RUN_TEST((testReadScalarDataset<double,int>()));  
  LMBUNIT_RUN_TEST((testReadScalarDataset<long double,long double>()));  

  LMBUNIT_RUN_TEST((testRead0DDataset<float,double>()));

  LMBUNIT_RUN_TEST((testReadTinyVectorDataset<float,float,3>()));
  LMBUNIT_RUN_TEST((testReadTinyVectorDataset<float,double,3>()));
  LMBUNIT_RUN_TEST((testReadTinyVectorDataset<double,double,3>()));
  LMBUNIT_RUN_TEST((testReadTinyVectorDataset<double,float,3>()));
  LMBUNIT_RUN_TEST((testReadVectorIntoTinyMatrixDataset<float,float,3,4>()));
  LMBUNIT_RUN_TEST((testReadVectorIntoTinyMatrixDataset<float,double,3,4>()));
  LMBUNIT_RUN_TEST((testReadVectorIntoTinyMatrixDataset<double,double,3,4>()));
  LMBUNIT_RUN_TEST((testReadVectorIntoTinyMatrixDataset<double,float,3,4>()));
  LMBUNIT_RUN_TEST((testReadTinyMatrixDataset<float,float,3,4>()));
  LMBUNIT_RUN_TEST((testReadTinyMatrixDataset<float,double,3,4>()));
  LMBUNIT_RUN_TEST((testReadTinyMatrixDataset<double,double,3,4>()));
  LMBUNIT_RUN_TEST((testReadTinyMatrixDataset<double,float,3,4>()));
  LMBUNIT_RUN_TEST((testReadStdVectorDataset<float,float,3>()));
  LMBUNIT_RUN_TEST((testReadStdVectorDataset<float,double,3>()));
  LMBUNIT_RUN_TEST((testReadStdVectorDataset<double,double,3>()));
  LMBUNIT_RUN_TEST((testReadStdVectorDataset<double,float,3>()));

  LMBUNIT_RUN_TEST(testReadStringDatasetS1Fixed());
  LMBUNIT_RUN_TEST(testReadStringDatasetCharArray());

  LMBUNIT_RUN_TEST((testReadDatasetSimple1D<float,float>()));
  LMBUNIT_RUN_TEST((testReadDatasetSimple1D<unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST((testReadDatasetSimple1D<double,unsigned int>()));
  LMBUNIT_RUN_TEST((testReadDatasetSimple1D<unsigned int,double>()));

  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple1DChunked<float,float>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple1DChunked<unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple1DChunked<double,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple1DChunked<unsigned int,double>()));

  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple1DChunkedSuboptimalChunkSize<float,float>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple1DChunkedSuboptimalChunkSize<
       unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple1DChunkedSuboptimalChunkSize<
       double,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple1DChunkedSuboptimalChunkSize<
       unsigned int,double>()));

  LMBUNIT_RUN_TEST((testReadDatasetSimple2D<float,float>()));
  LMBUNIT_RUN_TEST((testReadDatasetSimple2D<unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST((testReadDatasetSimple2D<double,unsigned int>()));
  LMBUNIT_RUN_TEST((testReadDatasetSimple2D<unsigned int,double>()));

  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedContiguous<float,float>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedContiguous<
       unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedContiguous<double,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedContiguous<unsigned int,double>()));

  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedNonContiguous<float,float>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedNonContiguous<
       unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedNonContiguous<double,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedNonContiguous<unsigned int,double>()));

  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedSuboptimalChunkSize<float,float>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedSuboptimalChunkSize<
       unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedSuboptimalChunkSize<
       double,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedSuboptimalChunkSize<
       unsigned int,double>()));

  LMBUNIT_RUN_TEST((testReadDatasetSimple2DVec<float,float>()));
  LMBUNIT_RUN_TEST((testReadDatasetSimple2DVec<unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST((testReadDatasetSimple2DVec<double,unsigned int>()));
  LMBUNIT_RUN_TEST((testReadDatasetSimple2DVec<unsigned int,double>()));

  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DVecChunked<float,float>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DVecChunked<unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DVecChunked<double,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DVecChunked<unsigned int,double>()));

  LMBUNIT_RUN_TEST((testReadDatasetSimple3D<float,float>()));
  LMBUNIT_RUN_TEST((testReadDatasetSimple3D<unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST((testReadDatasetSimple3D<double,unsigned int>()));
  LMBUNIT_RUN_TEST((testReadDatasetSimple3D<unsigned int,double>()));

  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple3DChunked<float,float>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple3DChunked<unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple3DChunked<double,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple3DChunked<unsigned int,double>()));

  LMBUNIT_RUN_TEST((testReadDatasetSimple3DVec<float,float>()));
  LMBUNIT_RUN_TEST((testReadDatasetSimple3DVec<unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST((testReadDatasetSimple3DVec<double,unsigned int>()));
  LMBUNIT_RUN_TEST((testReadDatasetSimple3DVec<unsigned int,double>()));

  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple3DVecChunked<float,float>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple3DVecChunked<unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple3DVecChunked<double,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple3DVecChunked<unsigned int,double>()));

  LMBUNIT_RUN_TEST((testReadDatasetSimple2DInto3DContainer<float,float>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DInto3DContainer<unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DInto3DContainer<double,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DInto3DContainer<unsigned int,double>()));

  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedContiguousInto3DContainer<float,float>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedContiguousInto3DContainer<
       unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedContiguousInto3DContainer<
       double,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedContiguousInto3DContainer<
       unsigned int,double>()));

  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedNonContiguousInto3DContainer<
       float,float>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedNonContiguousInto3DContainer<
       unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedNonContiguousInto3DContainer<
       double,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedNonContiguousInto3DContainer<
       unsigned int,double>()));

  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedSuboptimalChunkSizeInto3DContainer<
       float,float>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedSuboptimalChunkSizeInto3DContainer<
       unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedSuboptimalChunkSizeInto3DContainer<
       double,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DChunkedSuboptimalChunkSizeInto3DContainer<
       unsigned int,double>()));

  LMBUNIT_RUN_TEST((testReadDatasetSimple2DVecInto3DContainer<float,float>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DVecInto3DContainer<
       unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DVecInto3DContainer<double,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DVecInto3DContainer<unsigned int,double>()));

  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DVecChunkedInto3DContainer<float,float>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DVecChunkedInto3DContainer<
       unsigned short,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DVecChunkedInto3DContainer<
       double,unsigned int>()));
  LMBUNIT_RUN_TEST(
      (testReadDatasetSimple2DVecChunkedInto3DContainer<
       unsigned int,double>()));

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
