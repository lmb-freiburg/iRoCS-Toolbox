#include "lmbunit.hh"

#include <libBlitzHdf5/BlitzHdf5Light.hh>

static void testGetObjectsEmpty()
{
  try
  {
    BlitzH5File outFile("testGetObjects.h5", BlitzH5File::Replace);
    std::vector<std::string> objects(outFile.getObjects());
    LMBUNIT_ASSERT_EQUAL(objects.size(), 0);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught unexpected BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught unexpected std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown exception");
  }
}
                 
static void testGetObjectsRootGroup()
{
  try
  {
    BlitzH5File outFile("testGetObjects.h5", BlitzH5File::Replace);
    blitz::Array<float,1> data(1);
    data = 1;
    outFile.writeDataset(data, "dataset1");
    outFile.writeDataset(data, "group1/dataset2");
    std::vector<std::string> objects(outFile.getObjects());
    LMBUNIT_ASSERT_EQUAL(objects.size(), 2);
    LMBUNIT_ASSERT_EQUAL(objects[0], "dataset1");
    LMBUNIT_ASSERT_EQUAL(objects[1], "group1");  
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught unexpected BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught unexpected std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown exception");
  }
}

static void testGetObjectsGroupsOnly()
{
  try
  {
    BlitzH5File outFile("testGetObjects.h5", BlitzH5File::Replace);
    blitz::Array<float,1> data(1);
    data = 1;
    outFile.writeDataset(data, "group/subgroup1/dataset1");
    outFile.writeDataset(data, "group/subgroup2/dataset2");
    std::vector<std::string> objects(outFile.getObjects("group"));
    LMBUNIT_ASSERT_EQUAL(objects.size(), 2);
    LMBUNIT_ASSERT_EQUAL(objects[0], "subgroup1");
    LMBUNIT_ASSERT_EQUAL(objects[1], "subgroup2");  
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught unexpected BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught unexpected std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown exception");
  }
}

static void testGetObjectsDatasetsOnly()
{
  try
  {
    BlitzH5File outFile("testGetObjects.h5", BlitzH5File::Replace);
    blitz::Array<float,1> data(1);
    data = 1;
    outFile.writeDataset(data, "group/dataset1");
    outFile.writeDataset(data, "group/dataset2");
    std::vector<std::string> objects(outFile.getObjects("group"));
    LMBUNIT_ASSERT_EQUAL(objects.size(), 2);
    LMBUNIT_ASSERT_EQUAL(objects[0], "dataset1");
    LMBUNIT_ASSERT_EQUAL(objects[1], "dataset2");  
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught unexpected BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught unexpected std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown exception");
  }
}

static void testGetObjectsMixed()
{
  try
  {
    BlitzH5File outFile("testGetObjects.h5", BlitzH5File::Replace);
    blitz::Array<float,1> data(1);
    data = 1;
    outFile.writeDataset(data, "group/dataset1");
    outFile.writeDataset(data, "group/subgroup/dataset2");
    std::vector<std::string> objects(outFile.getObjects("group"));
    LMBUNIT_ASSERT_EQUAL(objects.size(), 2);
    LMBUNIT_ASSERT_EQUAL(objects[0], "dataset1");
    LMBUNIT_ASSERT_EQUAL(objects[1], "subgroup");  
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught unexpected BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught unexpected std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown exception");
  }
}

static void testGetObjectsMixedWithGlobalAttributes()
{
  try
  {
    BlitzH5File outFile("testGetObjects.h5", BlitzH5File::Replace);
    blitz::Array<float,1> data(1);
    data = 1;
    outFile.writeDataset(data, "group/dataset1");
    outFile.writeDataset(data, "group/subgroup/dataset2");
    outFile.writeAttribute(1, "globalAttribute", "group");
    std::vector<std::string> objects(outFile.getObjects("group"));
    LMBUNIT_ASSERT_EQUAL(objects.size(), 2);
    LMBUNIT_ASSERT_EQUAL(objects[0], "dataset1");
    LMBUNIT_ASSERT_EQUAL(objects[1], "subgroup");  
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught unexpected BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught unexpected std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown exception");
  }
}

static void testGetObjectsNoSuchGroup()
{
  BlitzH5File outFile("testGetObjects.h5", BlitzH5File::Replace);
  try
  {
    std::vector<std::string> objects(outFile.getObjects("group"));
    LMBUNIT_WRITE_FAILURE("Expected BlitzH5Error not thrown.");
  }
  catch (BlitzH5Error &)
  {
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(
        std::string("Caught unexpected std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("Caught unknown exception");
  }
}

int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST(testGetObjectsEmpty());
  LMBUNIT_RUN_TEST(testGetObjectsRootGroup());
  LMBUNIT_RUN_TEST(testGetObjectsGroupsOnly());
  LMBUNIT_RUN_TEST(testGetObjectsDatasetsOnly());
  LMBUNIT_RUN_TEST(testGetObjectsMixed());
  LMBUNIT_RUN_TEST(testGetObjectsMixedWithGlobalAttributes());
  LMBUNIT_RUN_TEST(testGetObjectsNoSuchGroup());
  
  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
