/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <LibtorrentWrapper/AlertHandlers/ErrorNotificationHandler>
#include <LibtorrentWrapper/WrapperInternal>

namespace GGS {
  namespace Libtorrent {
    namespace AlertHandlers {
      void ErrorNotificationHandler::operator ()(libtorrent::tracker_error_alert const& a) const
      {
        // Ошибка трекера - возникает когда мы получили в ответ не 200
        // times_in_row - колчичество фейлов подряд - т.е. не надо считать ручками
        this->wrapperInternal->trackerErrorAlert(a.handle, a.times_in_row, a.status_code);
      }

      void ErrorNotificationHandler::operator()(libtorrent::file_error_alert const& a) const
      {
        QString filePath = QString::fromUtf8(a.file.c_str());
        int errorCode = a.error.value();
        this->wrapperInternal->fileErrorAlert(a.handle, filePath, errorCode);
      }

      void ErrorNotificationHandler::operator()(libtorrent::listen_failed_alert const& a) const
      {
        int errorCode = a.error.value();
        unsigned short port = a.endpoint.port();
        
        this->wrapperInternal->listenFailAlert(port, errorCode);
      }

      void ErrorNotificationHandler::operator()(libtorrent::url_seed_alert const& a) const
      {
        this->wrapperInternal->torrentUrlSeedAlert(a.handle, a.url);
      }

      void ErrorNotificationHandler::operator()(libtorrent::torrent_error_alert const& a) const
      {
        qDebug() << "ErrorNotificationHandler torrent_error_alert: " << a.message().c_str();
        this->wrapperInternal->torrentErrorAlert(a.handle);
      }

    }
  }
}