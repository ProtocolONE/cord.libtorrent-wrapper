#include "Program.h"



Program::Program()
{
  qRegisterMetaType<GGS::Libtorrent::EventArgs::ProgressEventArgs>();

  this->setProgress1(0);
  this->setProgress2(0);

  QString root = QCoreApplication::applicationDirPath();
  QString qmlDir = root;
  qmlDir.append("/qml/main.qml");
  QUrl url(qmlDir);
  nQMLContainer = new QDeclarativeView();
  nQMLContainer->rootContext()->setContextProperty("mainWindow", this);
  nQMLContainer->setSource(url);

  nQMLContainer->setAlignment(Qt::AlignCenter);                                                                               
  nQMLContainer->setResizeMode(QDeclarativeView::SizeRootObjectToView);                                                       
  nQMLContainer->showNormal();

  qDebug() << "wrapper connect progressChanged " << connect(&this->_wrapper, SIGNAL(progressChanged(GGS::Libtorrent::EventArgs::ProgressEventArgs)), this, SLOT(torrentProgress(GGS::Libtorrent::EventArgs::ProgressEventArgs)));
  qDebug() << "wrapper connect file error " << connect(&this->_wrapper, SIGNAL(fileError(QString, QString, int)), this, SLOT(torrentFileError(QString, QString, int)));
  qDebug() << "wrapper connect status changed " << connect(&this->_wrapper, SIGNAL(torrentStatusChanged(QString, GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus, GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus)), this, SLOT(torrentStatusChanged(QString, GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus, GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus)));
  qDebug() << "wrapper connect torrentDownloadFinished " << connect(&this->_wrapper, SIGNAL(torrentDownloadFinished(QString)), this, SLOT(torrentDownloadFinished(QString)));
  qDebug() << "wrapper connect listeningPortChanged " << connect(&this->_wrapper, SIGNAL(listeningPortChanged(unsigned short)), this, SLOT(torrentListeningPortChanged(unsigned short)));
  
  qDebug() << "wrapper connect torrentPaused " << connect(&this->_wrapper, SIGNAL(torrentPaused(QString)), this, SLOT(torrentPaused(QString)));


  this->setPort("11999");
  QString torrentConfigPath = root;
  torrentConfigPath.append("/torrents");
  this->_wrapper.setTorrentConfigDirectoryPath(torrentConfigPath);
  this->_wrapper.setListeningPort(11999);
  this->_wrapper.initEngine();
}

Program::~Program()
{
}

void Program::start()
{
  qDebug() << "program started";
  this->_wrapper.initEngine();
  QString root = QCoreApplication::applicationDirPath();
  QString torrentPath = root;
  torrentPath.append("/300003010000000000.torrent");
  QString downloadPath = root;
  
  //downloadPath.append(QString::fromLocal8Bit("/игра_game"));
  downloadPath.append(QString::fromLocal8Bit("/game"));

  this->_config.setDownloadPath(downloadPath);
  this->_config.setPathToTorrentFile(torrentPath);
  QString id("300003010000000000");
  
  this->_wrapper.start(id, this->_config);
  //QTimer::singleShot(10000, this, SLOT(reloadSameTorrent()));

  //QTimer::singleShot(120000, this, SLOT(quit()));
}

void Program::reloadSameTorrent()
{
  qDebug() << "reloadSameTorrent";
  QString root = QCoreApplication::applicationDirPath();
  QString torrentPath = root;
  torrentPath.append("/300003010000000000.torrent");
  QString downloadPath = root;

  //downloadPath.append(QString::fromLocal8Bit("/игра_game"));
  downloadPath.append(QString::fromLocal8Bit("/game"));

  this->_config.setDownloadPath(downloadPath);
  this->_config.setPathToTorrentFile(torrentPath);
  this->_config.setIsReloadRequired(true);
  QString id("300003010000000000");

  this->_wrapper.start(id, this->_config);

}

void Program::torrentProgress( GGS::Libtorrent::EventArgs::ProgressEventArgs args )
{
  
  QString str = QString("id %1 progress %2 status %3 d_rate %4 u_rate %5 twd %6 tw %7").arg(args.id()).arg(args.progress()).arg(args.status()).arg(args.downloadRate()).arg(args.uploadRate()).arg(args.totalWantedDone()).arg(args.totalWanted());
  qDebug() << str;

  if(args.id() == "300002010000000000") {
    this->setProgress1(args.progress() * 100);
    this->setStatusText1(str);
  }

  if(args.id() == "300003010000000000") {
    this->setProgress2(args.progress() * 100);
    this->setStatusText2(str);
  }
}

void Program::beforeShutdown()
{
 this->_wrapper.shutdown();
}

void Program::quit()
{
  QCoreApplication::quit();
}

void Program::startTorrent(QString id)
{
  QString root = QCoreApplication::applicationDirPath();
  QString torrentPath = root;
  torrentPath.append("/");
  torrentPath.append(id);
  torrentPath.append(".torrent");
  QString downloadPath = root;

  //downloadPath.append(QString::fromLocal8Bit("/игра_game/"));
  downloadPath.append(QString::fromLocal8Bit("/game/"));
  downloadPath.append(id);
  downloadPath.append(QString::fromLocal8Bit("Игра"));
  downloadPath.append("/");

  this->_config.setDownloadPath(downloadPath);
  this->_config.setPathToTorrentFile(torrentPath);
  this->_wrapper.start(id, this->_config);
}

void Program::stopTorrent( QString id )
{
  qDebug() << "stopping" << id;
  this->_wrapper.stop(id);
}

void Program::restartTorrent( QString id )
{
  QString root = QCoreApplication::applicationDirPath();
  QString torrentPath = root;
  torrentPath.append("/");
  torrentPath.append(id);
  torrentPath.append(".torrent");
  QString downloadPath = root;

  //downloadPath.append(QString::fromLocal8Bit("/игра_game/"));
  downloadPath.append(QString::fromLocal8Bit("/game/"));
  downloadPath.append(id);
  downloadPath.append(QString::fromLocal8Bit("Игра"));
  downloadPath.append("/");

  this->_config.setDownloadPath(downloadPath);
  this->_config.setPathToTorrentFile(torrentPath);
  this->_config.setIsReloadRequired(true);
  this->_wrapper.start(id, this->_config);
}

void Program::torrentFileError( QString id, QString path, int errorCode )
{
  qDebug() << "file error " << id << " file " << path  << " error code " << errorCode;
  this->_wrapper.stop(id);
}

void Program::torrentStatusChanged( QString id, GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus oldState, GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus newState )
{
  qDebug() << "id " << id << " state change from " << oldState << " to " << newState;
}

void Program::torrentDownloadFinished( QString id )
{
  qDebug() << "download finished " << id;
}

void Program::torrentChangePort()
{
  qDebug() << "trying to change torrent port to " << this->_port;
  bool ok;
  unsigned short port = this->_port.toInt(&ok);
  if (!ok)
    return;

  this->_wrapper.changeListeningPort(port);
}

void Program::torrentListeningPortChanged( unsigned short newPort )
{
  QString portString = QString("%1").arg(newPort);
  this->setPort(portString);
}

void Program::setUploadRateLimit(QString bytesPerSecond)
{
  bool ok;
  int q = bytesPerSecond.toInt(&ok);
  if (ok)
    this->_wrapper.setUploadRateLimit(q);
}

void Program::setDownloadRateLimit(QString bytesPerSecond)
{
  bool ok;
  int q = bytesPerSecond.toInt(&ok);
  if (ok)
    this->_wrapper.setDownloadRateLimit(q);
}

void Program::torrentPaused(QString id)
{
  qDebug() << "stopped " << id;
}

