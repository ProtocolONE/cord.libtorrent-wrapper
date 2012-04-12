#include "StartStopDownloadTest.h"
#include "FileUtils.h"

using namespace GGS::Libtorrent;

StartStopDownloadTest::StartStopDownloadTest(QObject *parent)
  : QObject(parent)
{
  this->torrentResumeEventCount = 0;
  this->progressEventCount = 0;
  this->statusEventChangedCount = 0;
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
  this->_wrapper = new Wrapper();

  qDebug() << "wrapper connect progress " << connect(this->_wrapper, SIGNAL(progressChanged(ProgressEventArgs)), this, SLOT(torrentProgress(ProgressEventArgs)));
  qDebug() << "wrapper connect file error " << connect(this->_wrapper, SIGNAL(fileError(QString, QString, int)), this, SLOT(torrentFileError(QString, QString, int)));
  qDebug() << "wrapper connect status changed " << connect(this->_wrapper, SIGNAL(torrentStatusChanged(QString, ProgressEventArgs::TorrentStatus, ProgressEventArgs::TorrentStatus)), this, SLOT(torrentStatusChanged(QString, ProgressEventArgs::TorrentStatus, ProgressEventArgs::TorrentStatus)));
  qDebug() << "wrapper connect torrentDownloadFinished " << connect(this->_wrapper, SIGNAL(torrentDownloadFinished(QString)), this, SLOT(torrentDownloadFinished(QString)));
  qDebug() << "wrapper connect torrentResumed " << connect(this->_wrapper, SIGNAL(torrentResumed(QString)), this, SLOT(torrentResumed(QString)));

  QString torrentConfigPath = QCoreApplication::applicationDirPath();
  torrentConfigPath.append("/torrents");
  this->_wrapper->setTorrentConfigDirectoryPath(torrentConfigPath);

  this->_wrapper->setListeningPort(0);
  this->_wrapper->initEngine();

  // Set timeout for test;
  QTimer::singleShot(120000, this, SLOT(timeoutTick()));

  QString root = QCoreApplication::applicationDirPath();
  QString torrentPath = QString("%1/fixtures/bigclient.torrent").arg(root);
  QString downloadPath = QString("%1/work/").arg(root);

  FileUtils::removeDir(downloadPath);

  this->_torrentPath = torrentPath;
  this->_downloadPath = downloadPath;

  TorrentConfig config;
  config.setDownloadPath(downloadPath);
  config.setPathToTorrentFile(torrentPath);

  this->_wrapper->start(this->_torrentId, config);

  for (int i = 0; i < 10; i++) {
    QTimer::singleShot(1000, this, SLOT(singleStartSlot()));
  }

  for (int i = 0; i < 10; i++) {
    QTimer::singleShot(2000, this, SLOT(singleStopSlot()));
  }

  for (int i = 0; i < 10; i++) {
    QTimer::singleShot(3000, this, SLOT(singleStartSlot()));
    QTimer::singleShot(3000, this, SLOT(singleStopSlot()));
  }

  for (int i = 0; i < 10; i++) {
    QTimer::singleShot(4000, this, SLOT(singleReplaceSlot()));
  }

  this->_loop.exec();

  this->_wrapper->shutdown();
  delete this->_wrapper;
  return this->_result;
}

void StartStopDownloadTest::singleStartSlot()
{
  if (!this->_wrapper)
    return;

  TorrentConfig config;
  config.setDownloadPath(this->_downloadPath);
  config.setPathToTorrentFile(this->_torrentPath);
  this->_wrapper->start(this->_torrentId, config);
}

void StartStopDownloadTest::singleStopSlot()
{
  if (!this->_wrapper)
    return;

  this->_wrapper->stop(this->_torrentId);
}

void StartStopDownloadTest::singleReplaceSlot()
{
  if (!this->_wrapper)
    return;

  TorrentConfig config;
  config.setDownloadPath(this->_downloadPath);
  config.setPathToTorrentFile(this->_torrentPath);
  config.setIsReloadRequired(true);
  this->_wrapper->start(this->_torrentId, config);
}

void StartStopDownloadTest::torrentResumed( QString id )
{
  this->torrentResumeEventCount++;
}
