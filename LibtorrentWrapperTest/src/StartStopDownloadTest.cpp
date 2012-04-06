#include "StartStopDownloadTest.h"
#include "FileUtils.h"

StartStopDownloadTest::StartStopDownloadTest(QObject *parent)
  : QObject(parent)
{
}

StartStopDownloadTest::~StartStopDownloadTest()
{
}

void StartStopDownloadTest::torrentProgress(ProgressEventArgs args)
{
  if (args.id() != this->_torrentId)
  {
    this->setFail("wrong torrent id in torrentProgress");
    return;
  }

  this->progressEventCount++;
}

void StartStopDownloadTest::torrentFileError(QString id, QString path, int errorCode)
{
  if (id != this->_torrentId)
  {
    this->setFail("wrong torrent id in torrentFileError");
    return;
  }

  QString err = QString("File error. Code: %1 Path: %2").arg(errorCode).arg(path);
  this->setFail(err);
}

void StartStopDownloadTest::torrentStatusChanged(QString id, ProgressEventArgs::TorrentStatus oldState, ProgressEventArgs::TorrentStatus newState)
{
  if (id != this->_torrentId)
  {
    this->setFail("wrong torrent id in torrentStatusChanged");
    return;
  }

  this->statusEventChangedCount++;
}

void StartStopDownloadTest::torrentDownloadFinished(QString id)
{
  if (id != this->_torrentId)
  {
    this->setFail("wrong torrent id in torrentDownloadFinished");
    return;
  }

  this->setResult(true);
}

void StartStopDownloadTest::timeoutTick()
{
  this->setResult(false);
}

void StartStopDownloadTest::setResult(bool result)
{
  QMutexLocker l(&this->_resultLock);
  if (!this->_isResultFinished) {
    this->_result = result;
    this->_isResultFinished = true;
  }

  this->_loop.quit();
}

void StartStopDownloadTest::setFail(QString reason)
{
  qDebug() << "fail : " << reason;
  this->setResult(false);
}

bool StartStopDownloadTest::start()
{
  using namespace GGS::Libtorrent;
  Wrapper wrapper;

  qDebug() << "wrapper connect progress " << connect(&wrapper, SIGNAL(progress(ProgressEventArgs)), this, SLOT(torrentProgress(ProgressEventArgs)));
  qDebug() << "wrapper connect file error " << connect(&wrapper, SIGNAL(fileError(QString, QString, int)), this, SLOT(torrentFileError(QString, QString, int)));
  qDebug() << "wrapper connect status changed " << connect(&wrapper, SIGNAL(torrentStatusChanged(QString, ProgressEventArgs::TorrentStatus, ProgressEventArgs::TorrentStatus)), this, SLOT(torrentStatusChanged(QString, ProgressEventArgs::TorrentStatus, ProgressEventArgs::TorrentStatus)));
  qDebug() << "wrapper connect torrentDownloadFinished " << connect(&wrapper, SIGNAL(torrentDownloadFinished(QString)), this, SLOT(torrentDownloadFinished(QString)));

  QString torrentConfigPath = QCoreApplication::applicationDirPath();
  torrentConfigPath.append("/torrents");
  wrapper.setTorrentConfigDirectoryPath(torrentConfigPath);

  wrapper.setListeningPort(0);
  wrapper.initEngine();

  // Set timeout for test;
  QTimer::singleShot(30000, this, SLOT(timeoutTick()));

  QString root = QCoreApplication::applicationDirPath();
  QString torrentPath = QString("%1/fixtures/bigclient.torrent").arg(root);
  QString downloadPath = QString("%1/work/").arg(root);

  FileUtils::removeDir(downloadPath);

  TorrentConfig config;
  config.setDownloadPath(downloadPath);
  config.setPathToTorrentFile(torrentPath);
  return true;
}
