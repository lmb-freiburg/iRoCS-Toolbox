#include "lmbunit.hh"

#include <libProgressReporter/ProgressReporterQt4.hh>

static void testConstructor()
{
  iRoCS::ProgressReporter *pr = NULL;
  pr = new iRoCS::ProgressReporterQt4();
  LMBUNIT_ASSERT(pr != NULL);
  delete pr;
}

int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST(testConstructor());

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
