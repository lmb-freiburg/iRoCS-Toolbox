#include "lmbunit.hh"

#include <libProgressReporter/ProgressReporterStream.hh>

static void testConstructor()
{
  iRoCS::ProgressReporter *pr = NULL;
  pr = new iRoCS::ProgressReporterStream(
      std::cout, 0, 0, 100, "Header message", "Progress message");
  LMBUNIT_ASSERT(pr != NULL);
  delete pr;
}

static void testProgressMinMax()
{
  iRoCS::ProgressReporter *pr = NULL;
  pr = new iRoCS::ProgressReporterStream(
      std::cout, 0, 10, 90, "Header message", "Progress message");
  LMBUNIT_ASSERT_EQUAL(pr->progressMin(), 10);
  LMBUNIT_ASSERT_EQUAL(pr->progressMax(), 90);
  pr->setProgressMin(0);
  LMBUNIT_ASSERT_EQUAL(pr->progressMin(), 0);
  pr->setProgressMax(100);
  LMBUNIT_ASSERT_EQUAL(pr->progressMax(), 100);
  delete pr;
}

static void testProgress()
{
  iRoCS::ProgressReporter *pr = NULL;
  pr = new iRoCS::ProgressReporterStream(
      std::cout, 0, 0, 100, "Header message", "Progress message");
  LMBUNIT_ASSERT_EQUAL(pr->progress(), 0);
  pr->setProgress(10);
  LMBUNIT_ASSERT_EQUAL(pr->progress(), 10);
  pr->updateProgress(20);
  LMBUNIT_ASSERT_EQUAL(pr->progress(), 20);
  delete pr;
}

static void testTaskProgress()
{
  iRoCS::ProgressReporter *pr = NULL;
  pr = new iRoCS::ProgressReporterStream(
      std::cout, 0, 0, 100, "Header message", "Progress message");
  LMBUNIT_ASSERT_EQUAL(pr->taskProgressMin(), 0);
  LMBUNIT_ASSERT_EQUAL(pr->taskProgressMax(), 0);
  pr->setTaskProgressMin(10);
  LMBUNIT_ASSERT_EQUAL(pr->taskProgressMin(), 10);
  pr->setTaskProgressMax(20);
  LMBUNIT_ASSERT_EQUAL(pr->taskProgressMax(), 20);
  pr->setTaskProgressRange(30, 50);
  LMBUNIT_ASSERT_EQUAL(pr->taskProgressMin(), 30);
  LMBUNIT_ASSERT_EQUAL(pr->taskProgressMax(), 50);
  delete pr;
}

static void testMessages()
{
  iRoCS::ProgressReporter *pr = NULL;
  pr = new iRoCS::ProgressReporterStream(
      std::cout, 0, 0, 100, "Header message", "Progress message");
  LMBUNIT_ASSERT_EQUAL(pr->headerMessage(), "Header message");
  LMBUNIT_ASSERT_EQUAL(pr->progressMessage(), "Progress message");
  pr->setHeaderMessage("New header message");
  LMBUNIT_ASSERT_EQUAL(pr->headerMessage(), "New header message");
  pr->setProgressMessage("New progress message");
  LMBUNIT_ASSERT_EQUAL(pr->progressMessage(), "New progress message");
  pr->updateProgressMessage("Updated progress message");
  LMBUNIT_ASSERT_EQUAL(pr->progressMessage(), "Updated progress message");
  delete pr;
}

static void testAbort()
{
  iRoCS::ProgressReporter *pr = NULL;
  pr = new iRoCS::ProgressReporterStream(
      std::cout, 0, 0, 100, "Header message", "Progress message");
  LMBUNIT_ASSERT(!pr->isAborted());
  pr->abort();
  LMBUNIT_ASSERT(pr->isAborted());
  LMBUNIT_ASSERT(!pr->updateProgress(10));
  LMBUNIT_ASSERT(!pr->updateProgressMessage("New progress message"));
  pr->setAborted(false);
  LMBUNIT_ASSERT(!pr->isAborted());
  pr->abortWithError("Error message");
  LMBUNIT_ASSERT(pr->isAborted());
  LMBUNIT_ASSERT(!pr->updateProgress(10));
  LMBUNIT_ASSERT(!pr->updateProgressMessage("New progress message"));
  delete pr;
}

int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST(testConstructor());
  LMBUNIT_RUN_TEST(testProgressMinMax());
  LMBUNIT_RUN_TEST(testProgress());
  LMBUNIT_RUN_TEST(testTaskProgress());
  LMBUNIT_RUN_TEST(testMessages());
  LMBUNIT_RUN_TEST(testAbort());

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
