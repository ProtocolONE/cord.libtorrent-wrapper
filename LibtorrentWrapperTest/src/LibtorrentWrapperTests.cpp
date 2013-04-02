#include "gtest/gtest.h"

#include "SimpleDownloadTest.h"
#include "StartStopDownloadTest.h"
#include <LibtorrentWrapper/Wrapper.h>
<<<<<<< HEAD
<<<<<<< HEAD
#include <QtCore/QList>
=======
>>>>>>> 037a373... QGNA-295 Добавил возможность получит infohash торпента. Добавил вариант старта торрента для рехешинга.
=======
#include <QtCore/QList>
>>>>>>> 1b9b91e... QGNA-295 Добавил метод на получения списка файлов.

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

TEST(LibtorrentWrapper, GetInfoHashTest)
{
  GGS::Libtorrent::Wrapper wrapper;
  QString torrentPath = QCoreApplication::applicationDirPath();
  torrentPath.append("/fixtures/sampleclient.torrent");
  QString infoHash;
  ASSERT_TRUE(wrapper.getInfoHash(torrentPath, infoHash));
  ASSERT_EQ(0, infoHash.compare("1e9195379fdedf04eca6118aea1f6a2afb76798b"));
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> 1b9b91e... QGNA-295 Добавил метод на получения списка файлов.
}

TEST(LibtorrentWrapper, GetFileListTest)
{
  GGS::Libtorrent::Wrapper wrapper;
  QString torrentPath = QCoreApplication::applicationDirPath();
  torrentPath.append("/fixtures/sampleclient.torrent");
  QList<QString> fileList;
  
  ASSERT_TRUE(wrapper.getFileList(torrentPath, fileList));
  ASSERT_EQ(2, fileList.length());
  ASSERT_EQ(0, fileList[0].compare("text.txt"));
  ASSERT_EQ(0, fileList[1].compare("data.bin"));
<<<<<<< HEAD
=======
>>>>>>> 037a373... QGNA-295 Добавил возможность получит infohash торпента. Добавил вариант старта торрента для рехешинга.
=======
>>>>>>> 1b9b91e... QGNA-295 Добавил метод на получения списка файлов.
}