/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#ifndef _GGS_LIBTORRENT_ALERTHANDLERS_TRACKERNOTIFICATIONHANDLER_H_
#define _GGS_LIBTORRENT_ALERTHANDLERS_TRACKERNOTIFICATIONHANDLER_H_

#include <libtorrent/alert.hpp>
#include <libtorrent/alert_types.hpp>

#include <QtCore/QString>
#include <QtCore/QDebug>

namespace GGS {
  namespace Libtorrent
  {
    class WrapperInternal;
    namespace AlertHandlers {
      struct TrackerNotificationHandler
      {
        WrapperInternal *wrapperInternal;
        void operator()(libtorrent::tracker_announce_alert const& a) const
        {
          qDebug() << "TrackerNotificationHandler tracker_announce_alert: " << a.message().c_str();
        }

        void operator()(libtorrent::scrape_reply_alert const& a) const
        {
          qDebug() << "TrackerNotificationHandler scrape_reply_alert: " << a.message().c_str();
        }
        
        void operator()(libtorrent::tracker_reply_alert const& a) const
        {
          qDebug() << "TrackerNotificationHandler tracker_reply_alert: " << a.message().c_str();
        }
        
        void operator()(libtorrent::dht_reply_alert const& a) const
        {
          qDebug() << "TrackerNotificationHandler dht_reply_alert: " << a.message().c_str();
        }
      };
    }
  }
}

#endif // _GGS_LIBTORRENT_ALERTHANDLERS_TRACKERNOTIFICATIONHANDLER_H_