#include "SimpleDownloadTest.h"
#include "FileUtils.h"

SimpleDownloadTest::SimpleDownloadTest(QObject *parent)
  : QObject(parent)
{
  this->_result = false;
  this->_isResultFinished = false;
  this->_torrentId = "someTorrentId";

  this->progressEventCount = 0;
  this->statusEventChangedCount = 0;
}

SimpleDownloadTest::~SimpleDownloadTest()
{
}

bool SimpleDownloadTest::start()
{
  using namespace GGS::Libtorrent;
  Wrapper wrapper;

  qDebug() << "wrapper connect progress " << connect(&wrapper, SIGNAL(progressChanged(ProgressEventArgs)), this, SLOT(torrentProgress(ProgressEventArgs)));
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
  QString torrentPath = QString("%1/fixtures/sampleclient.torrent").arg(root);
  QString downloadPath = QString("%1/work/").arg(root);

  FileUtils::removeDir(downloadPath);

  TorrentConfig config;
  config.setDownloadPath(downloadPath);
  config.setPathToTorrentFile(torrentPath);
    
  wrapper.start(this->_torrentId, config);

  this->_loop.exec();
  wrapper.shutdown();

  QString filePath1 = QString("%1%2").arg(downloadPath, "sampleclient/data.bin");
  QString filePath2 = QString("%1%2").arg(downloadPath, "sampleclient/text.txt");

  QFile f1(filePath1);
  if (!f1.open(QIODevice::ReadOnly))
    return false;
   
  QTextStream in1(&f1);
  QString str1 = in1.readAll();
  if (0 != str1.compare("123456"))
    return false;

  QFile f2(filePath2);
  if (!f2.open(QIODevice::ReadOnly))
    return false;

  QTextStream in2(&f2);
  QString str2 = in2.readAll();
  if (0 != str2.compare("hello world"))
    return false;

  return this->_result;
}

void SimpleDownloadTest::torrentProgress( ProgressEventArgs args )
{
  if (args.id() != this->_torrentId)
  {
    this->setFail("wrong torrent id in torrentProgress");
    return;
  }
  
  this->progressEventCount++;
}

void SimpleDownloadTest::torrentFileError( QString id, QString path, int errorCode )
{
  if (id != this->_torrentId)
  {
    this->setFail("wrong torrent id in torrentFileError");
    return;
  }

  QString err = QString("File error. Code: %1 Path: %2").arg(errorCode).arg(path);
  this->setFail(err);
}

void SimpleDownloadTest::torrentStatusChanged( QString id, ProgressEventArgs::TorrentStatus oldState, ProgressEventArgs::TorrentStatus newState )
{
  if (id != this->_torrentId)
  {
    this->setFail("wrong torrent id in torrentStatusChanged");
    return;
  }

  this->statusEventChangedCount++;
}

void SimpleDownloadTest::torrentDownloadFinished( QString id )
{
  if (id != this->_torrentId)
  {
    this->setFail("wrong torrent id in torrentDownloadFinished");
    return;
  }

  this->setResult(true);
}

void SimpleDownloadTest::timeoutTick()
{
  this->setResult(false);
}

void SimpleDownloadTest::setResult(bool result)
{
  QMutexLocker l(&this->_resultLock);
  if (!this->_isResultFinished) {
    this->_result = result;
    this->_isResultFinished = true;
  }

  this->_loop.quit();
}

void SimpleDownloadTest::setFail( QString reason )
{
  qDebug() << "fail : " << reason;
  this->setResult(false);
}
