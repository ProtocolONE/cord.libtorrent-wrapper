#ifndef _GGS_LIBTORRENT_WRAPPER_TEST_SIMPLE_DOWNLOAD_TEST_H_
#define _GGS_LIBTORRENT_WRAPPER_TEST_SIMPLE_DOWNLOAD_TEST_H_

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

class SimpleDownloadTest : public QObject
{
  Q_OBJECT
public:
  SimpleDownloadTest(QObject *parent = 0);
  ~SimpleDownloadTest();

  bool start();

  int progressEventCount;
  int statusEventChangedCount;
  int listenPortChangedEventCount;
  int trackerFailedEventCount;
public slots:
  void torrentProgress(GGS::Libtorrent::EventArgs::ProgressEventArgs args);
  void torrentFileError(QString id, QString path, int errorCode);
  void torrentStatusChanged(QString id, GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus oldState, GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus newState);
  void torrentDownloadFinished(QString id);
  void listeningPortChanged(unsigned short port);

  void trackerFailed(QString id, int failCountInARow, int httpStatusCode);

  void timeoutTick();

private:
  QString _torrentId;
  QEventLoop _loop;
  bool _result;
  bool _isResultFinished;
  QMutex _resultLock;

  void setResult(bool result);

  void setFail(QString reason);
};

#endif // _GGS_LIBTORRENT_WRAPPER_TEST_SIMPLE_DOWNLOAD_TEST_H_