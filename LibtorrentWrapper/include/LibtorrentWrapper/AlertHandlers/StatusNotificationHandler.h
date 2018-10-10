#pragma once

#include <libtorrent/alert.hpp>
#include <libtorrent/alert_types.hpp>

namespace P1 {
  namespace Libtorrent
  {
    class WrapperInternal;
    namespace AlertHandlers {
      struct StatusNotificationHandler
      {
        WrapperInternal *wrapperInternal;

        void operator()(libtorrent::torrent_paused_alert const& a) const;
        void operator()(libtorrent::state_changed_alert const& a) const;
        void operator()(libtorrent::torrent_finished_alert const& a) const;
        void operator()(libtorrent::torrent_resumed_alert const& a) const;

        void operator()(libtorrent::hash_failed_alert const& a) const;

        void operator()(libtorrent::torrent_checked_alert const& a) const;

        void operator()(libtorrent::metadata_received_alert const& a) const;

        void operator()(libtorrent::external_ip_alert const& a) const;

        void operator()(libtorrent::listen_succeeded_alert const& a) const;

        void operator()(libtorrent::torrent_added_alert const& a) const;

        void operator()(libtorrent::trackerid_alert const& a) const;

        void operator()(libtorrent::torrent_removed_alert const& a) const;
      };
    }
  }
}
