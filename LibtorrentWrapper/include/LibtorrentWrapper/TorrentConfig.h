/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#pragma once

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

      bool rehashOnly() const;
      void setRehashOnly(bool value);

      bool isSeedEnable() const;
      void setIsSeedEnable(bool enable);

    private:
      QString _pathToTorrentFile;
      QString _downloadPath;
      bool _isReloadRequired;
      bool _forceRehash;
      bool _rehashOnly;
      bool _isSeedEnable;
    };

  }
}
