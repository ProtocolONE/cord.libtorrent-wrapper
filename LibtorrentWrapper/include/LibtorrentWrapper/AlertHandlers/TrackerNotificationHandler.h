#pragma once

#include <libtorrent/alert.hpp>
#include <libtorrent/alert_types.hpp>

namespace P1 {
  namespace Libtorrent
  {
    class WrapperInternal;
    namespace AlertHandlers {
      struct TrackerNotificationHandler
      {
        WrapperInternal *wrapperInternal;
        void operator()(libtorrent::tracker_announce_alert const& a) const;

        void operator()(libtorrent::scrape_reply_alert const& a) const;
        
        void operator()(libtorrent::tracker_reply_alert const& a) const;
        
        void operator()(libtorrent::dht_reply_alert const& a) const;
      };
    }
  }
}
