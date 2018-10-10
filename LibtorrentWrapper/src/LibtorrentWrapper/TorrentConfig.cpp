#include <LibtorrentWrapper/TorrentConfig.h>

namespace P1 {
  namespace Libtorrent {

    TorrentConfig::TorrentConfig()
      : _isReloadRequired(false)
      , _forceRehash(false)
      , _rehashOnly(false)
      , _isSeedEnable(true)

    {
    }

    TorrentConfig::~TorrentConfig()
    {
    }

    void TorrentConfig::setPathToTorrentFile(const QString& pathToTorrentFile)
    {
      this->_pathToTorrentFile = pathToTorrentFile;
    }

    const QString& TorrentConfig::pathToTorrentFile() const
    {
      return this->_pathToTorrentFile;
    }

    void TorrentConfig::setDownloadPath(const QString& downloadPath)
    {
      this->_downloadPath = downloadPath;
    }

    const QString& TorrentConfig::downloadPath() const
    {
      return this->_downloadPath;
    }

    void TorrentConfig::setIsReloadRequired(bool isReloadRequired)
    {
      this->_isReloadRequired = isReloadRequired;
    }

    bool TorrentConfig::isReloadRequired() const
    {
      return this->_isReloadRequired;
    }

    void TorrentConfig::setIsForceRehash(bool isForceRehash)
    {
      this->_forceRehash = isForceRehash;
    }

    bool TorrentConfig::isForceRehash() const
    {
      return this->_forceRehash;
    }

    bool TorrentConfig::rehashOnly() const
    {
      return this->_rehashOnly;
    }

    void TorrentConfig::setRehashOnly(bool value)
    {
      this->_rehashOnly = value;
    }

    void TorrentConfig::setIsSeedEnable(bool enable)
    {
      this->_isSeedEnable = enable;
    }

    bool TorrentConfig::isSeedEnable() const
    {
      return this->_isSeedEnable;
    }

  }
}