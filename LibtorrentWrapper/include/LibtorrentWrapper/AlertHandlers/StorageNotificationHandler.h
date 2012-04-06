/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (�) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#ifndef _GGS_LIBTORRENT_ALERTHANDLERS_STORAGENOTIFICATIONHANDLER_H_
#define _GGS_LIBTORRENT_ALERTHANDLERS_STORAGENOTIFICATIONHANDLER_H_

#include <libtorrent/alert.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/entry.hpp>

#include <QtCore/QString>
#include <QtCore/QDebug>

namespace GGS {
  namespace Libtorrent
  {
    class WrapperInternal;
    namespace AlertHandlers {
      struct StorageNotificationHandler
      {
        WrapperInternal *wrapperInternal;

        void operator()(libtorrent::save_resume_data_alert const& a) const;

        void operator()(libtorrent::read_piece_alert const& a) const
        {
          qDebug() << "StorageNotificationHandler read_piece_alert: " << a.message().c_str();
        }

        void operator()(libtorrent::file_renamed_alert const& a) const
        {
          qDebug() << "StorageNotificationHandler file_renamed_alert: " << a.message().c_str();
        }
        void operator()(libtorrent::file_rename_failed_alert const& a) const
        {
          qDebug() << "StorageNotificationHandler file_rename_failed_alert: " << a.message().c_str();
        }

        void operator()(libtorrent::storage_moved_alert const& a) const
        {
          qDebug() << "StorageNotificationHandler storage_moved_alert: " << a.message().c_str();
        }

        void operator()(libtorrent::storage_moved_failed_alert const& a) const
        {
          qDebug() << "StorageNotificationHandler storage_moved_failed_alert: " << a.message().c_str();
        }

        void operator()(libtorrent::torrent_deleted_alert const& a) const
        {
          qDebug() << "StorageNotificationHandler torrent_deleted_alert: " << a.message().c_str();
        }

        void operator()(libtorrent::cache_flushed_alert const& a) const
        {
          qDebug() << "StorageNotificationHandler cache_flushed_alert: " << a.message().c_str();
        }
      };
    }
  }
}

#endif // _GGS_LIBTORRENT_ALERTHANDLERS_STORAGENOTIFICATIONHANDLER_H_