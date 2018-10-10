#pragma once

#include <LibtorrentWrapper/libtorrentwrapper_global.h>

#include <QtCore/QObject>

namespace P1 {
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
