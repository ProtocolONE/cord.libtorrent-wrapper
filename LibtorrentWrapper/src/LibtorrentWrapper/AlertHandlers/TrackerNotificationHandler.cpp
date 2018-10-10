#include <LibtorrentWrapper/libtorrentwrapper_global.h>
#include <LibtorrentWrapper/AlertHandlers/TrackerNotificationHandler.h>

#include <QtCore/QString>
#include <QtCore/QDebug>

void P1::Libtorrent::AlertHandlers::TrackerNotificationHandler::operator()( libtorrent::tracker_announce_alert const& a ) const
{
  DEBUG_LOG << "TrackerNotificationHandler tracker_announce_alert: " << a.message().c_str();
}
  
void P1::Libtorrent::AlertHandlers::TrackerNotificationHandler::operator()( libtorrent::scrape_reply_alert const& a ) const
{
  DEBUG_LOG << "TrackerNotificationHandler scrape_reply_alert: " << a.message().c_str();
}

void P1::Libtorrent::AlertHandlers::TrackerNotificationHandler::operator()( libtorrent::tracker_reply_alert const& a ) const
{
  DEBUG_LOG << "TrackerNotificationHandler tracker_reply_alert: " << a.message().c_str();
}

void P1::Libtorrent::AlertHandlers::TrackerNotificationHandler::operator()( libtorrent::dht_reply_alert const& a ) const
{
//  DEBUG_LOG << "TrackerNotificationHandler dht_reply_alert: " << a.message().c_str();
}

