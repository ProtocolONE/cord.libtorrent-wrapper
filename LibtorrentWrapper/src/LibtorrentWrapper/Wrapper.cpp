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

    Wrapper::Wrapper(QObject *parent)
      : QObject(parent)
    {
      this->_internalWrapper = new WrapperInternal(this);
      connect(this->_internalWrapper, SIGNAL(progressChanged(GGS::Libtorrent::EventArgs::ProgressEventArgs)), this, SIGNAL(progressChanged(GGS::Libtorrent::EventArgs::ProgressEventArgs)));
      connect(this->_internalWrapper, SIGNAL(trackerFailed(QString, int, int)), this, SIGNAL(trackerFailed(QString, int, int)));
      connect(this->_internalWrapper, SIGNAL(fileError(QString, QString, int)), this, SIGNAL(fileError(QString, QString, int)));
      connect(this->_internalWrapper, SIGNAL(listenFailed(int, int)), this, SIGNAL(listenFailed(int, int)));
      connect(this->_internalWrapper, SIGNAL(torrentStatusChanged(QString, GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus, GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus)), 
        this, SIGNAL(torrentStatusChanged(QString, GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus, GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus)));

      connect(this->_internalWrapper, SIGNAL(torrentDownloadFinished(QString)), this, SIGNAL(torrentDownloadFinished(QString)));
      connect(this->_internalWrapper, SIGNAL(torrentResumed(QString)), this, SIGNAL(torrentResumed(QString)));
      connect(this->_internalWrapper, SIGNAL(listeningPortChanged(unsigned short)), this, SIGNAL(listeningPortChanged(unsigned short)));

      connect(this->_internalWrapper, SIGNAL(startTorrentFailed(QString, int)), this, SIGNAL(startTorrentFailed(QString,int)));
      connect(this->_internalWrapper, SIGNAL(torrentPaused(QString)), this, SIGNAL(torrentPaused(QString)));

      connect(this->_internalWrapper, SIGNAL(torrentError(QString)), this, SIGNAL(torrentError(QString)));
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

    int Wrapper::maxConnection()
    {
      return this->_internalWrapper->maxConnection();
    }

    void Wrapper::setMaxConnection(int maxConnection)
    {
      this->_internalWrapper->setMaxConnection(maxConnection);
    }

    void Wrapper::pauseSession()
    {
      this->_internalWrapper->pauseSession();
    }

    void Wrapper::resumeSession()
    {
      this->_internalWrapper->resumeSession();
    }

  }
}