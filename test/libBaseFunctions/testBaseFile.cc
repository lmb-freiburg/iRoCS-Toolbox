#include "lmbunit.hh"

#include <fstream>

#include <libBaseFunctions/BaseFile.hh>

static void testBaseName()
{
  std::string somePath("simpleRelativePath.txt");
  LMBUNIT_ASSERT_EQUAL(BaseFile::BaseName(somePath), somePath);
  somePath = "../some/more/./complicated///path";
  LMBUNIT_ASSERT_EQUAL(BaseFile::BaseName(somePath), "path");
}

static void testDirName()
{
  std::string somePath("simpleRelativePath.txt");
  LMBUNIT_ASSERT_EQUAL(BaseFile::DirName(somePath), "");
  somePath = "../some/more/./complicated///path";
  LMBUNIT_ASSERT_EQUAL(
      BaseFile::DirName(somePath), "../some/more/./complicated//");
}

static void testDirectoryStructure()
{
  LMBUNIT_ASSERT(
      !BaseFile::IsFile(TOP_BUILD_DIR "/test/libBaseFunctions/testTree"));
  LMBUNIT_ASSERT(
      !BaseFile::IsLink(TOP_BUILD_DIR "/test/libBaseFunctions/testTree"));
  LMBUNIT_ASSERT(
      !BaseFile::IsDirectory(TOP_BUILD_DIR "/test/libBaseFunctions/testTree"));
  LMBUNIT_ASSERT(
      !BaseFile::Exists(TOP_BUILD_DIR "/test/libBaseFunctions/testTree"));

  // Create directory
  LMBUNIT_ASSERT(
      BaseFile::DirectoryCreate(
          TOP_BUILD_DIR "/test/libBaseFunctions/testTree"));
  LMBUNIT_ASSERT(
      !BaseFile::IsFile(TOP_BUILD_DIR "/test/libBaseFunctions/testTree"));
  LMBUNIT_ASSERT(
      !BaseFile::IsLink(TOP_BUILD_DIR "/test/libBaseFunctions/testTree"));
  LMBUNIT_ASSERT(
      BaseFile::IsDirectory(TOP_BUILD_DIR "/test/libBaseFunctions/testTree"));
  LMBUNIT_ASSERT(
      BaseFile::Exists(TOP_BUILD_DIR "/test/libBaseFunctions/testTree"));

  // Rename directory
  LMBUNIT_ASSERT(
      BaseFile::Move(
          TOP_BUILD_DIR "/test/libBaseFunctions/testTree",
          TOP_BUILD_DIR "/test/libBaseFunctions/testTree2"));
  LMBUNIT_ASSERT(
      !BaseFile::IsFile(TOP_BUILD_DIR "/test/libBaseFunctions/testTree"));
  LMBUNIT_ASSERT(
      !BaseFile::IsLink(TOP_BUILD_DIR "/test/libBaseFunctions/testTree"));
  LMBUNIT_ASSERT(
      !BaseFile::IsDirectory(TOP_BUILD_DIR "/test/libBaseFunctions/testTree"));
  LMBUNIT_ASSERT(
      !BaseFile::Exists(TOP_BUILD_DIR "/test/libBaseFunctions/testTree"));
  LMBUNIT_ASSERT(
      !BaseFile::IsFile(TOP_BUILD_DIR "/test/libBaseFunctions/testTree2"));
  LMBUNIT_ASSERT(
      !BaseFile::IsLink(TOP_BUILD_DIR "/test/libBaseFunctions/testTree2"));
  LMBUNIT_ASSERT(
      BaseFile::IsDirectory(TOP_BUILD_DIR "/test/libBaseFunctions/testTree2"));
  LMBUNIT_ASSERT(
      BaseFile::Exists(TOP_BUILD_DIR "/test/libBaseFunctions/testTree2"));
  
  // Remove Directory
  LMBUNIT_ASSERT(
      BaseFile::Remove(TOP_BUILD_DIR "/test/libBaseFunctions/testTree2"));
  LMBUNIT_ASSERT(
      !BaseFile::IsFile(TOP_BUILD_DIR "/test/libBaseFunctions/testTree2"));
  LMBUNIT_ASSERT(
      !BaseFile::IsLink(TOP_BUILD_DIR "/test/libBaseFunctions/testTree2"));
  LMBUNIT_ASSERT(
      !BaseFile::IsDirectory(TOP_BUILD_DIR "/test/libBaseFunctions/testTree2"));
  LMBUNIT_ASSERT(
      !BaseFile::Exists(TOP_BUILD_DIR "/test/libBaseFunctions/testTree2"));

  // Create another directory
  LMBUNIT_ASSERT(
      BaseFile::DirectoryCreate(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated"));
  LMBUNIT_ASSERT(
      !BaseFile::IsFile(TOP_BUILD_DIR "/test/libBaseFunctions/populated"));
  LMBUNIT_ASSERT(
      !BaseFile::IsLink(TOP_BUILD_DIR "/test/libBaseFunctions/populated"));
  LMBUNIT_ASSERT(
      BaseFile::IsDirectory(TOP_BUILD_DIR "/test/libBaseFunctions/populated"));
  LMBUNIT_ASSERT(
      BaseFile::Exists(TOP_BUILD_DIR "/test/libBaseFunctions/populated"));

  // Create a file in that directory
  try
  {
    std::ofstream outFile(
        TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile.txt",
        std::ios::trunc);
    outFile << "Test";
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
  }
  LMBUNIT_ASSERT(
      BaseFile::IsFile(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::IsLink(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::IsDirectory(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile.txt"));
  LMBUNIT_ASSERT(
      BaseFile::Exists(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile.txt"));
  
  // Create another file in that directory
  try
  {
    std::ofstream outFile(
        TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile2.txt",
        std::ios::trunc);
    outFile << "Test2";
  }
  catch (std::exception &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught std::exception: ") + e.what());
  }
  LMBUNIT_ASSERT(
      BaseFile::IsFile(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile2.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::IsLink(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile2.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::IsDirectory(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile2.txt"));
  LMBUNIT_ASSERT(
      BaseFile::Exists(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile2.txt"));
  
  // Create another directory
  LMBUNIT_ASSERT(
      BaseFile::DirectoryCreate(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub"));
  LMBUNIT_ASSERT(
      !BaseFile::IsFile(TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub"));
  LMBUNIT_ASSERT(
      !BaseFile::IsLink(TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub"));
  LMBUNIT_ASSERT(
      BaseFile::IsDirectory(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub"));
  LMBUNIT_ASSERT(
      BaseFile::Exists(TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub"));

  // Create a link to the file in the sub directory
  int res = std::system(
      "ln -s " TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile.txt "
      TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub/outlink.txt");
  LMBUNIT_ASSERT_EQUAL(res, 0);
  LMBUNIT_ASSERT(
      !BaseFile::IsFile(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub/outlink.txt"));
  LMBUNIT_ASSERT(
      BaseFile::IsLink(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub/outlink.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::IsDirectory(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub/outlink.txt"));
  LMBUNIT_ASSERT(
      BaseFile::Exists(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub/outlink.txt"));

  // List the contents of the created folder
  std::vector<std::string> files;
  std::string pattern = "";
  LMBUNIT_ASSERT(
      BaseFile::ListDir(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated", files, pattern));
  LMBUNIT_ASSERT_EQUAL(files.size(), 2);
  LMBUNIT_ASSERT_EQUAL(
      files[0], TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile.txt");
  LMBUNIT_ASSERT_EQUAL(
      files[1], TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile2.txt");

  // ToDo: Implement: List the contents with filter
  // pattern = "*2.txt";
  // LMBUNIT_ASSERT(
  //     BaseFile::ListDir(
  //         TOP_BUILD_DIR "/test/libBaseFunctions/populated", files, pattern));
  // LMBUNIT_ASSERT_EQUAL(files.size(), 1);
  // if (files.size() != 1)
  // {
  //   LMBUNIT_DEBUG_STREAM << "files = ";
  //   for (size_t i = 0; i < files.size(); ++i)
  //       LMBUNIT_DEBUG_STREAM << "'" << files[i] << "' ";
  //   LMBUNIT_DEBUG_STREAM << std::endl;
  // }
  // LMBUNIT_ASSERT_EQUAL(
  //     files[0], TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile2.txt");

  // Get absolute path to real file
  LMBUNIT_ASSERT_EQUAL(
      BaseFile::BaseNamePath(
          TOP_BUILD_DIR
          "/./test/libBaseFunctions///populated/sub//outlink.txt"),
      TOP_BUILD_DIR "/test/libBaseFunctions/populated");

  // Move file
  LMBUNIT_ASSERT(
      BaseFile::Move(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile2.txt",
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub/outfile3.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::IsFile(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile2.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::IsLink(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile2.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::IsDirectory(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile2.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::Exists(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile2.txt"));
  LMBUNIT_ASSERT(
      BaseFile::IsFile(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub/outfile3.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::IsLink(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub/outfile3.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::IsDirectory(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub/outfile3.txt"));
  LMBUNIT_ASSERT(
      BaseFile::Exists(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub/outfile3.txt"));

  // Copy file
  LMBUNIT_ASSERT(
      BaseFile::Copy(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/outfile.txt",
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub/copy.txt"));
  LMBUNIT_ASSERT(
      BaseFile::IsFile(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub/copy.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::IsLink(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub/copy.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::IsDirectory(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub/copy.txt"));
  LMBUNIT_ASSERT(
      BaseFile::Exists(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated/sub/copy.txt"));

  // Rename whole tree
  LMBUNIT_ASSERT(
      BaseFile::Move(
          TOP_BUILD_DIR "/test/libBaseFunctions/populated",
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed"));
  LMBUNIT_ASSERT(
      BaseFile::IsFile(
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed/sub/outfile3.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::IsLink(
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed/sub/outfile3.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::IsDirectory(
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed/sub/outfile3.txt"));
  LMBUNIT_ASSERT(
      BaseFile::Exists(
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed/sub/outfile3.txt"));

  std::string newFileName = BaseFile::FindUniqueUnexistingName(
      TOP_BUILD_DIR "/test/libBaseFunctions/renamed/sub/xxxxxxxx.txt", 'x');
  LMBUNIT_ASSERT(!BaseFile::Exists(newFileName));

  // Remove whole tree (piece by piece. Recursive deletion is not supported)
  LMBUNIT_ASSERT(
      BaseFile::Remove(
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed/sub/copy.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::Exists(
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed/sub/copy.txt"));
  LMBUNIT_ASSERT(
      BaseFile::Remove(
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed/sub/outfile3.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::Exists(
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed/sub/outfile3.txt"));
  LMBUNIT_ASSERT(
      BaseFile::Remove(
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed/sub/outlink.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::Exists(
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed/sub/outlink.txt"));
  LMBUNIT_ASSERT(
      BaseFile::Remove(
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed/sub"));
  LMBUNIT_ASSERT(
      !BaseFile::Exists(
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed/sub"));
  LMBUNIT_ASSERT(
      BaseFile::Remove(
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed/outfile.txt"));
  LMBUNIT_ASSERT(
      !BaseFile::Exists(
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed/outfile.txt"));
  LMBUNIT_ASSERT(
      BaseFile::Remove(
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed"));
  LMBUNIT_ASSERT(
      !BaseFile::Exists(
          TOP_BUILD_DIR "/test/libBaseFunctions/renamed"));
}

static void testBeautifyFilePath()
{
  std::string somePath = "../some/./complicated///path";
  LMBUNIT_ASSERT_EQUAL(
      BaseFile::BeautifyFilePath(somePath), "../some/complicated/path");
}

int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST(testBaseName());
  LMBUNIT_RUN_TEST(testDirName());
  LMBUNIT_RUN_TEST(testDirectoryStructure());
  LMBUNIT_RUN_TEST(testBeautifyFilePath());

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
