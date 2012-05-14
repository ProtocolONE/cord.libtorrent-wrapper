/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#ifndef _GGS_LIBTORRENT_TORRENT_CONFIG_H_
#define _GGS_LIBTORRENT_TORRENT_CONFIG_H_

#include <LibtorrentWrapper/libtorrentwrapper_global>

#include <QtCore/QObject>

namespace GGS {
  namespace Libtorrent
  {

    class LIBTORRENTWRAPPER_EXPORT TorrentConfig
    {
    public:
      TorrentConfig();
      ~TorrentConfig();

      void setPathToTorrentFile(const QString& pathToTorrentFile);
      const QString& pathToTorrentFile() const;
      
      void setDownloadPath(const QString& downloadPath);
      const QString& downloadPath() const;

      void setIsReloadRequired(bool isReloadRequired);
      bool isReloadRequired() const;

      void setIsForceRehash(bool isForceRehash);
      bool isForceRehash() const;

    private:
      QString _pathToTorrentFile;
      QString _downloadPath;
      bool _isReloadRequired;
      bool _forceRehash;
    };

  }
}

#endif // _GGS_LIBTORRENT_TORRENT_CONFIG_H_