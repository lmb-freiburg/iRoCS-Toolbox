#include "lmbunit.hh"

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

template<typename DataT, int Dim>
struct test_traits< blitz::TinyVector<DataT,Dim> >
{
  static hid_t h5Type;
  static std::string name;
};

template<typename DataT, int Dim>
hid_t test_traits< blitz::TinyVector<DataT,Dim> >::h5Type =
    test_traits<DataT>::h5Type();
template<typename DataT, int Dim>
std::string test_traits< blitz::TinyVector<DataT,Dim> >::name =
    "TinyVector<" + test_traits<DataT>::name + ",Dim>";

template<typename DataT>
static void testDatasetType()
{
  blitz::TinyVector<ptrdiff_t,3> dataShape(2, 3, 4);
  blitz::Array<DataT,3> data(dataShape);
  try
  {
    BlitzH5File outFile("testDatasetType.h5", BlitzH5File::Replace);
    outFile.writeDataset(data, "/testDatasetType");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
  }
  
  try
  {
    BlitzH5File inFile("testDatasetType.h5");
    hid_t dataType = inFile.getDatasetType("/testDatasetType");
    LMBUNIT_ASSERT(H5Tequal(dataType, test_traits<DataT>::h5Type));
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
  }
}

template<typename DataT>
static void testDatasetTypeVec()
{
  blitz::TinyVector<ptrdiff_t,3> dataShape(2, 3, 4);
  blitz::Array<blitz::TinyVector<DataT,3>,3> data(dataShape);
  try
  {
    BlitzH5File outFile("testDatasetType.h5", BlitzH5File::Replace);
    outFile.writeDataset(data, "/testDatasetType");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
  }
  
  try
  {
    BlitzH5File inFile("testDatasetType.h5");
    hid_t dataType = inFile.getDatasetType("/testDatasetType");
    LMBUNIT_ASSERT(H5Tequal(dataType, test_traits<DataT>::h5Type));
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown error");
  }
}

int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST((testDatasetType<unsigned char>()));
  LMBUNIT_RUN_TEST((testDatasetType<char>()));
  LMBUNIT_RUN_TEST((testDatasetType<unsigned short>()));
  LMBUNIT_RUN_TEST((testDatasetType<short>()));
  LMBUNIT_RUN_TEST((testDatasetType<unsigned int>()));
  LMBUNIT_RUN_TEST((testDatasetType<int>()));
  LMBUNIT_RUN_TEST((testDatasetType<unsigned long>()));
  LMBUNIT_RUN_TEST((testDatasetType<long>()));
  LMBUNIT_RUN_TEST((testDatasetType<unsigned long long>()));
  LMBUNIT_RUN_TEST((testDatasetType<long long>()));
  LMBUNIT_RUN_TEST((testDatasetType<float>()));
  LMBUNIT_RUN_TEST((testDatasetType<double>()));
  LMBUNIT_RUN_TEST((testDatasetType<long double>()));

  LMBUNIT_RUN_TEST((testDatasetTypeVec<float>()));

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
