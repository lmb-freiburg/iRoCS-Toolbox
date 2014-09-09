#include "lmbunit.hh"

#include <libBlitzHdf5/BlitzHdf5Light.hh>

static void testDeleteDataset()
{
  blitz::Array<float,3> data(2, 3, 4);
  try
  {
    BlitzH5File outFile("testDeleteDataset.h5", BlitzH5File::Replace);
    outFile.writeDataset(data, "/deleteMe");
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
  
  try
  {
    BlitzH5File outFile("testDeleteDataset.h5", BlitzH5File::Write);
    outFile.deleteDataset("/deleteMe");
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
  
  try
  {
    BlitzH5File inFile("testDeleteDataset.h5");
    LMBUNIT_ASSERT(!inFile.existsDataset("/deleteMe"));
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
  
}

int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST(testDeleteDataset());

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
