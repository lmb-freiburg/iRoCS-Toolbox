#include "lmbunit.hh"

#include <libBaseFunctions/BaseEnvironment.hh>

static void testHomeDirectory()
{
  LMBUNIT_ASSERT(
      BaseEnvironment::UserHomeDirectory() == std::string(getenv("HOME")));
}

static void testTempDirectory()
{
  BaseEnvironment::UserTempDirectory();
}

static void testUserAppDataDirectory()
{
  BaseEnvironment::UserAppDataDirectory();
}

static void testHostName()
{
  BaseEnvironment::HostName();
}

int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST(testHomeDirectory());
  LMBUNIT_RUN_TEST(testTempDirectory());
  LMBUNIT_RUN_TEST(testUserAppDataDirectory());
  LMBUNIT_RUN_TEST(testHostName());

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
