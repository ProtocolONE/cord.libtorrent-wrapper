/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <LibtorrentWrapper/libtorrentwrapper_global.h>
#include <LibtorrentWrapper/AlertHandlers/StorageNotificationHandler>
#include <LibtorrentWrapper/WrapperInternal>

#include <QtCore/QString>
#include <QtCore/QDebug>

namespace GGS {
  namespace Libtorrent {
    namespace AlertHandlers {
      void  StorageNotificationHandler::operator ()(libtorrent::save_resume_data_alert const& a) const
      {
        DEBUG_LOG << "StorageNotificationHandler:" << a.message().c_str();
        if (!a.resume_data)
          return;

        if (!a.handle.is_valid())
          return;

        this->wrapperInternal->saveFastResume(a.handle, a.resume_data);
      }

      void StorageNotificationHandler::operator()(libtorrent::read_piece_alert const& a) const
      {
        DEBUG_LOG << "read_piece_alert:" << a.message().c_str();
      }

      void StorageNotificationHandler::operator()(libtorrent::file_renamed_alert const& a) const
      {
        DEBUG_LOG << "file_renamed_alert:" << a.message().c_str();
      }

      void StorageNotificationHandler::operator()(libtorrent::file_rename_failed_alert const& a) const
      {
        DEBUG_LOG << "file_rename_failed_alert:" << a.message().c_str();
      }

      void StorageNotificationHandler::operator()(libtorrent::storage_moved_alert const& a) const
      {
        DEBUG_LOG << "storage_moved_alert:" << a.message().c_str();
      }

      void StorageNotificationHandler::operator()(libtorrent::storage_moved_failed_alert const& a) const
      {
        DEBUG_LOG << "storage_moved_failed_alert:" << a.message().c_str();
      }

      void StorageNotificationHandler::operator()(libtorrent::torrent_deleted_alert const& a) const
      {
        DEBUG_LOG << "torrent_deleted_alert: " << a.message().c_str();
      }

      void StorageNotificationHandler::operator()(libtorrent::cache_flushed_alert const& a) const
      {
        DEBUG_LOG << "cache_flushed_alert:" << a.message().c_str();
      }

    }
  }
}