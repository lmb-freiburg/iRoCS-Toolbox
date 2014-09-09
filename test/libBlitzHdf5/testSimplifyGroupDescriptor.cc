#include "lmbunit.hh"

#include <libBlitzHdf5/BlitzHdf5Light.hh>

static void testSimplifyEmptyGroupDescriptor()
{
  LMBUNIT_ASSERT_EQUAL(BlitzH5File::simplifyGroupDescriptor(""), "");
}

static void testSimplifyGroupDescriptorEndingWithSlash()
{
  LMBUNIT_ASSERT_EQUAL(
      BlitzH5File::simplifyGroupDescriptor("/this/is/a/group/"),
      "/this/is/a/group");
}

static void testSimplifyGroupDescriptorRelative()
{
  LMBUNIT_ASSERT_EQUAL(
      BlitzH5File::simplifyGroupDescriptor("this/is/a/group/"),
      "this/is/a/group");
}

static void testSimplifyGroupDescriptorEndingNoSlash()
{
  LMBUNIT_ASSERT_EQUAL(
      BlitzH5File::simplifyGroupDescriptor("/this/is/a/group"),
      "/this/is/a/group");
}

static void testSimplifyGroupDescriptorWeird()
{
  LMBUNIT_ASSERT_EQUAL(
      BlitzH5File::simplifyGroupDescriptor("/this/./is/a////group"),
      "/this/is/a/group");
}

int main(int, char**)
{
  LMBUNIT_WRITE_HEADER();

  LMBUNIT_RUN_TEST(testSimplifyEmptyGroupDescriptor());
  LMBUNIT_RUN_TEST(testSimplifyGroupDescriptorEndingWithSlash());
  LMBUNIT_RUN_TEST(testSimplifyGroupDescriptorRelative());
  LMBUNIT_RUN_TEST(testSimplifyGroupDescriptorEndingNoSlash());
  LMBUNIT_RUN_TEST(testSimplifyGroupDescriptorWeird());

  LMBUNIT_WRITE_STATISTICS();
  return _nFails;
}
