#include "gtest/gtest.h"

#include "SimpleDownloadTest.h"
#include "StartStopDownloadTest.h"

TEST(LibtorrentWrapper, DISABLED_downloadTest)
{
  SimpleDownloadTest test;
  ASSERT_TRUE(test.start());

  ASSERT_LT(0, test.progressEventCount);
  ASSERT_LT(0, test.statusEventChangedCount);
  ASSERT_LT(0, test.listenPortChangedEventCount);
  ASSERT_LT(0, test.trackerFailedEventCount);
}

TEST(LibtorrentWrapper, DISABLED_startStopDownloadTest)
{
  StartStopDownloadTest test;
  ASSERT_TRUE(test.start());

  ASSERT_LT(0, test.progressEventCount);
  ASSERT_LT(0, test.statusEventChangedCount);
  ASSERT_LT(0, test.torrentResumeEventCount);
}