#pragma once

#include <LibtorrentWrapper/TorrentConfig.h>
#include <LibtorrentWrapper/TorrentState.h>
#include <LibtorrentWrapper/ResumeInfo.h>
#include <LibtorrentWrapper/EventArgs/ProgressEventArgs.h>
#include <LibtorrentWrapper/AlertHandlers/ErrorNotificationHandler.h>
#include <LibtorrentWrapper/AlertHandlers/StatusNotificationHandler.h>
#include <LibtorrentWrapper/AlertHandlers/TrackerNotificationHandler.h>
#include <LibtorrentWrapper/AlertHandlers/StorageNotificationHandler.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QTimer>
#include <QtCore/QMutex>

namespace P1 {
  namespace Libtorrent {

    class WrapperInternal : public QObject
    {
      Q_OBJECT
    public:
      explicit WrapperInternal(QObject *parent = 0);
      virtual ~WrapperInternal();

      void initEngine(libtorrent::session_settings &settings);

      void setProfile(libtorrent::session_settings &settings);

      void loadSessionState();

      void start(const QString& id, TorrentConfig& config);

      /// <summary>Сгенерировать полностью заполненный fast resume файл на основе торрет файла.</summary>
      /// <param name="id">Id торрента.</param>
      /// <param name="config">Настройки добавляемого торрента.</param>
      void createFastResume(const QString& id, TorrentConfig& config);

      void stop(const QString& id);
      void remove(const QString& id);
      
      void setTorrentConfigDirectoryPath(const QString& path);
      void setListeningPort(unsigned short port);
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

      void setCredentials(const QString &userId, const QString &hash);

      void resetCredentials();

    signals:
      void listeningPortChanged(unsigned short port);
      void progressChanged(P1::Libtorrent::EventArgs::ProgressEventArgs args);

      void torrentStatusChanged(QString id, 
        P1::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus oldState, 
        P1::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus newState);

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
      P1::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus convertStatus(const libtorrent::torrent_status::state_t status);
      inline TorrentState* getStateByTorrentHandle(const libtorrent::torrent_handle &handle);
      inline TorrentState* getStateById(const QString& id);

      void calcDirectSpeed(P1::Libtorrent::EventArgs::ProgressEventArgs& args, const libtorrent::torrent_handle &handle);
      void emitTorrentProgress(const QString& id, const libtorrent::torrent_handle &handle);
      void emitTorrentProgress(
        const QString& id, 
        const libtorrent::torrent_handle& handle, 
        libtorrent::torrent_status &status, 
        libtorrent::torrent_status::state_t torrentState);

      void saveFastResumeWithoutLock(const libtorrent::torrent_handle &handle, boost::shared_ptr<libtorrent::entry> resumeData);
      void saveFastResumeEntry(const QString &resumeFilePath, const libtorrent::entry &resumeData);

      void saveSessionState();
      void cleanIdToTorrentStateMap();

      void updateCredentials();
      void updateTrackerCredentials(libtorrent::torrent_handle& handle);

      QString getSessionStatePath();

      QString _torrentConfigDirectoryPath;
      int _fastresumeCounter;
      int _fastresumeCounterMax;
      int _fastResumeWaitTimeInSec;

      int _startupListeningPort;
      libtorrent::session *_session;

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

      int _uploadRateLimit;
      int _downloadRateLimit;
      int _connectionsLimit;
    
      qint64 _lastDirectDownloaded;
      qint64 _lastPeerDownloaded;

      QString _credentialUserId;
      QString _credentialHash;
    };
  }
}
