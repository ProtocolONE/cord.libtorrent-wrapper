/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <LibtorrentWrapper/AlertHandlers/StorageNotificationHandler>
#include <LibtorrentWrapper/WrapperInternal>

namespace GGS {
  namespace Libtorrent {
    namespace AlertHandlers {
      void  StorageNotificationHandler::operator ()(libtorrent::save_resume_data_alert const& a) const
      {
        qDebug() << "StorageNotificationHandler: " << a.message().c_str();
        if (!a.resume_data)
          return;

        if (!a.handle.is_valid())
          return;

        this->wrapperInternal->saveFastResume(a.handle, a.resume_data);
      }
    }
  }
}