#include "gtest/gtest.h"

#include "SimpleDownloadTest.h"

TEST(LibtorrentWrapper, downloadTest)
{
  SimpleDownloadTest test;
  ASSERT_TRUE(test.start());

  ASSERT_LE(0, test.progressEventCount);
  ASSERT_LE(0, test.statusEventChangedCount);
}