/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/
#pragma once

#include <libtorrent/torrent_handle.hpp>
#include <QtCore/QString>

namespace GGS {
  namespace Libtorrent {

    class TorrentState
    {
    public:
      TorrentState();
      ~TorrentState();

      void setHandle(libtorrent::torrent_handle handle);
      const libtorrent::torrent_handle &handle() const;

      void setId(const QString& id);
      const QString& id() const;

      bool backgroundSeeding() const;
      void setBackgroundSeeding(bool value);

      bool isStopping() const;
      void setIsStopping(bool value);

      bool rehashOnly() const;
      void setRehashOnly(bool value);

      bool isSeedEnable() const;
      void setIsSeedEnable(bool enable);

      QString pathToTorrent() const;
      void setPathToTorrent(const QString& path);

    private:
      QString _id;
      QString _pathToTorrent;
      libtorrent::torrent_handle _handle;
      bool _backgroundSeeding;
      bool _isStopping;
      bool _rehashOnly;
      bool _isSeedEnable;
    };

  }
}
