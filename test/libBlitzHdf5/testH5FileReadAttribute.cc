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

static void testReadAttributeStringInRootGroup()
{
  std::string attributeName = "globalTestString";
  const char *string_att = "my global test string";

  hid_t fileId = H5Fcreate(
      "testReadAttribute.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
    return;
  }
  hid_t datatypeId = H5Tcopy(H5T_C_S1);
  if (datatypeId < 0)
  {
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not get a H5T_C_S1 copy");
    return;
  }
  herr_t err = H5Tset_size(datatypeId, std::strlen(string_att));
  if (err < 0)
  {
    H5Tclose(datatypeId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE(
        "Internal error: Could not set variable string length");
    return;
  }
  hid_t dataspaceId = H5Screate(H5S_SCALAR);
  if (dataspaceId < 0)
  {
    H5Tclose(datatypeId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
    return;
  }
  hid_t groupId = H5Gopen2(fileId, "/", H5P_DEFAULT);
  if (groupId < 0)
  {
    H5Sclose(dataspaceId);
    H5Tclose(datatypeId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not open root group");
    return;
  }
  hid_t attributeId =
      H5Acreate2(groupId, attributeName.c_str(), datatypeId,
                 dataspaceId, H5P_DEFAULT, H5P_DEFAULT);
  if (attributeId < 0)
  {
    H5Gclose(groupId);
    H5Sclose(dataspaceId);
    H5Tclose(datatypeId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create attribute");
    return;
  }
  err = H5Awrite(attributeId, datatypeId, string_att);
  H5Aclose(attributeId);
  H5Gclose(groupId);
  H5Sclose(dataspaceId);
  H5Tclose(datatypeId);
  H5Fclose(fileId); 
  if (err < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not write attribute");
    return;
  }
  
  try
  {
    BlitzH5File inFile("testReadAttribute.h5");
    std::string value;
    inFile.readAttribute(value, attributeName);
    LMBUNIT_ASSERT_EQUAL(value, string_att);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
    return;
  }
}

static void testReadAttributeStringInGroup()
{
  std::string attributeName = "globalTestString";
  const char *string_att = "my global test string";

  hid_t fileId = H5Fcreate(
      "testReadAttribute.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
    return;
  }

  // Create group
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t groupId = H5Gcreate2(
      fileId, "/group/subgroup", linkCreationPropertiesId,
      H5P_DEFAULT, H5P_DEFAULT);
  H5Pclose(linkCreationPropertiesId);
  if (groupId < 0)
  {
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create group");
    return;
  }

  hid_t datatypeId = H5Tcopy(H5T_C_S1);
  if (datatypeId < 0)
  {
    H5Gclose(groupId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not get a H5T_C_S1 copy");
    return;
  }
  herr_t err = H5Tset_size(datatypeId, std::strlen(string_att));
  if (err < 0)
  {
    H5Tclose(datatypeId);
    H5Gclose(groupId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE(
        "Internal error: Could not set variable string length");
    return;
  }
  hid_t dataspaceId = H5Screate(H5S_SCALAR);
  if (dataspaceId < 0)
  {
    H5Tclose(datatypeId);
    H5Gclose(groupId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
    return;
  }
  hid_t attributeId =
      H5Acreate2(groupId, attributeName.c_str(), datatypeId,
                 dataspaceId, H5P_DEFAULT, H5P_DEFAULT);
  if (attributeId < 0)
  {
    H5Sclose(dataspaceId);
    H5Tclose(datatypeId);
    H5Gclose(groupId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create attribute");
    return;
  }
  err = H5Awrite(attributeId, datatypeId, string_att);
  H5Aclose(attributeId);
  H5Sclose(dataspaceId);
  H5Tclose(datatypeId);
  H5Gclose(groupId);
  H5Fclose(fileId); 
  if (err < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not write attribute");
    return;
  }
  
  try
  {
    BlitzH5File inFile("testReadAttribute.h5");
    std::string value;
    inFile.readAttribute(value, attributeName, "/group/subgroup");
    LMBUNIT_ASSERT_EQUAL(value, string_att);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
    return;
  }
}

static void testReadAttributeStringInDataset()
{
  std::string attributeName = "local test string";
  const char *string_att = "my local test string";

  // Create dataset
  try
  {
    BlitzH5File outFile("testReadAttribute.h5", BlitzH5File::Replace);
    blitz::Array<float,1> data(1);
    outFile.writeDataset(data, "/group/dataset");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
    return;
  }

  hid_t fileId = H5Fopen("testReadAttribute.h5", H5F_ACC_RDWR, H5P_DEFAULT);
  if (fileId < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not open test file");
    return;
  }
  hid_t datatypeId = H5Tcopy(H5T_C_S1);
  if (datatypeId < 0)
  {
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not get a H5T_C_S1 copy");
    return;
  }
  herr_t err = H5Tset_size(datatypeId, std::strlen(string_att));
  if (err < 0)
  {
    H5Tclose(datatypeId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE(
        "Internal error: Could not set variable string length");
    return;
  }
  hid_t dataspaceId = H5Screate(H5S_SCALAR);
  if (dataspaceId < 0)
  {
    H5Tclose(datatypeId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
    return;
  }
  hid_t datasetId = H5Dopen2(fileId, "/group/dataset", H5P_DEFAULT);
  if (datasetId < 0)
  {
    H5Sclose(dataspaceId);
    H5Tclose(datatypeId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not open dataset");
    return;
  }
  hid_t attributeId =
      H5Acreate2(datasetId, attributeName.c_str(), datatypeId,
                 dataspaceId, H5P_DEFAULT, H5P_DEFAULT);
  if (attributeId < 0)
  {
    H5Dclose(datasetId);
    H5Sclose(dataspaceId);
    H5Tclose(datatypeId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create attribute");
    return;
  }
  err = H5Awrite(attributeId, datatypeId, string_att);
  H5Aclose(attributeId);
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Tclose(datatypeId);
  H5Fclose(fileId); 
  if (err < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not write attribute");
    return;
  }
  
  try
  {
    BlitzH5File inFile("testReadAttribute.h5");
    std::string value;
    inFile.readAttribute(value, attributeName, "/group/dataset");
    LMBUNIT_ASSERT_EQUAL(value, string_att);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
    return;
  }
}

static void testReadAttributeStringAsCharArrayInGroup()
{
  std::string attributeName = "globalTestString";
  const char *string_att = "my global test string";

  hid_t fileId = H5Fcreate(
      "testReadAttribute.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
    return;
  }

  // Create group
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t groupId = H5Gcreate2(
      fileId, "/group/subgroup", linkCreationPropertiesId,
      H5P_DEFAULT, H5P_DEFAULT);
  H5Pclose(linkCreationPropertiesId);
  if (groupId < 0)
  {
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create group");
    return;
  }

  hid_t datatypeId = H5Tcopy(H5T_C_S1);
  if (datatypeId < 0)
  {
    H5Gclose(groupId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not get a H5T_C_S1 copy");
    return;
  }
  herr_t err = H5Tset_size(datatypeId, 1);
  if (err < 0)
  {
    H5Tclose(datatypeId);
    H5Gclose(groupId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE(
        "Internal error: Could not set variable string length");
    return;
  }
  hsize_t size = strlen(string_att);
  hid_t dataspaceId = H5Screate_simple(1, &size, NULL);
  if (dataspaceId < 0)
  {
    H5Tclose(datatypeId);
    H5Gclose(groupId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
    return;
  }
  hid_t attributeId =
      H5Acreate2(groupId, attributeName.c_str(), datatypeId,
                 dataspaceId, H5P_DEFAULT, H5P_DEFAULT);
  if (attributeId < 0)
  {
    H5Sclose(dataspaceId);
    H5Tclose(datatypeId);
    H5Gclose(groupId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create attribute");
    return;
  }
  err = H5Awrite(attributeId, datatypeId, string_att);
  H5Aclose(attributeId);
  H5Sclose(dataspaceId);
  H5Tclose(datatypeId);
  H5Gclose(groupId);
  H5Fclose(fileId); 
  if (err < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not write attribute");
    return;
  }
  
  try
  {
    BlitzH5File inFile("testReadAttribute.h5");
    std::string value;
    inFile.readAttribute(value, attributeName, "/group/subgroup");
    LMBUNIT_ASSERT_EQUAL(value, string_att);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
    return;
  }
}

static void testReadAttributeStringAsCharArrayInDataset()
{
  std::string attributeName = "local test string";
  const char *string_att = "my local test string";

  // Create dataset
  try
  {
    BlitzH5File outFile("testReadAttribute.h5", BlitzH5File::Replace);
    blitz::Array<float,1> data(1);
    outFile.writeDataset(data, "/group/dataset");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
    return;
  }

  hid_t fileId = H5Fopen("testReadAttribute.h5", H5F_ACC_RDWR, H5P_DEFAULT);
  if (fileId < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not open test file");
    return;
  }
  hid_t datatypeId = H5Tcopy(H5T_C_S1);
  if (datatypeId < 0)
  {
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not get a H5T_C_S1 copy");
    return;
  }
  herr_t err = H5Tset_size(datatypeId, 1);
  if (err < 0)
  {
    H5Tclose(datatypeId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE(
        "Internal error: Could not set variable string length");
    return;
  }
  hsize_t size = std::strlen(string_att);
  hid_t dataspaceId = H5Screate_simple(1, &size, NULL);
  if (dataspaceId < 0)
  {
    H5Tclose(datatypeId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
    return;
  }
  hid_t datasetId = H5Dopen2(fileId, "/group/dataset", H5P_DEFAULT);
  if (datasetId < 0)
  {
    H5Sclose(dataspaceId);
    H5Tclose(datatypeId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not open dataset");
    return;
  }
  hid_t attributeId =
      H5Acreate2(datasetId, attributeName.c_str(), datatypeId,
                 dataspaceId, H5P_DEFAULT, H5P_DEFAULT);
  if (attributeId < 0)
  {
    H5Dclose(datasetId);
    H5Sclose(dataspaceId);
    H5Tclose(datatypeId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create attribute");
    return;
  }
  err = H5Awrite(attributeId, datatypeId, string_att);
  H5Aclose(attributeId);
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Tclose(datatypeId);
  H5Fclose(fileId); 
  if (err < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not write attribute");
    return;
  }
  
  try
  {
    BlitzH5File inFile("testReadAttribute.h5");
    std::string value;
    inFile.readAttribute(value, attributeName, "/group/dataset");
    LMBUNIT_ASSERT_EQUAL(value, string_att);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
    return;
  }
}

template<typename WriteT, typename ReadT>
static void testReadAttributeTinyVectorInGroup()
{
  std::string attributeName = "TinyVectorAttribute";
  blitz::TinyVector<WriteT,3> attValue(1.0, 2.0, 3.0);

  hid_t fileId = H5Fcreate(
      "testReadAttribute.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
    return;
  }

  // Create group
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t groupId = H5Gcreate2(
      fileId, "/group/subgroup", linkCreationPropertiesId,
      H5P_DEFAULT, H5P_DEFAULT);
  H5Pclose(linkCreationPropertiesId);
  if (groupId < 0)
  {
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create group");
    return;
  }
  
  hsize_t size = 3;
  hid_t dataspaceId = H5Screate_simple(1, &size, NULL);
  if (dataspaceId < 0)
  {
    H5Gclose(groupId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
    return;
  }
  hid_t attributeId =
      H5Acreate2(groupId, attributeName.c_str(), test_traits<WriteT>::h5Type,
                 dataspaceId, H5P_DEFAULT, H5P_DEFAULT);
  if (attributeId < 0)
  {
    H5Sclose(dataspaceId);
    H5Gclose(groupId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create attribute");
    return;
  }
  herr_t err = H5Awrite(
      attributeId, test_traits<WriteT>::h5Type, attValue.data());
  H5Aclose(attributeId);
  H5Sclose(dataspaceId);
  H5Gclose(groupId);
  H5Fclose(fileId); 
  if (err < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not write attribute");
    return;
  }
  
  try
  {
    BlitzH5File inFile("testReadAttribute.h5");
    blitz::TinyVector<ReadT,3> value;
    inFile.readAttribute(value, attributeName, "/group/subgroup");
    LMBUNIT_ASSERT(blitz::all(value == blitz::TinyVector<ReadT,3>(attValue)));
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
    return;
  }  
}

template<typename WriteT, typename ReadT>
static void testReadAttributeTinyVectorInDataset()
{
  std::string attributeName = "TinyVectorAttribute";
  blitz::TinyVector<WriteT,3> attValue(1.0, 2.0, 3.0);

  // Create dataset
  try
  {
    BlitzH5File outFile("testReadAttribute.h5", BlitzH5File::Replace);
    blitz::Array<float,1> data(1);
    outFile.writeDataset(data, "/group/dataset");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
    return;
  }

  hid_t fileId = H5Fopen("testReadAttribute.h5", H5F_ACC_RDWR, H5P_DEFAULT);
  if (fileId < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not open test file");
    return;
  }
  hsize_t size = 3;
  hid_t dataspaceId = H5Screate_simple(1, &size, NULL);
  if (dataspaceId < 0)
  {
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
    return;
  }
  hid_t datasetId = H5Dopen2(fileId, "/group/dataset", H5P_DEFAULT);
  if (datasetId < 0)
  {
    H5Sclose(dataspaceId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not open dataset");
    return;
  }
  hid_t attributeId =
      H5Acreate2(datasetId, attributeName.c_str(), test_traits<WriteT>::h5Type,
                 dataspaceId, H5P_DEFAULT, H5P_DEFAULT);
  if (attributeId < 0)
  {
    H5Dclose(datasetId);
    H5Sclose(dataspaceId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create attribute");
    return;
  }
  herr_t err = H5Awrite(
      attributeId, test_traits<WriteT>::h5Type, attValue.data());
  H5Aclose(attributeId);
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId); 
  if (err < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not write attribute");
    return;
  }
  
  try
  {
    BlitzH5File inFile("testReadAttribute.h5");
    blitz::TinyVector<ReadT,3> value;
    inFile.readAttribute(value, attributeName, "/group/dataset");
    LMBUNIT_ASSERT(blitz::all(value == blitz::TinyVector<ReadT,3>(attValue)));
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
    return;
  }  
}

template<typename WriteT, typename ReadT>
static void testReadAttributeVectorInGroup()
{
  std::string attributeName = "VectorAttribute";
  std::vector<WriteT> attValue(10);
  for (size_t i = 0; i < attValue.size(); ++i) attValue[i] = i;

  hid_t fileId = H5Fcreate(
      "testReadAttribute.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
    return;
  }

  // Create group
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t groupId = H5Gcreate2(
      fileId, "/group/subgroup", linkCreationPropertiesId,
      H5P_DEFAULT, H5P_DEFAULT);
  H5Pclose(linkCreationPropertiesId);
  if (groupId < 0)
  {
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create group");
    return;
  }
  
  hsize_t size = attValue.size();
  hid_t dataspaceId = H5Screate_simple(1, &size, NULL);
  if (dataspaceId < 0)
  {
    H5Gclose(groupId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
    return;
  }
  hid_t attributeId =
      H5Acreate2(groupId, attributeName.c_str(), test_traits<WriteT>::h5Type,
                 dataspaceId, H5P_DEFAULT, H5P_DEFAULT);
  if (attributeId < 0)
  {
    H5Sclose(dataspaceId);
    H5Gclose(groupId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create attribute");
    return;
  }
  herr_t err = H5Awrite(
      attributeId, test_traits<WriteT>::h5Type, attValue.data());
  H5Aclose(attributeId);
  H5Sclose(dataspaceId);
  H5Gclose(groupId);
  H5Fclose(fileId); 
  if (err < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not write attribute");
    return;
  }
  
  try
  {
    BlitzH5File inFile("testReadAttribute.h5");
    std::vector<ReadT> value;
    inFile.readAttribute(value, attributeName, "/group/subgroup");
    LMBUNIT_ASSERT(value.size() == attValue.size());
    bool equal = true;
    for (size_t i = 0; i < value.size() && equal; ++i)
        equal &= value[i] == ReadT(attValue[i]);
    LMBUNIT_ASSERT(equal);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
    return;
  }  
}

template<typename WriteT, typename ReadT>
static void testReadAttributeVectorInDataset()
{
  std::string attributeName = "VectorAttribute";
  std::vector<WriteT> attValue(10);
  for (size_t i = 0; i < attValue.size(); ++i) attValue[i] = i;

  // Create dataset
  try
  {
    BlitzH5File outFile("testReadAttribute.h5", BlitzH5File::Replace);
    blitz::Array<float,1> data(1);
    outFile.writeDataset(data, "/group/dataset");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
    return;
  }

  hid_t fileId = H5Fopen("testReadAttribute.h5", H5F_ACC_RDWR, H5P_DEFAULT);
  if (fileId < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not open test file");
    return;
  }
  hsize_t size = attValue.size();
  hid_t dataspaceId = H5Screate_simple(1, &size, NULL);
  if (dataspaceId < 0)
  {
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
    return;
  }
  hid_t datasetId = H5Dopen2(fileId, "/group/dataset", H5P_DEFAULT);
  if (datasetId < 0)
  {
    H5Sclose(dataspaceId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not open dataset");
    return;
  }
  hid_t attributeId =
      H5Acreate2(datasetId, attributeName.c_str(), test_traits<WriteT>::h5Type,
                 dataspaceId, H5P_DEFAULT, H5P_DEFAULT);
  if (attributeId < 0)
  {
    H5Dclose(datasetId);
    H5Sclose(dataspaceId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create attribute");
    return;
  }
  herr_t err = H5Awrite(
      attributeId, test_traits<WriteT>::h5Type, attValue.data());
  H5Aclose(attributeId);
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId); 
  if (err < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not write attribute");
    return;
  }
  
  try
  {
    BlitzH5File inFile("testReadAttribute.h5");
    std::vector<ReadT> value;
    inFile.readAttribute(value, attributeName, "/group/dataset");
    LMBUNIT_ASSERT(value.size() == attValue.size());
    bool equal = true;
    for (size_t i = 0; i < value.size() && equal; ++i)
        equal &= value[i] == ReadT(attValue[i]);
    LMBUNIT_ASSERT(equal);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
    return;
  }  
}
                 
template<typename WriteT, typename ReadT>
static void testReadAttributeScalarInGroup()
{
  std::string attributeName = "ScalarAttribute";
  WriteT attValue = 1;

  hid_t fileId = H5Fcreate(
      "testReadAttribute.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  if (fileId < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create test file");
    return;
  }

  // Create group
  hid_t linkCreationPropertiesId = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(linkCreationPropertiesId, 1);
  hid_t groupId = H5Gcreate2(
      fileId, "/group/subgroup", linkCreationPropertiesId,
      H5P_DEFAULT, H5P_DEFAULT);
  H5Pclose(linkCreationPropertiesId);
  if (groupId < 0)
  {
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create group");
    return;
  }
  
  hid_t dataspaceId = H5Screate(H5S_SCALAR);
  if (dataspaceId < 0)
  {
    H5Gclose(groupId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
    return;
  }
  hid_t attributeId =
      H5Acreate2(groupId, attributeName.c_str(), test_traits<WriteT>::h5Type,
                 dataspaceId, H5P_DEFAULT, H5P_DEFAULT);
  if (attributeId < 0)
  {
    H5Sclose(dataspaceId);
    H5Gclose(groupId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create attribute");
    return;
  }
  herr_t err = H5Awrite(
      attributeId, test_traits<WriteT>::h5Type, &attValue);
  H5Aclose(attributeId);
  H5Sclose(dataspaceId);
  H5Gclose(groupId);
  H5Fclose(fileId); 
  if (err < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not write attribute");
    return;
  }
  
  try
  {
    BlitzH5File inFile("testReadAttribute.h5");
    ReadT value = -1;
    inFile.readAttribute(value, attributeName, "/group/subgroup");
    LMBUNIT_ASSERT_EQUAL(value, ReadT(attValue));
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
    return;
  }  
}

template<typename WriteT, typename ReadT>
static void testReadAttributeScalarInDataset()
{
  std::string attributeName = "ScalarAttribute";
  WriteT attValue = 1;

  // Create dataset
  try
  {
    BlitzH5File outFile("testReadAttribute.h5", BlitzH5File::Replace);
    blitz::Array<float,1> data(1);
    outFile.writeDataset(data, "/group/dataset");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
    return;
  }

  hid_t fileId = H5Fopen("testReadAttribute.h5", H5F_ACC_RDWR, H5P_DEFAULT);
  if (fileId < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not open test file");
    return;
  }
  hid_t dataspaceId = H5Screate(H5S_SCALAR);
  if (dataspaceId < 0)
  {
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create data space");
    return;
  }
  hid_t datasetId = H5Dopen2(fileId, "/group/dataset", H5P_DEFAULT);
  if (datasetId < 0)
  {
    H5Sclose(dataspaceId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not open dataset");
    return;
  }
  hid_t attributeId =
      H5Acreate2(datasetId, attributeName.c_str(), test_traits<WriteT>::h5Type,
                 dataspaceId, H5P_DEFAULT, H5P_DEFAULT);
  if (attributeId < 0)
  {
    H5Dclose(datasetId);
    H5Sclose(dataspaceId);
    H5Fclose(fileId);
    LMBUNIT_WRITE_FAILURE("Internal error: Could not create attribute");
    return;
  }
  herr_t err = H5Awrite(
      attributeId, test_traits<WriteT>::h5Type, &attValue);
  H5Aclose(attributeId);
  H5Dclose(datasetId);
  H5Sclose(dataspaceId);
  H5Fclose(fileId); 
  if (err < 0)
  {
    LMBUNIT_WRITE_FAILURE("Internal error: Could not write attribute");
    return;
  }
  
  try
  {
    BlitzH5File inFile("testReadAttribute.h5");
    ReadT value = -1;
    inFile.readAttribute(value, attributeName, "/group/dataset");
    LMBUNIT_ASSERT_EQUAL(value, ReadT(attValue));
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
    return;
  }  
}

int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST(testReadAttributeStringInRootGroup());
  LMBUNIT_RUN_TEST(testReadAttributeStringInGroup());
  LMBUNIT_RUN_TEST(testReadAttributeStringInDataset());
  LMBUNIT_RUN_TEST(testReadAttributeStringAsCharArrayInGroup());
  LMBUNIT_RUN_TEST(testReadAttributeStringAsCharArrayInDataset());
  LMBUNIT_RUN_TEST((testReadAttributeTinyVectorInGroup<float,float>()));
  LMBUNIT_RUN_TEST((testReadAttributeTinyVectorInGroup<float,int>()));
  LMBUNIT_RUN_TEST((testReadAttributeTinyVectorInGroup<int,double>()));
  LMBUNIT_RUN_TEST((testReadAttributeTinyVectorInDataset<float,float>()));
  LMBUNIT_RUN_TEST((testReadAttributeTinyVectorInDataset<float,int>()));
  LMBUNIT_RUN_TEST((testReadAttributeTinyVectorInDataset<int,double>()));
  LMBUNIT_RUN_TEST((testReadAttributeVectorInGroup<float,float>()));
  LMBUNIT_RUN_TEST((testReadAttributeVectorInGroup<float,int>()));
  LMBUNIT_RUN_TEST((testReadAttributeVectorInGroup<int,double>()));
  LMBUNIT_RUN_TEST((testReadAttributeVectorInDataset<float,float>()));
  LMBUNIT_RUN_TEST((testReadAttributeVectorInDataset<float,int>()));
  LMBUNIT_RUN_TEST((testReadAttributeVectorInDataset<int,double>()));
  LMBUNIT_RUN_TEST((testReadAttributeScalarInGroup<float,float>()));
  LMBUNIT_RUN_TEST((testReadAttributeScalarInGroup<float,int>()));
  LMBUNIT_RUN_TEST((testReadAttributeScalarInGroup<int,double>()));
  LMBUNIT_RUN_TEST((testReadAttributeScalarInDataset<float,float>()));
  LMBUNIT_RUN_TEST((testReadAttributeScalarInDataset<float,int>()));
  LMBUNIT_RUN_TEST((testReadAttributeScalarInDataset<int,double>()));

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
