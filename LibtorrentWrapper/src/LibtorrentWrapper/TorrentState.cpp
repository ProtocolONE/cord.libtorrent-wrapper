/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <LibtorrentWrapper/TorrentState>

namespace GGS {
  namespace Libtorrent {

    TorrentState::TorrentState() : _backgroundSeeding(false)
    {
    }

    TorrentState::~TorrentState()
    {
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

  }
}