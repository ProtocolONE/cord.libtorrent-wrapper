#include <LibtorrentWrapper/TorrentState.h>

namespace P1 {
  namespace Libtorrent {

    TorrentState::TorrentState() 
      : _backgroundSeeding(false)
      , _isStopping(false)
      , _rehashOnly(false)
      , _isSeedEnable(true)
    {
    }

    TorrentState::~TorrentState()
    {
    }

    QString TorrentState::pathToTorrent() const
    {
      return this->_pathToTorrent;
    }

    void TorrentState::setPathToTorrent(const QString& path)
    {
      this->_pathToTorrent = path;
    }

    void TorrentState::setHandle(libtorrent::torrent_handle handle)
    {
      this->_handle = handle;
    }

    const libtorrent::torrent_handle& TorrentState::handle() const
    {
      return this->_handle;
    }

    void TorrentState::setId(const QString& id)
    {
      this->_id = id;
    }

    const QString& TorrentState::id() const
    {
      return this->_id;
    }

    bool TorrentState::backgroundSeeding() const
    {
      return this->_backgroundSeeding;
    }

    void TorrentState::setBackgroundSeeding(bool value)
    {
      this->_backgroundSeeding = value;
    }

    bool TorrentState::isStopping() const
    {
      return this->_isStopping;
    }

    void TorrentState::setIsStopping(bool value)
    {
      this->_isStopping = value;
    }

    bool TorrentState::rehashOnly() const
    {
      return this->_rehashOnly;
    }

    void TorrentState::setRehashOnly(bool value)
    {
      this->_rehashOnly = value;
    }

    void TorrentState::setIsSeedEnable(bool enable)
    {
      this->_isSeedEnable;
    }

    bool TorrentState::isSeedEnable() const
    {
      return this->_isSeedEnable;
    }

  }
}