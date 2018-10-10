#pragma once

#include <libtorrent/alert.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/entry.hpp>

namespace P1 {
  namespace Libtorrent
  {
    class WrapperInternal;
    namespace AlertHandlers {
      struct StorageNotificationHandler
      {
        WrapperInternal *wrapperInternal;

        void operator()(libtorrent::save_resume_data_alert const& a) const;

        void operator()(libtorrent::read_piece_alert const& a) const;

        void operator()(libtorrent::file_renamed_alert const& a) const;

        void operator()(libtorrent::file_rename_failed_alert const& a) const;

        void operator()(libtorrent::storage_moved_alert const& a) const;

        void operator()(libtorrent::storage_moved_failed_alert const& a) const;

        void operator()(libtorrent::torrent_deleted_alert const& a) const;

        void operator()(libtorrent::cache_flushed_alert const& a) const;
      };
    }
  }
}
