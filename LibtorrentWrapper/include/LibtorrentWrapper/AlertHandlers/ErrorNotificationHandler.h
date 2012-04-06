/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#ifndef _GGS_LIBTORRENT_ALERTHANDLERS_ERRORNOTIFICATIONHANDLER_H_
#define _GGS_LIBTORRENT_ALERTHANDLERS_ERRORNOTIFICATIONHANDLER_H_

#include <libtorrent/alert.hpp>
#include <libtorrent/alert_types.hpp>

#include <QtCore/QString>
#include <QtCore/QDebug>

namespace GGS 
{
  namespace Libtorrent
  {
    class WrapperInternal;
    namespace AlertHandlers 
    {
      struct ErrorNotificationHandler
      {
        WrapperInternal *wrapperInternal;
        void operator()(libtorrent::tracker_error_alert const& a) const;
        void operator()(libtorrent::file_error_alert const& a) const;
        void operator()(libtorrent::listen_failed_alert const& a) const;

        /// <summary>This alert is generated when a HTTP seed name lookup fails.</summary>
        /// <remarks>Ilya.Tkachenko, 06.04.2012.</remarks>
        void operator()(libtorrent::url_seed_alert const& a) const;

        /// <summary This alert is triggered if the tracker reply contains a warning field. 
        /// 				 Usually this means that the tracker announce was successful, but the tracker has a message to the client.
        /// 				 The msg string in the alert contains the warning message from the tracker.</summary>
        /// <remarks The msg string in the alert contains the warning message from the tracker.>Ilya.Tkachenko, 06.04.2012.</remarks>
        void operator()(libtorrent::tracker_warning_alert const& a) const
        {
          qDebug() << "ErrorNotificationHandler tracker_warning_alert: " << a.message().c_str();
        }


        /// <summary>If a scrape request fails, this alert is generated. 
        ///   This might be due to the tracker timing out, refusing connection or returning an http 
        ///   response code indicating an error. msg contains a message describing the error.</summary>
        /// <remarks>Ilya.Tkachenko, 06.04.2012.</remarks>
        void operator()(libtorrent::scrape_failed_alert const& a) const
        {
          qDebug() << "ErrorNotificationHandler scrape_failed_alert: " << a.message().c_str();
        }


        /// <summary>This alert is generated when a request to delete the files of a torrent fails.</summary>
        /// <remarks>Ilya.Tkachenko, 06.04.2012.</remarks>
        void operator()(libtorrent::torrent_delete_failed_alert const& a) const
        {
          qDebug() << "ErrorNotificationHandler torrent_delete_failed_alert: error_code " 
            << a.error << " msg " << a.message().c_str();
        }


        /// <summary>This alert is generated instead of save_resume_data_alert if there was an error 
        ///   generating the resume data. error describes what went wrong.</summary>
        /// <remarks>Ilya.Tkachenko, 06.04.2012.</remarks>
        void operator()(libtorrent::save_resume_data_failed_alert const& a) const
        {
          qDebug() << "ErrorNotificationHandler save_resume_data_failed_alert: error_code " 
            << a.error << " msg " << a.message().c_str();
        }

        /// <summary>This alert is generated when the metadata has been completely received and the info-hash 
        /// failed to match it. i.e. the metadata that was received was corrupt. libtorrent will automatically retry 
        /// to fetch it in this case. This is only relevant when running a torrent-less download, with the metadata 
        /// extension provided by libtorrent.</summary>
        /// <remarks>Ilya.Tkachenko, 06.04.2012.</remarks>
        void operator()(libtorrent::metadata_failed_alert const& a) const
        {
          qDebug() << "ErrorNotificationHandler metadata_failed_alert: " << a.message().c_str();
        }


        /// <summary>Не особо понятная ошибка но появляется чутка часто.</summary>
        /// <remarks>Ilya.Tkachenko, 06.04.2012.</remarks>
        void operator()(libtorrent::udp_error_alert const& a) const
        {
          qDebug() << "ErrorNotificationHandler udp_error_alert: " << a.message().c_str();
        }

        /// <summary>This alert is generated when a NAT router was successfully found but some part of the port mapping 
        /// request failed. It contains a text message that may help the user figure out what is wrong. 
        /// This alert is not generated in case it appears the client is not running on a NAT:ed network or 
        /// if it appears there is no NAT router that can be remote controlled to add port mappings.</summary>
        /// <remarks>Ilya.Tkachenko, 06.04.2012.</remarks>
        void operator()(libtorrent::portmap_error_alert const& a) const
        {
          qDebug() << "ErrorNotificationHandler portmap_error_alert: mapping" << a.message().c_str();
        }

        /// <summary>This alert is generated when a fastresume file has been passed to add_torrent but the files on disk did 
        /// 				 not match the fastresume file. The error_code explains the reason why the resume file was rejected.</summary>
        /// <remarks>Ilya.Tkachenko, 06.04.2012.</remarks>
        void operator()(libtorrent::fastresume_rejected_alert const& a) const
        {
          qDebug() << "ErrorNotificationHandler fastresume_rejected_alert: " << a.message().c_str();
        }
      };
    }
  }
}

#endif // _GGS_LIBTORRENT_ALERTHANDLERS_ERRORNOTIFICATIONHANDLER_H_