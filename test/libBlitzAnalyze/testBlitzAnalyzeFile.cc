#include "lmbunit.hh"

#include <libBlitzAnalyze/BlitzAnalyzeFile.hh>

static void testBlitzAnalyzeConstructor()
{
  try
  {
    BlitzAnalyzeFile file(TOP_BUILD_DIR "/testFile");
  }
  catch (BlitzAnalyzeError &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzAnalyzeError: ") + e.what());
  }
}

template<typename DataT>
static void testSaveArrayWithElemSize()
{
  try
  {
    BlitzAnalyzeFile outFile(TOP_BUILD_DIR "/testFile");

    blitz::Array<DataT,3> data(10, 10, 10);
    blitz::TinyVector<float,3> elSize(1.0, 0.5, 0.5);
    
    for (size_t i = 0; i < data.size(); ++i) data.data()[i] = DataT(i);

    outFile.saveArrayWithElemSize(data, elSize);
  }
  catch (BlitzAnalyzeError &e)
  {
    LMBUNIT_WRITE_FAILURE(std::string("Caught BlitzAnalyzeError: ") + e.what());
  }
  if (remove(TOP_BUILD_DIR "/testFile.hdr") != 0)
      LMBUNIT_WRITE_FAILURE(std::string("hdr file not written"));
  if (remove(TOP_BUILD_DIR "/testFile.img") != 0)
      LMBUNIT_WRITE_FAILURE(std::string("img file not written"));
}

int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST(testBlitzAnalyzeConstructor());
  LMBUNIT_RUN_TEST((testSaveArrayWithElemSize<unsigned char>()));
  LMBUNIT_RUN_TEST((testSaveArrayWithElemSize<short>()));
  LMBUNIT_RUN_TEST(
      (testSaveArrayWithElemSize< blitz::TinyVector<unsigned char,3> >()));

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
