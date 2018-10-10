#include <LibtorrentWrapper/libtorrentwrapper_global.h>
#include <LibtorrentWrapper/AlertHandlers/StorageNotificationHandler.h>
#include <LibtorrentWrapper/WrapperInternal.h>

#include <QtCore/QString>
#include <QtCore/QDebug>

namespace P1 {
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