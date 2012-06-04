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
#include <LibtorrentWrapper/AlertHandlers/TrackerNotificationHandler.h>

#include <QtCore/QString>
#include <QtCore/QDebug>

void GGS::Libtorrent::AlertHandlers::TrackerNotificationHandler::operator()( libtorrent::tracker_announce_alert const& a ) const
{
  DEBUG_LOG << "TrackerNotificationHandler tracker_announce_alert: " << a.message().c_str();
}
  
void GGS::Libtorrent::AlertHandlers::TrackerNotificationHandler::operator()( libtorrent::scrape_reply_alert const& a ) const
{
  DEBUG_LOG << "TrackerNotificationHandler scrape_reply_alert: " << a.message().c_str();
}

void GGS::Libtorrent::AlertHandlers::TrackerNotificationHandler::operator()( libtorrent::tracker_reply_alert const& a ) const
{
  DEBUG_LOG << "TrackerNotificationHandler tracker_reply_alert: " << a.message().c_str();
}

void GGS::Libtorrent::AlertHandlers::TrackerNotificationHandler::operator()( libtorrent::dht_reply_alert const& a ) const
{
  DEBUG_LOG << "TrackerNotificationHandler dht_reply_alert: " << a.message().c_str();
}

