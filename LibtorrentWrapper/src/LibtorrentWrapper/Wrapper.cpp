#include <LibtorrentWrapper/Wrapper.h>
#include <LibtorrentWrapper/WrapperInternal.h>
#include <LibtorrentWrapper/SessionSettings.h>

namespace P1 {
  namespace Libtorrent
  {
    libtorrent::session_settings resolveFromProfile(Wrapper::Profile profile)
    {
      if (profile == P1::Libtorrent::Wrapper::HIGH_PERFORMANCE_SEED) {
        return highPerformanceSeed();
      }
      else if (profile == P1::Libtorrent::Wrapper::MIN_MEMORY_USAGE){
        return minMemoryUsage();
      }

      return defaultProfile();
    }

    Wrapper::Wrapper(QObject *parent)
      : QObject(parent)
    {
      this->_internalWrapper = new WrapperInternal(this);
      connect(this->_internalWrapper, SIGNAL(progressChanged(P1::Libtorrent::EventArgs::ProgressEventArgs)), this, SIGNAL(progressChanged(P1::Libtorrent::EventArgs::ProgressEventArgs)));
      connect(this->_internalWrapper, SIGNAL(trackerFailed(QString, int, int)), this, SIGNAL(trackerFailed(QString, int, int)));
      connect(this->_internalWrapper, SIGNAL(fileError(QString, QString, int)), this, SIGNAL(fileError(QString, QString, int)));
      connect(this->_internalWrapper, SIGNAL(listenFailed(int, int)), this, SIGNAL(listenFailed(int, int)));
      connect(this->_internalWrapper, SIGNAL(torrentStatusChanged(QString, P1::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus, P1::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus)), 
        this, SIGNAL(torrentStatusChanged(QString, P1::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus, P1::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus)));

      connect(this->_internalWrapper, SIGNAL(torrentDownloadFinished(QString)), this, SIGNAL(torrentDownloadFinished(QString)));
      connect(this->_internalWrapper, SIGNAL(torrentResumed(QString)), this, SIGNAL(torrentResumed(QString)));
      connect(this->_internalWrapper, SIGNAL(listeningPortChanged(unsigned short)), this, SIGNAL(listeningPortChanged(unsigned short)));

      connect(this->_internalWrapper, SIGNAL(startTorrentFailed(QString, int)), this, SIGNAL(startTorrentFailed(QString,int)));
      connect(this->_internalWrapper, SIGNAL(torrentPaused(QString)), this, SIGNAL(torrentPaused(QString)));

      connect(this->_internalWrapper, SIGNAL(torrentError(QString)), this, SIGNAL(torrentError(QString)));
      connect(this->_internalWrapper, SIGNAL(torrentRehashed(QString, bool)), this, SIGNAL(torrentRehashed(QString, bool)));
    }

    Wrapper::~Wrapper()
    {
      delete this->_internalWrapper;
    }
   
    void Wrapper::initEngine(Wrapper::Profile profile /*= DEFAULT_PROFILE*/)
    {
      this->_internalWrapper->initEngine(resolveFromProfile(profile));
    }

    void Wrapper::setProfile(Wrapper::Profile profile)
    {
      this->_internalWrapper->setProfile(resolveFromProfile(profile));
    }

    void Wrapper::start(const QString& id, TorrentConfig& config)
    {
      this->_internalWrapper->start(id, config);
    }

    void Wrapper::createFastResume(const QString& id, TorrentConfig& config)
    {
      this->_internalWrapper->createFastResume(id, config);
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

    QString Wrapper::getFastResumeFilePath(const QString& id)
    {
      return this->_internalWrapper->getFastResumeFilePath(id);
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

    bool Wrapper::seedEnabled() const
    {
      return this->_internalWrapper->seedEnabled();
    }

    void Wrapper::setSeedEnabled(bool value)
    {
      this->_internalWrapper->setSeedEnabled(value);
    }

    bool Wrapper::getInfoHash(const QString& path, QString& result)
    {
      return this->_internalWrapper->getInfoHash(path, result);
    }

    bool Wrapper::getFileList(const QString& path, QList<QString> &result)
    {
      return this->_internalWrapper->getFileList(path, result);
    }

    void Wrapper::setCredentials(const QString &userId, const QString &hash)
    {
      this->_internalWrapper->setCredentials(userId, hash);
    }

    void Wrapper::resetCredentials()
    {
      this->_internalWrapper->resetCredentials();
    }
  }
}