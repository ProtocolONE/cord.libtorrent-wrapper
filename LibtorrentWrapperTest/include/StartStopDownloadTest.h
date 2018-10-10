#ifndef _GGS_LIBTORRENT_WRAPPER_TEST_STARTSTOP_DOWNLOAD_TEST_H_
#define _GGS_LIBTORRENT_WRAPPER_TEST_STARTSTOP_DOWNLOAD_TEST_H_

#include <LibtorrentWrapper/Wrapper>
#include <LibtorrentWrapper/TorrentConfig>
#include <LibtorrentWrapper/EventArgs/ProgressEventArgs>

#include <QtCore/QObject>
#include <QtCore/QDebug>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QCoreApplication>

#include <gtest/gtest.h>

using namespace GGS::Libtorrent::EventArgs;
class StartStopDownloadTest : public QObject
{
  Q_OBJECT
public:
  StartStopDownloadTest(QObject *parent = 0);
  ~StartStopDownloadTest();

  bool start();

  int progressEventCount;
  int statusEventChangedCount;
  int torrentResumeEventCount;

public slots:
  void torrentProgress(ProgressEventArgs args);
  void torrentFileError(QString id, QString path, int errorCode);
  void torrentStatusChanged(QString id, ProgressEventArgs::TorrentStatus oldState, ProgressEventArgs::TorrentStatus newState);
  void torrentDownloadFinished(QString id);
  void torrentResumed(QString id);
  void timeoutTick();

  void singleStartSlot();
  void singleStopSlot();
  void singleReplaceSlot();

  //void singleReplSlot();

private:
  QString _torrentId;
  QEventLoop _loop;
  bool _result;
  bool _isResultFinished;
  QMutex _resultLock;
  
  QString _downloadPath;
  QString _torrentPath;
  GGS::Libtorrent::Wrapper *_wrapper;

  void setResult(bool result);

  void setFail(QString reason);

};

#endif // _GGS_LIBTORRENT_WRAPPER_TEST_STARTSTOP_DOWNLOAD_TEST_H_