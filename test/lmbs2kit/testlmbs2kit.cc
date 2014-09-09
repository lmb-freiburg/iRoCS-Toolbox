#include "lmbunit.hh"

static void testlmbs2kit()
{
  // ToDo: Write tests
}

int main()
{
  LMBUNIT_WRITE_HEADER();
  
  LMBUNIT_RUN_TEST(testlmbs2kit());

  LMBUNIT_WRITE_STATISTICS();
  
  return _nFails;
}

