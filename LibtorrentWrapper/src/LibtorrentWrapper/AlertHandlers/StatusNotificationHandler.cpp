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
#include <LibtorrentWrapper/AlertHandlers/StatusNotificationHandler>
#include <LibtorrentWrapper/WrapperInternal>

#include <QtCore/QDebug>
#include <QtCore/QString>

namespace GGS {
  namespace Libtorrent {
    namespace AlertHandlers {
      void StatusNotificationHandler::operator ()(libtorrent::torrent_paused_alert const& a) const
      {
        if (!a.handle.is_valid())
          return;
        
        this->wrapperInternal->torrentPausedAlert(a.handle);
      }

      void StatusNotificationHandler::operator()(libtorrent::state_changed_alert const& a) const
      {
        if (!a.handle.is_valid())
          return;

        this->wrapperInternal->torrentStatusChangedAlert(a.handle, a.prev_state, a.state);
      }

      void StatusNotificationHandler::operator()(libtorrent::torrent_finished_alert const& a) const
      {
        if (!a.handle.is_valid())
          return;

        this->wrapperInternal->torrentFinishedAlert(a.handle);
      }

      void StatusNotificationHandler::operator()(libtorrent::torrent_resumed_alert const& a) const
      {
        if (!a.handle.is_valid())
          return;

        this->wrapperInternal->torrentResumedAlert(a.handle);
      }

      void StatusNotificationHandler::operator()( libtorrent::hash_failed_alert const& a ) const
      {
        // Не очень хочется выводить это в лог, много и не особо информативно
        //qDebug() << "StatusNotificationHandler hash_failed_alert: " << a.message().c_str();
      }

      void StatusNotificationHandler::operator()( libtorrent::torrent_checked_alert const& a ) const
      {
        DEBUG_LOG << "torrent_checked_alert: " << a.message().c_str();
      }

      void StatusNotificationHandler::operator()( libtorrent::metadata_received_alert const& a ) const
      {
        DEBUG_LOG << "metadata_received_alert: " << a.message().c_str();
      }

      void StatusNotificationHandler::operator()( libtorrent::external_ip_alert const& a ) const
      {
        DEBUG_LOG << "external_ip_alert: " << a.message().c_str();
      }

      void StatusNotificationHandler::operator()( libtorrent::listen_succeeded_alert const& a ) const
      {
        DEBUG_LOG << "listen_succeeded_alert: " << a.message().c_str();
      }

      void StatusNotificationHandler::operator()( libtorrent::torrent_added_alert const& a ) const
      {
        DEBUG_LOG << "add_torrent_alert: " << a.message().c_str();
      }

      void StatusNotificationHandler::operator()( libtorrent::trackerid_alert const& a ) const
      {
        DEBUG_LOG << "trackerid_alert: " << a.message().c_str();
      }

      void StatusNotificationHandler::operator()( libtorrent::torrent_removed_alert const& a ) const
      {
        DEBUG_LOG << "torrent_removed_alert: " << a.message().c_str();
      }

    }
  }
}