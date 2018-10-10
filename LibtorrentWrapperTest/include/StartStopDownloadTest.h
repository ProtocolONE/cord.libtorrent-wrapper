#pragma once

#include <LibtorrentWrapper/Wrapper.h>
#include <LibtorrentWrapper/TorrentConfig.h>
#include <LibtorrentWrapper/EventArgs/ProgressEventArgs.h>

#include <QtCore/QObject>
#include <QtCore/QDebug>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QCoreApplication>

#include <gtest/gtest.h>

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
  void torrentProgress(P1::Libtorrent::EventArgs::ProgressEventArgs args);
  void torrentFileError(QString id, QString path, int errorCode);
  void torrentStatusChanged(QString id, P1::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus oldState, P1::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus newState);
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
  P1::Libtorrent::Wrapper *_wrapper;

  void setResult(bool result);

  void setFail(QString reason);

};
