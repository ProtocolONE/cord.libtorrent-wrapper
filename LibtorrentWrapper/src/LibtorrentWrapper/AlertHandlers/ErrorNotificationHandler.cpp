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
#include <LibtorrentWrapper/AlertHandlers/ErrorNotificationHandler>
#include <LibtorrentWrapper/WrapperInternal>

#include <QtCore/QDebug>
#include <QtCore/QString>

namespace GGS {
  namespace Libtorrent {
    namespace AlertHandlers {
      void ErrorNotificationHandler::operator ()(libtorrent::tracker_error_alert const& a) const
      {
        // Ошибка трекера - возникает когда мы получили в ответ не 200
        // times_in_row - колчичество фейлов подряд - т.е. не надо считать ручками
        CRITICAL_LOG << "listen_failed_alert: status " << a.status_code << " times in a row " << a.times_in_row << " message " << a.message().c_str();
        this->wrapperInternal->trackerErrorAlert(a.handle, a.times_in_row, a.status_code);
      }

      void ErrorNotificationHandler::operator()(libtorrent::file_error_alert const& a) const
      {
        QString filePath = QString::fromUtf8(a.file.c_str());
        int errorCode = a.error.value();
        CRITICAL_LOG << "file_error_alert: errorCode " << errorCode << " file " << filePath << " message " << a.message().c_str();
        this->wrapperInternal->fileErrorAlert(a.handle, filePath, errorCode);
      }

      void ErrorNotificationHandler::operator()(libtorrent::listen_failed_alert const& a) const
      {
        int errorCode = a.error.value();
        unsigned short port = a.endpoint.port();
        CRITICAL_LOG << "listen_failed_alert: errorCode " << errorCode << " port " << port << " message " << a.message().c_str();
        this->wrapperInternal->listenFailAlert(port, errorCode);
      }

      void ErrorNotificationHandler::operator()(libtorrent::url_seed_alert const& a) const
      {
        WARNING_LOG << "url_seed_alert: " << a.message().c_str();
        this->wrapperInternal->torrentUrlSeedAlert(a.handle, a.url);
      }

      void ErrorNotificationHandler::operator()(libtorrent::torrent_error_alert const& a) const
      {
        CRITICAL_LOG << "torrent_error_alert: " << a.message().c_str();
        this->wrapperInternal->torrentErrorAlert(a.handle);
      }

      void ErrorNotificationHandler::operator()( libtorrent::tracker_warning_alert const& a ) const
      {
        WARNING_LOG << "tracker_warning_alert: " << a.message().c_str();
      }

      void ErrorNotificationHandler::operator()( libtorrent::scrape_failed_alert const& a ) const
      {
        CRITICAL_LOG << "scrape_failed_alert: " << a.message().c_str();
      }

      void ErrorNotificationHandler::operator()( libtorrent::torrent_delete_failed_alert const& a ) const
      {
        WARNING_LOG << "torrent_delete_failed_alert: error_code " 
          << a.error << "msg" << a.message().c_str();
      }

      void ErrorNotificationHandler::operator()( libtorrent::save_resume_data_failed_alert const& a ) const
      {
        CRITICAL_LOG << "save_resume_data_failed_alert: error_code " 
          << a.error << " msg " << a.message().c_str();
      }

      void ErrorNotificationHandler::operator()( libtorrent::metadata_failed_alert const& a ) const
      {
        CRITICAL_LOG << "metadata_failed_alert: " << a.message().c_str();
      }

      void ErrorNotificationHandler::operator()( libtorrent::udp_error_alert const& a ) const
      {
        QString str = QString::fromLocal8Bit(a.message().c_str());
        //WARNING_LOG << "udp_error_alert: " << str;
      }

      void ErrorNotificationHandler::operator()( libtorrent::portmap_error_alert const& a ) const
      {
        CRITICAL_LOG << "portmap_error_alert: mapping" << a.message().c_str();
      }

      void ErrorNotificationHandler::operator()( libtorrent::fastresume_rejected_alert const& a ) const
      {
        WARNING_LOG << "fastresume_rejected_alert: " << a.message().c_str();
      }
    }
  }
}