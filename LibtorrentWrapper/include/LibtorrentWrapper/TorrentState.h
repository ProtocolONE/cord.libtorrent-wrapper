/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#ifndef _GGS_LIBTORRENT_TORRENTSTATE_H_
#define _GGS_LIBTORRENT_TORRENTSTATE_H_

#include <libtorrent/torrent_handle.hpp>
#include <QtCore/QString>
namespace GGS {
  namespace Libtorrent
  {
    class TorrentState
    {
    public:
      TorrentState();
      ~TorrentState();

      void setHandle(libtorrent::torrent_handle handle) { this->_handle = handle; }
      const libtorrent::torrent_handle &handle() const { return this->_handle; }

      void setId(const QString& id) { this->_id = id; }
      const QString& id() const { return this->_id; }

    private:
      QString _id;
      libtorrent::torrent_handle _handle;
    };

  }
}

#endif //_GGS_LIBTORRENT_TORRENTSTATE_H_