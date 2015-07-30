/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#ifndef _GGS_LIBTORRENT_WRAPPERINTERNAL_H_
#define _GGS_LIBTORRENT_WRAPPERINTERNAL_H_

#include <LibtorrentWrapper/TorrentConfig>
#include <LibtorrentWrapper/TorrentState>
#include <LibtorrentWrapper/ResumeInfo>
#include <LibtorrentWrapper/EventArgs/ProgressEventArgs>
#include <LibtorrentWrapper/AlertHandlers/ErrorNotificationHandler>
#include <LibtorrentWrapper/AlertHandlers/StatusNotificationHandler>
#include <LibtorrentWrapper/AlertHandlers/TrackerNotificationHandler>
#include <LibtorrentWrapper/AlertHandlers/StorageNotificationHandler>

#include <libtorrent/config.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/error_code.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/entry.hpp>
#include <libtorrent/lazy_entry.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/alert.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/extensions/ut_pex.hpp>
#include <libtorrent/extensions/smart_ban.hpp>
#include <libtorrent/peer_info.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QTimer>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QVariant>
#include <QtCore/QMetaObject>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTime>
#include <QtCore/QDebug>

namespace GGS {
  namespace Libtorrent {

    class WrapperInternal : public QObject
    {
      Q_OBJECT
    public:
      WrapperInternal(QObject *parent = 0);
      ~WrapperInternal();

      void initEngine();

      void loadSessionState();

      void start(const QString& id, TorrentConfig& config);
      void stop(const QString& id);
      void remove(const QString& id);
      
      void setTorrentConfigDirectoryPath(const QString& path) { this->_torrentConfigDirectoryPath = path; }
      void setListeningPort(unsigned short port) { this->_startupListeningPort = port; }
      void changeListeningPort(unsigned short port);
      unsigned short listeningPort() const;

      int maxConnection();
      void setMaxConnection(int maxConnection);

      void setUploadRateLimit(int bytesPerSecond);
      void setDownloadRateLimit(int bytesPerSecond);
      
      int uploadRateLimit() const;
      int downloadRateLimit() const;

      void shutdown();

      void saveFastResume(const libtorrent::torrent_handle &handle, boost::shared_ptr<libtorrent::entry> resumeData);

      QString getFastResumeFilePath(const QString& id);

      void torrentPausedAlert(const libtorrent::torrent_handle &handle);
      void trackerErrorAlert(const libtorrent::torrent_handle &handle, int failCountInARow, int httpStatusCode);
      void fileErrorAlert(const libtorrent::torrent_handle &handle, const QString& filePath, int errorCode);

      void torrentStatusChangedAlert(
        const libtorrent::torrent_handle &handle
        , libtorrent::torrent_status::state_t oldState
        , libtorrent::torrent_status::state_t newState);

      void listenFailAlert(int port, int errorCode);

      void torrentFinishedAlert(const libtorrent::torrent_handle &handle);
      void torrentResumedAlert(const libtorrent::torrent_handle &handle);

      void torrentUrlSeedAlert(const libtorrent::torrent_handle &handle, const std::string& url);
      void torrentErrorAlert(const libtorrent::torrent_handle &handle);

      void pauseSession();
      void resumeSession();

      bool seedEnabled() const;
      void setSeedEnabled(bool value);

      bool getInfoHash(const QString& path, QString& result);

      bool getFileList(const QString& path, QList<QString> &result);

    signals:
      void listeningPortChanged(unsigned short port);
      void progressChanged(GGS::Libtorrent::EventArgs::ProgressEventArgs args);

      void torrentStatusChanged(QString id, 
        GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus oldState, 
        GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus newState);

      void torrentDownloadFinished(QString id);
      void torrentResumed(QString id);
      void torrentPaused(QString id);

      void trackerFailed(QString id, int failCountInARow, int httpStatusCode);
      void torrentError(QString id);
      void fileError(QString id, QString filePath, int errorCode);
      void startTorrentFailed(QString id, int errorCode);

      void listenFailed(int port, int errorCode);
      void torrentRehashed(QString id, bool isComplete);

    private slots:
      void alertTimerTick();
      void progressTimerTick();
      void backgroundSeedStart();

    private:
      int loadFile(std::string const& filename, std::vector<char>& v, libtorrent::error_code& ec, int limit = 8000000);

      void createDirectoryIfNotExists(const QString &resumeFilePath );
      void loadAndStartTorrent(const QString& id, const TorrentConfig &config, bool backgroudSeeding = false);
      GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus convertStatus(const libtorrent::torrent_status::state_t status);
      inline TorrentState* getStateByTorrentHandle(const libtorrent::torrent_handle &handle);
      inline TorrentState* getStateById(const QString& id);

      void calcDirectSpeed(GGS::Libtorrent::EventArgs::ProgressEventArgs& args, const libtorrent::torrent_handle &handle);
      void emitTorrentProgress(const QString& id, const libtorrent::torrent_handle &handle);
      void emitTorrentProgress(
        const QString& id, 
        const libtorrent::torrent_handle& handle, 
        libtorrent::torrent_status &status, 
        libtorrent::torrent_status::state_t torrentState);

      void saveFastResumeWithoutLock(const libtorrent::torrent_handle &handle, boost::shared_ptr<libtorrent::entry> resumeData);
      void saveSessionState();
      void cleanIdToTorrentStateMap();

      QString getSessionStatePath();

      QString _torrentConfigDirectoryPath;
      int _fastresumeCounter;
      int _fastresumeCounterMax;
      int _fastResumeWaitTimeInSec;

      int _startupListeningPort;
      libtorrent::session *_session;
      libtorrent::session_settings _sessionsSettings;

      QTimer _alertTimer;
      QTimer _progressTimer;

      AlertHandlers::ErrorNotificationHandler _errorNotificationHandler;
      AlertHandlers::StatusNotificationHandler _statusNotificationHandler;
      AlertHandlers::TrackerNotificationHandler _trackerNotificationHandler;
      AlertHandlers::StorageNotificationHandler _storageNotificationHandler;

      QMutex _torrentsMapLock;
      QMap<QString, TorrentState*> _idToTorrentState;
      QMap<QString, TorrentState*> _infohashToTorrentState;

      QMap<QString, ResumeInfo> _resumeInfo;
      bool _seedEnabled;
      bool _shuttingDown;
      bool _initialized;
    
      qint64 _lastDirectDownloaded;
      qint64 _lastPeerDownloaded;
    };
  }
}

#endif // _GGS_LIBTORRENT_WRAPPERINTERNAL_H_