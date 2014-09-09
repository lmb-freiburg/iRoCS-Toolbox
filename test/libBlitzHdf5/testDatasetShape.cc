#include "lmbunit.hh"

#include <libBlitzHdf5/BlitzHdf5Light.hh>

#if defined HAVE_CONFIG_H && !defined PACKAGE
#include <config.hh>
#endif

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

static void testDatasetShapeScalar()
{
  float data = 1.0f;
  try
  {
    BlitzH5File outFile("testDatasetShape.h5", BlitzH5File::Replace);
    outFile.writeDataset(data, "/testDatasetShapeScalar");
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
    BlitzH5File inFile("testDatasetShape.h5");
    std::vector<hsize_t> shape(
        inFile.getDatasetShape("/testDatasetShapeScalar"));
    LMBUNIT_ASSERT_EQUAL(shape.size(), 0);
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


static void testDatasetShape3D()
{
  blitz::TinyVector<ptrdiff_t,3> dataShape(2, 3, 4);
  blitz::Array<float,3> data(dataShape);
  try
  {
    BlitzH5File outFile("testDatasetShape.h5", BlitzH5File::Replace);
    outFile.writeDataset(data, "/testDatasetShape3D");
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
    BlitzH5File inFile("testDatasetShape.h5");
    std::vector<hsize_t> shape(inFile.getDatasetShape("/testDatasetShape3D"));
    LMBUNIT_ASSERT_EQUAL(shape.size(), 3);
    blitz::TinyVector<ptrdiff_t,3> readShape;
    for (int d = 0; d < 3; ++d) readShape(d) = shape[d];
    LMBUNIT_ASSERT(blitz::all(readShape == dataShape));
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

static void testDatasetShape3DVec()
{
  blitz::TinyVector<ptrdiff_t,3> dataShape(2, 3, 4);
  blitz::Array<blitz::TinyVector<float,3>,3> data(dataShape);
  try
  {
    BlitzH5File outFile("testDatasetShape.h5", BlitzH5File::Replace);
    outFile.writeDataset(data, "/testDatasetShape3D");
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
    BlitzH5File inFile("testDatasetShape.h5");
    std::vector<hsize_t> shape(inFile.getDatasetShape("/testDatasetShape3D"));
    LMBUNIT_ASSERT_EQUAL(shape.size(), 4);
    blitz::TinyVector<ptrdiff_t,3> readShape;
    for (int d = 0; d < 3; ++d) readShape(d) = shape[d];
    LMBUNIT_ASSERT(blitz::all(readShape == dataShape));
    LMBUNIT_ASSERT_EQUAL(shape[3], 3);
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
  LMBUNIT_RUN_TEST(testDatasetShapeScalar());
  LMBUNIT_RUN_TEST(testDatasetShape3D());
  LMBUNIT_RUN_TEST(testDatasetShape3DVec());

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
