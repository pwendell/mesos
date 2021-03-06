#include <gtest/gtest.h>

#include "common/foreach.hpp"
#include "common/hashset.hpp"
#include "common/utils.hpp"
#include "common/uuid.hpp"

namespace mesos {
namespace internal {
namespace utils {
namespace os {

static hashset<std::string> listfiles(const std::string& dir)
{
  hashset<std::string> fileset;
  foreach (const std::string& file, listdir(dir)) {
    fileset.insert(file);
  }
  return fileset;
}


TEST(UtilsTest, rmdir)
{
  // TODO(John Sirois): It would be good to use something like mkdtemp, but
  //abstract away a proper platform independent /tmp dir.
  std::string tmpdir = "/tmp/zks-" + UUID::random().toString();

  hashset<std::string> emptyListing;
  emptyListing.insert(".");
  emptyListing.insert("..");

  hashset<std::string> expectedListing;
  EXPECT_EQ(expectedListing, listfiles(tmpdir));

  mkdir(tmpdir + "/a/b/c");
  mkdir(tmpdir + "/a/b/d");
  mkdir(tmpdir + "/e/f");

  expectedListing = emptyListing;
  expectedListing.insert("a");
  expectedListing.insert("e");
  EXPECT_EQ(expectedListing, listfiles(tmpdir));

  expectedListing = emptyListing;
  expectedListing.insert("b");
  EXPECT_EQ(expectedListing, listfiles(tmpdir + "/a"));

  expectedListing = emptyListing;
  expectedListing.insert("c");
  expectedListing.insert("d");
  EXPECT_EQ(expectedListing, listfiles(tmpdir + "/a/b"));

  expectedListing = emptyListing;
  EXPECT_EQ(expectedListing, listfiles(tmpdir + "/a/b/c"));
  EXPECT_EQ(expectedListing, listfiles(tmpdir + "/a/b/d"));

  expectedListing.insert("f");
  EXPECT_EQ(expectedListing, listfiles(tmpdir + "/e"));

  expectedListing = emptyListing;
  EXPECT_EQ(expectedListing, listfiles(tmpdir + "/e/f"));

  rmdir(tmpdir);

  expectedListing.clear();
  EXPECT_EQ(expectedListing, listfiles(tmpdir));
}

} // namespace os
} // namespace utils
} // namespace internal
} // namespace mesos
