#include "lmbunit.hh"

#include <libBlitzHdf5/BlitzHdf5Light.hh>

static void testBlitzH5FileConstructorReadOnly()
{
  try
  {
    BlitzH5File inFile("testBlitzH5FileConstructorReadOnly.h5");
    LMBUNIT_WRITE_FAILURE(
        "File could be opened in 'ReadOnly' mode although it did not exist.");
  }
  catch (BlitzH5Error &)
  {}
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
  }

  hid_t fileId = H5Fcreate(
      "testBlitzH5FileConstructorReadOnly.h5", H5F_ACC_TRUNC,
      H5P_DEFAULT, H5P_DEFAULT);
  H5Fclose(fileId);

  try
  {
    BlitzH5File inFile("testBlitzH5FileConstructorReadOnly.h5");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
  }

  system("rm testBlitzH5FileConstructorReadOnly.h5");
}

static void testBlitzH5FileConstructorWrite()
{
  try
  {
    BlitzH5File inFile(
        "testBlitzH5FileConstructorWrite.h5", BlitzH5File::Write);
    LMBUNIT_WRITE_FAILURE(
        "File could be opened in 'Write' mode although it did not exist.");
  }
  catch (BlitzH5Error &)
  {}
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
  }

  hid_t fileId = H5Fcreate(
      "testBlitzH5FileConstructorWrite.h5", H5F_ACC_TRUNC,
      H5P_DEFAULT, H5P_DEFAULT);
  H5Fclose(fileId);

  try
  {
    BlitzH5File inFile(
        "testBlitzH5FileConstructorWrite.h5", BlitzH5File::Write);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
  }

  system("rm testBlitzH5FileConstructorWrite.h5");  
}
    
static void testBlitzH5FileConstructorReplace()
{
  try
  {
    BlitzH5File inFile(
        "testBlitzH5FileConstructorReplace.h5", BlitzH5File::Replace);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
  }

  std::ofstream outfileStream(
      "testBlitzH5FileConstructorReplace.h5", std::ios::trunc);
  outfileStream << "Testfile";
  outfileStream.close();

  try
  {
    BlitzH5File inFile("testBlitzH5FileConstructorReplace.h5");
    LMBUNIT_WRITE_FAILURE(
        "File could be opened in 'ReadOnly' mode although it is no hdf5 file.");
  }
  catch (BlitzH5Error &)
  {}
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
  }

  try
  {
    BlitzH5File inFile(
        "testBlitzH5FileConstructorReplace.h5", BlitzH5File::Replace);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
  }

  try
  {
    BlitzH5File inFile("testBlitzH5FileConstructorReplace.h5");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
  }

  system("rm testBlitzH5FileConstructorReplace.h5");
}

static void testBlitzH5FileConstructorNew()
{
  try
  {
    BlitzH5File inFile("testBlitzH5FileConstructorNew.h5", BlitzH5File::New);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
  }

  try
  {
    BlitzH5File inFile("testBlitzH5FileConstructorNew.h5", BlitzH5File::New);
    LMBUNIT_WRITE_FAILURE(
        "File could be opened in 'New' mode although it already existed.");
  }
  catch (BlitzH5Error &)
  {}
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
  }

  system("rm testBlitzH5FileConstructorNew.h5");
}

static void testBlitzH5FileConstructorWriteOrNew()
{
  try
  {
    BlitzH5File inFile(
        "testBlitzH5FileConstructorWriteOrNew.h5", BlitzH5File::WriteOrNew);
  }
  catch (BlitzH5Error &e)
  {   
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
  }

  try
  {
    BlitzH5File inFile(
        "testBlitzH5FileConstructorWriteOrNew.h5", BlitzH5File::WriteOrNew);
  }
  catch (BlitzH5Error &e)
  {   
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
  }

  std::ofstream outfileStream(
      "testBlitzH5FileConstructorWriteOrNew.h5", std::ios::trunc);
  outfileStream << "Testfile";
  outfileStream.close();

  try
  {
    BlitzH5File inFile("testBlitzH5FileConstructorWriteOrNew.h5");
    LMBUNIT_WRITE_FAILURE(
        "File could be opened in 'WriteOrNew' mode although a non-hdf5 file "
        "with that name already exists.");
  }
  catch (BlitzH5Error &)
  {}
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
  }

  system("rm testBlitzH5FileConstructorWriteOrNew.h5");
}
                 
static void testBlitzH5FileConstructorWriteAfterReadOnly()
{
  hid_t fileId = H5Fcreate(
      "testBlitzH5FileConstructorWriteAfterReadOnly.h5", H5F_ACC_TRUNC,
      H5P_DEFAULT, H5P_DEFAULT);
  H5Fclose(fileId);
  
  try
  {
    BlitzH5File inFile("testBlitzH5FileConstructorWriteAfterReadOnly.h5");
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
  }

  try
  {
    BlitzH5File inFile(
        "testBlitzH5FileConstructorWriteAfterReadOnly.h5", BlitzH5File::Write);
  }
  catch (BlitzH5Error &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("BlitzH5Error: ") + e.what());
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("std::exception: ") + e.what());
  }
  catch (...)
  {
    LMBUNIT_WRITE_FAILURE("unknown error");
  }

  system("rm testBlitzH5FileConstructorWriteAfterReadOnly.h5");
}

int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST(testBlitzH5FileConstructorReadOnly());
  LMBUNIT_RUN_TEST(testBlitzH5FileConstructorWrite());
  LMBUNIT_RUN_TEST(testBlitzH5FileConstructorReplace());
  LMBUNIT_RUN_TEST(testBlitzH5FileConstructorNew());
  LMBUNIT_RUN_TEST(testBlitzH5FileConstructorWriteOrNew());
  LMBUNIT_RUN_TEST(testBlitzH5FileConstructorWriteAfterReadOnly())

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
