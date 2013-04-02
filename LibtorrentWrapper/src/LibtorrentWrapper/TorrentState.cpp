/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (�) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <LibtorrentWrapper/TorrentState>

namespace GGS {
  namespace Libtorrent {

    TorrentState::TorrentState() 
      : _backgroundSeeding(false)
      , _isStopping(false)
<<<<<<< HEAD
<<<<<<< HEAD
      , _rehashOnly(false)
      , _isSeedEnable(true)
<<<<<<< HEAD
=======
>>>>>>> 47ffffb... QGNA-398 Добавил в стайт торрент информацию об остановке и сигнал остановки будет посылаться только после вызова stop. В будущем лучше убрать эту функциональность в DownloadService.
=======
      , _rehashOnly(false)
>>>>>>> 037a373... QGNA-295 Добавил возможность получит infohash торпента. Добавил вариант старта торрента для рехешинга.
=======
>>>>>>> 72f0035... QGNA-295 Добавил параметр isSeeding
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

<<<<<<< HEAD
<<<<<<< HEAD
    bool TorrentState::isStopping() const
=======
    bool TorrentState::isStopping()
>>>>>>> 47ffffb... QGNA-398 Добавил в стайт торрент информацию об остановке и сигнал остановки будет посылаться только после вызова stop. В будущем лучше убрать эту функциональность в DownloadService.
=======
    bool TorrentState::isStopping() const
>>>>>>> 037a373... QGNA-295 Добавил возможность получит infohash торпента. Добавил вариант старта торрента для рехешинга.
    {
      return this->_isStopping;
    }

    void TorrentState::setIsStopping(bool value)
    {
      this->_isStopping = value;
    }

<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> 037a373... QGNA-295 Добавил возможность получит infohash торпента. Добавил вариант старта торрента для рехешинга.
    bool TorrentState::rehashOnly() const
    {
      return this->_rehashOnly;
    }

    void TorrentState::setRehashOnly(bool value)
    {
      this->_rehashOnly = value;
    }

<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> 72f0035... QGNA-295 Добавил параметр isSeeding
    void TorrentState::setIsSeedEnable(bool enable)
    {
      this->_isSeedEnable;
    }

    bool TorrentState::isSeedEnable() const
    {
      return this->_isSeedEnable;
    }

<<<<<<< HEAD
=======
>>>>>>> 47ffffb... QGNA-398 Добавил в стайт торрент информацию об остановке и сигнал остановки будет посылаться только после вызова stop. В будущем лучше убрать эту функциональность в DownloadService.
=======
>>>>>>> 037a373... QGNA-295 Добавил возможность получит infohash торпента. Добавил вариант старта торрента для рехешинга.
=======
>>>>>>> 72f0035... QGNA-295 Добавил параметр isSeeding
  }
}