#include "lmbunit.hh"

#include <typeinfo>

#if defined HAVE_CONFIG_H && !defined PACKAGE
#include <config.hh>
#endif

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

#include <libBlitzHdf5/BlitzHdf5Light.hh>

static void testWriteAttributeStringInRootGroup()
{
  std::string dataWritten = "my cool string attribute";
  try
  {
    BlitzH5File outFile("testWriteAttribute.h5", BlitzH5File::Replace);
    outFile.writeAttribute(dataWritten, "stringAttribute");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }

  try
  {
    std::string dataRead;
    BlitzH5File inFile("testWriteAttribute.h5");
    inFile.readAttribute(dataRead, "stringAttribute");
    LMBUNIT_ASSERT_EQUAL(dataRead, dataWritten);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }  
}

static void testWriteAttributeStringInGroup()
{
  std::string dataWritten = "my cool string attribute";
  try
  {
    BlitzH5File outFile("testWriteAttribute.h5", BlitzH5File::Replace);
    outFile.writeAttribute(dataWritten, "stringAttribute", "/some/subgroup");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }

  try
  {
    std::string dataRead;
    BlitzH5File inFile("testWriteAttribute.h5");
    inFile.readAttribute(dataRead, "stringAttribute", "/some/subgroup");
    LMBUNIT_ASSERT_EQUAL(dataRead, dataWritten);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }  
}

static void testWriteAttributeStringInDataset()
{
  std::string dataWritten = "my cool string attribute";
  try
  {
    BlitzH5File outFile("testWriteAttribute.h5", BlitzH5File::Replace);
    blitz::Array<float,1> data(1);
    data(0) = 1.0f;
    outFile.writeDataset(data, "/some/dataset");
    outFile.writeAttribute(dataWritten, "stringAttribute", "/some/dataset");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }

  try
  {
    std::string dataRead;
    BlitzH5File inFile("testWriteAttribute.h5");
    inFile.readAttribute(dataRead, "stringAttribute", "/some/dataset");
    LMBUNIT_ASSERT_EQUAL(dataRead, dataWritten);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }  
}

template<typename WriteT>
static void testWriteAttributeTinyVectorInGroup()
{
  blitz::TinyVector<WriteT,3> dataWritten(1, 2, 3);
  try
  {
    BlitzH5File outFile("testWriteAttribute.h5", BlitzH5File::Replace);
    outFile.writeAttribute(dataWritten, "tinyvecAttribute", "/some/subgroup");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }

  try
  {
    blitz::TinyVector<WriteT,3> dataRead;
    BlitzH5File inFile("testWriteAttribute.h5");
    inFile.readAttribute(dataRead, "tinyvecAttribute", "/some/subgroup");
    LMBUNIT_ASSERT(blitz::all(dataWritten == dataRead));
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }  
}

template<typename WriteT>
static void testWriteAttributeTinyVectorInDataset()
{
  blitz::TinyVector<WriteT,3> dataWritten(1, 2, 3);
  try
  {
    BlitzH5File outFile("testWriteAttribute.h5", BlitzH5File::Replace);
    blitz::Array<float,1> data(1);
    data(0) = 1.0f;
    outFile.writeDataset(data, "/some/dataset");
    outFile.writeAttribute(dataWritten, "tinyvecAttribute", "/some/dataset");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }

  try
  {
    blitz::TinyVector<WriteT,3> dataRead;
    BlitzH5File inFile("testWriteAttribute.h5");
    inFile.readAttribute(dataRead, "tinyvecAttribute", "/some/dataset");
    LMBUNIT_ASSERT(blitz::all(dataWritten == dataRead));
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }  
}

template<typename WriteT>
static void testWriteAttributeVectorInGroup()
{
  std::vector<WriteT> dataWritten(10);
  for (size_t i = 0; i < dataWritten.size(); ++i) dataWritten[i] = i;
  try
  {
    BlitzH5File outFile("testWriteAttribute.h5", BlitzH5File::Replace);
    outFile.writeAttribute(dataWritten, "vecAttribute", "/some/subgroup");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }

  try
  {
    std::vector<WriteT> dataRead;
    BlitzH5File inFile("testWriteAttribute.h5");
    inFile.readAttribute(dataRead, "vecAttribute", "/some/subgroup");
    LMBUNIT_ASSERT_EQUAL(dataWritten.size(), dataRead.size());
    bool dataEqual = true;
    for (size_t i = 0; i < dataRead.size() && dataEqual; ++i)
        dataEqual &= dataRead[i] == dataWritten[i];
    LMBUNIT_ASSERT(dataEqual);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }  
}

template<typename WriteT>
static void testWriteAttributeVectorInDataset()
{
  std::vector<WriteT> dataWritten(10);
  for (size_t i = 0; i < dataWritten.size(); ++i) dataWritten[i] = i;
  try
  {
    BlitzH5File outFile("testWriteAttribute.h5", BlitzH5File::Replace);
    blitz::Array<float,1> data(1);
    data(0) = 1.0f;
    outFile.writeDataset(data, "/some/dataset");
    outFile.writeAttribute(dataWritten, "vecAttribute", "/some/dataset");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }

  try
  {
    std::vector<WriteT> dataRead;
    BlitzH5File inFile("testWriteAttribute.h5");
    inFile.readAttribute(dataRead, "vecAttribute", "/some/dataset");
    LMBUNIT_ASSERT_EQUAL(dataWritten.size(), dataRead.size());
    bool dataEqual = true;
    for (size_t i = 0; i < dataRead.size() && dataEqual; ++i)
        dataEqual &= dataRead[i] == dataWritten[i];
    LMBUNIT_ASSERT(dataEqual);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }  
}
                 
template<typename WriteT>
static void testWriteAttributeScalarInGroup()
{
  WriteT dataWritten = 1;
  try
  {
    BlitzH5File outFile("testWriteAttribute.h5", BlitzH5File::Replace);
    outFile.writeAttribute(dataWritten, "scalarAttribute", "/some/subgroup");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }

  try
  {
    WriteT dataRead;
    BlitzH5File inFile("testWriteAttribute.h5");
    inFile.readAttribute(dataRead, "scalarAttribute", "/some/subgroup");
    LMBUNIT_ASSERT_EQUAL(dataWritten, dataRead);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }  
}

template<typename WriteT>
static void testWriteAttributeScalarInDataset()
{
  WriteT dataWritten = 1;
  try
  {
    BlitzH5File outFile("testWriteAttribute.h5", BlitzH5File::Replace);
    blitz::Array<float,1> data(1);
    data(0) = 1.0f;
    outFile.writeDataset(data, "/some/dataset");
    outFile.writeAttribute(dataWritten, "scalarAttribute", "/some/dataset");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }

  try
  {
    WriteT dataRead;
    BlitzH5File inFile("testWriteAttribute.h5");
    inFile.readAttribute(dataRead, "scalarAttribute", "/some/dataset");
    LMBUNIT_ASSERT_EQUAL(dataWritten, dataRead);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
    return;
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
    return;
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
    return;
  }  
}

int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST(testWriteAttributeStringInRootGroup());
  LMBUNIT_RUN_TEST(testWriteAttributeStringInGroup());
  LMBUNIT_RUN_TEST(testWriteAttributeStringInDataset());
  LMBUNIT_RUN_TEST((testWriteAttributeTinyVectorInGroup<float>()));
  LMBUNIT_RUN_TEST((testWriteAttributeTinyVectorInGroup<int>()));
  LMBUNIT_RUN_TEST((testWriteAttributeTinyVectorInGroup<double>()));
  LMBUNIT_RUN_TEST((testWriteAttributeTinyVectorInDataset<float>()));
  LMBUNIT_RUN_TEST((testWriteAttributeTinyVectorInDataset<int>()));
  LMBUNIT_RUN_TEST((testWriteAttributeTinyVectorInDataset<double>()));
  LMBUNIT_RUN_TEST((testWriteAttributeVectorInGroup<float>()));
  LMBUNIT_RUN_TEST((testWriteAttributeVectorInGroup<int>()));
  LMBUNIT_RUN_TEST((testWriteAttributeVectorInGroup<double>()));
  LMBUNIT_RUN_TEST((testWriteAttributeVectorInDataset<float>()));
  LMBUNIT_RUN_TEST((testWriteAttributeVectorInDataset<int>()));
  LMBUNIT_RUN_TEST((testWriteAttributeVectorInDataset<double>()));
  LMBUNIT_RUN_TEST((testWriteAttributeScalarInGroup<float>()));
  LMBUNIT_RUN_TEST((testWriteAttributeScalarInGroup<int>()));
  LMBUNIT_RUN_TEST((testWriteAttributeScalarInGroup<double>()));
  LMBUNIT_RUN_TEST((testWriteAttributeScalarInDataset<float>()));
  LMBUNIT_RUN_TEST((testWriteAttributeScalarInDataset<int>()));
  LMBUNIT_RUN_TEST((testWriteAttributeScalarInDataset<double>()));

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
