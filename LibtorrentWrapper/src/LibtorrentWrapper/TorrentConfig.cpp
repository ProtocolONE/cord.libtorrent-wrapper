/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <LibtorrentWrapper/TorrentConfig>

namespace GGS {
  namespace Libtorrent
  {

    TorrentConfig::TorrentConfig()
      : _isReloadRequired(false)
      , _forceRehash(false)
    {
    }

    TorrentConfig::~TorrentConfig()
    {
    }

    void TorrentConfig::setPathToTorrentFile(const QString& pathToTorrentFile)
    {
      this->_pathToTorrentFile = pathToTorrentFile;
    }

    const QString& TorrentConfig::pathToTorrentFile() const
    {
      return this->_pathToTorrentFile;
    }

    void TorrentConfig::setDownloadPath(const QString& downloadPath)
    {
      this->_downloadPath = downloadPath;
    }

    const QString& TorrentConfig::downloadPath() const
    {
      return this->_downloadPath;
    }

    void TorrentConfig::setIsReloadRequired(bool isReloadRequired)
    {
      this->_isReloadRequired = isReloadRequired;
    }

    bool TorrentConfig::isReloadRequired() const
    {
      return this->_isReloadRequired;
    }

    void TorrentConfig::setIsForceRehash(bool isForceRehash)
    {
      this->_forceRehash = isForceRehash;
    }

    bool TorrentConfig::isForceRehash() const
    {
      return this->_forceRehash;
    }

  }
}