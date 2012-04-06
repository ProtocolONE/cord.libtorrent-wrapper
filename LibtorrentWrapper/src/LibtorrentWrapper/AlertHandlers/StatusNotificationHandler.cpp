/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <LibtorrentWrapper/AlertHandlers/StatusNotificationHandler>
#include <LibtorrentWrapper/WrapperInternal>

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
    }
  }
}