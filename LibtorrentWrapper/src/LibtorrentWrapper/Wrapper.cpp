/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <LibtorrentWrapper/Wrapper>
#include <LibtorrentWrapper/WrapperInternal>

namespace GGS {
  namespace Libtorrent
  {

    Wrapper::Wrapper()
    {
      this->_internalWrapper = new WrapperInternal(this);
      qDebug() << "connect progress " <<    connect(this->_internalWrapper, SIGNAL(progressChanged(ProgressEventArgs)), this, SIGNAL(progressChanged(ProgressEventArgs)));
      qDebug() << "connect trackerFailed " << connect(this->_internalWrapper, SIGNAL(trackerFailed(QString, int, int)), this, SIGNAL(trackerFailed(QString, int, int)));
      qDebug() << "connect fileFail  " <<   connect(this->_internalWrapper, SIGNAL(fileError(QString, QString, int)), this, SIGNAL(fileError(QString, QString, int)));
      qDebug() << "connect listenFail  " << connect(this->_internalWrapper, SIGNAL(listenFailed(int, int)), this, SIGNAL(listenFailed(int, int)));
      qDebug() << "connect torrentStatusChanged  " << 
        connect(this->_internalWrapper, 
        SIGNAL(torrentStatusChanged(QString, ProgressEventArgs::TorrentStatus, ProgressEventArgs::TorrentStatus)), 
        this, 
        SIGNAL(torrentStatusChanged(QString, ProgressEventArgs::TorrentStatus, ProgressEventArgs::TorrentStatus)));

      qDebug() << "connect torrentFinished  " << connect(this->_internalWrapper, SIGNAL(torrentDownloadFinished(QString)), this, SIGNAL(torrentDownloadFinished(QString)));
      qDebug() << "connect torrentResumed  " <<  connect(this->_internalWrapper, SIGNAL(torrentResumed(QString)), this, SIGNAL(torrentResumed(QString)));
      qDebug() << "connect listeningPortChanged  " <<  connect(this->_internalWrapper, SIGNAL(listeningPortChanged(unsigned short)), this, SIGNAL(listeningPortChanged(unsigned short)));
    }

    Wrapper::~Wrapper()
    {
      delete this->_internalWrapper;
    }

    void Wrapper::initEngine()
    {
      this->_internalWrapper->initEngine();
    }

    void Wrapper::start(const QString& id, TorrentConfig& config)
    {
      this->_internalWrapper->start(id, config);
    }

    void Wrapper::stop(const QString& id)
    {
      this->_internalWrapper->stop(id);
    }

    void Wrapper::remove(const QString& id)
    {
      this->_internalWrapper->remove(id);
    }

    void Wrapper::shutdown()
    {
      this->_internalWrapper->shutdown();
    }

    void Wrapper::setListeningPort(unsigned short port)
    {
      this->_internalWrapper->setListeningPort(port);
    }

    void Wrapper::changeListeningPort(unsigned short port)
    {
      this->_internalWrapper->changeListeningPort(port);
    }

    unsigned short Wrapper::listeningPort() const
    {
      return this->_internalWrapper->listeningPort();
    }

    void Wrapper::setUploadRateLimit( int bytesPerSecond )
    {
      this->_internalWrapper->setUploadRateLimit(bytesPerSecond);
    }

    void Wrapper::setDownloadRateLimit( int bytesPerSecond )
    {
      this->_internalWrapper->setDownloadRateLimit(bytesPerSecond);
    }

    int Wrapper::uploadRateLimit() const
    {
      return this->_internalWrapper->uploadRateLimit();
    }

    int Wrapper::downloadRateLimit() const
    {
      return this->_internalWrapper->downloadRateLimit();
    }

    void Wrapper::setTorrentConfigDirectoryPath(const QString& path)
    {
      this->_internalWrapper->setTorrentConfigDirectoryPath(path);
    }

  }
}