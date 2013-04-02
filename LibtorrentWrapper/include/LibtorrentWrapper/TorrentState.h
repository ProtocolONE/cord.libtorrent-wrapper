/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (�) 2011 - 2012, Syncopate Limited and/or affiliates.
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

<<<<<<< HEAD
<<<<<<< HEAD
      bool isStopping() const;
      void setIsStopping(bool value);

      bool rehashOnly() const;
      void setRehashOnly(bool value);

      bool isSeedEnable() const;
      void setIsSeedEnable(bool enable);

<<<<<<< HEAD
<<<<<<< HEAD
      QString pathToTorrent() const;
      void setPathToTorrent(const QString& path);

=======
      bool isStopping();
      void setIsStopping(bool value);

>>>>>>> 47ffffb... QGNA-398 Добавил в стайт торрент информацию об остановке и сигнал остановки будет посылаться только после вызова stop. В будущем лучше убрать эту функциональность в DownloadService.
=======
      bool isStopping() const;
      void setIsStopping(bool value);

      bool rehashOnly() const;
      void setRehashOnly(bool value);

>>>>>>> 037a373... QGNA-295 Добавил возможность получит infohash торпента. Добавил вариант старта торрента для рехешинга.
=======
>>>>>>> 72f0035... QGNA-295 Добавил параметр isSeeding
=======
      QString pathToTorrent() const;
      void setPathToTorrent(const QString& path);

>>>>>>> 78744f6... QGNA-295 Запоминаем путь для torrent файла
    private:
      QString _id;
      QString _pathToTorrent;
      libtorrent::torrent_handle _handle;
      bool _backgroundSeeding;
      bool _isStopping;
<<<<<<< HEAD
<<<<<<< HEAD
      bool _rehashOnly;
      bool _isSeedEnable;
=======
>>>>>>> 47ffffb... QGNA-398 Добавил в стайт торрент информацию об остановке и сигнал остановки будет посылаться только после вызова stop. В будущем лучше убрать эту функциональность в DownloadService.
=======
      bool _rehashOnly;
<<<<<<< HEAD

>>>>>>> 037a373... QGNA-295 Добавил возможность получит infohash торпента. Добавил вариант старта торрента для рехешинга.
=======
      bool _isSeedEnable;
>>>>>>> 72f0035... QGNA-295 Добавил параметр isSeeding
    };

  }
}
