/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <LibtorrentWrapper/WrapperInternal>

using namespace libtorrent;
namespace GGS {
  namespace Libtorrent
  {
    WrapperInternal::WrapperInternal(QObject *parent)
      : QObject(parent)
    {
      this->_fastResumeWaitTimeInSec = 30;
      this->_fastresumeCounterMax = 40;
      this->_fastresumeCounter = 0;
      this->_startupListeningPort = 11888;
      
      this->_errorNotificationHandler.wrapperInternal = this;
      this->_statusNotificationHandler.wrapperInternal = this;
      this->_trackerNotificationHandler.wrapperInternal = this;
      this->_storageNotificationHandler.wrapperInternal = this;

      qDebug() << "alertTimerTick connect is " << QObject::connect(&this->_alertTimer, SIGNAL(timeout()), this, SLOT(alertTimerTick()));
      qDebug() << "progressTimerTick connect is " <<QObject::connect(&this->_progressTimer, SIGNAL(timeout()), this, SLOT(progressTimerTick()));
    }

    WrapperInternal::~WrapperInternal()
    {
    }

    void WrapperInternal::initEngine()
    {
      //using namespace libtorrent;

      // 1. construct a session
      // 2. load_state()
      // 3. add_extension()
      // 4. start DHT, LSD, UPnP, NAT-PMP etc

      // UNDONE: прочитать опции и выбрать нужные. текущие поставил так, чтоб была заготовка куда сеттинги втыкать
      session_settings settings;
      settings.user_agent = "qgna/" LIBTORRENT_VERSION;
      settings.optimize_hashing_for_speed = true;
      settings.disk_cache_algorithm = session_settings::largest_contiguous;

      this->_session = new session(fingerprint("LT", LIBTORRENT_VERSION_MAJOR, LIBTORRENT_VERSION_MINOR, 0, 0)
        , session::start_default_features | session::add_default_plugins
        , alert::error_notification
        + alert::status_notification
        + alert::tracker_notification
        + alert::storage_notification
        );

      error_code ec;
      this->_session->listen_on(std::make_pair(this->_startupListeningPort, this->_startupListeningPort), ec);
      if (!ec) 
      {
        qDebug() << "can't listen on " << this->_startupListeningPort << " error code " << ec;
        emit this->listenFailed(this->_startupListeningPort, ec.value());
      }

      this->loadSessionState();
      
      this->_alertTimer.start(100);
      this->_progressTimer.start(1000);
    }

    void WrapperInternal::start(const QString& id, TorrentConfig& config)
    {
      QMutexLocker lock(&this->_torrentsMapLock);

      TorrentState *state = this->getStateById(id);
      if (!state) {
        this->loadAndStartTorrent(id, config);
        return;
      }

      if (config.isReloadRequired()) {
        this->_session->remove_torrent(state->handle());
        this->_idToTorrentState.remove(id);
        QString infohash = QString::fromStdString(state->handle().info_hash().to_string());
        this->_infohashToTorrentState.remove(infohash);
        delete state;
        this->loadAndStartTorrent(id, config);
      } else {
        torrent_handle handle = state->handle();
        if (handle.is_valid())
          handle.resume();

      }
    }

    void WrapperInternal::stop(const QString& id)
    {
      QMutexLocker lock(&this->_torrentsMapLock);

      TorrentState *state = this->getStateById(id);
      if (!state)
        return;

      if (state->handle().is_valid())
        state->handle().pause();
    }

    void WrapperInternal::remove(const QString& id)
    {
      QMutexLocker lock(&this->_torrentsMapLock);

      TorrentState *state = this->getStateById(id);
      if (!state)
        return;

      this->_session->remove_torrent(state->handle());
      this->_idToTorrentState.remove(id);
      QString infohash = QString::fromStdString(state->handle().info_hash().to_string());
      this->_infohashToTorrentState.remove(infohash);
      delete state;
    }

    void WrapperInternal::alertTimerTick()
    {
      std::auto_ptr<alert> alertObject = this->_session->pop_alert();
      while (alertObject.get())
      {
        try
        {
          if(alertObject->category() & alert::error_notification) {
            handle_alert<
              tracker_error_alert
              , tracker_warning_alert
              , scrape_failed_alert
              , torrent_delete_failed_alert
              , save_resume_data_failed_alert
              , url_seed_alert
              , file_error_alert
              , metadata_failed_alert
              , udp_error_alert
              , listen_failed_alert
              , portmap_error_alert
              , fastresume_rejected_alert
            >::handle_alert(alertObject, this->_errorNotificationHandler);
          } else if(alertObject->category() & alert::status_notification) {
            handle_alert<
              state_changed_alert
              , hash_failed_alert
              , torrent_finished_alert
              , torrent_paused_alert
              , torrent_resumed_alert
              , torrent_checked_alert
              , metadata_received_alert
              , external_ip_alert
              , listen_succeeded_alert
              , torrent_added_alert
              , trackerid_alert
            >::handle_alert(alertObject, this->_statusNotificationHandler);
          } else if(alertObject->category() & alert::tracker_notification) {
            handle_alert<
              tracker_announce_alert
              , scrape_reply_alert
              , tracker_reply_alert
              , dht_reply_alert>::handle_alert(alertObject, this->_trackerNotificationHandler);
          } else if(alertObject->category() & alert::storage_notification) {
            handle_alert<
              read_piece_alert
              , file_renamed_alert
              , file_rename_failed_alert
              , storage_moved_alert
              , storage_moved_failed_alert
              , torrent_deleted_alert
              , save_resume_data_alert
              , cache_flushed_alert>::handle_alert(alertObject, this->_storageNotificationHandler);
          }


        } catch(libtorrent::unhandled_alert &e) {
          QString str = QString::fromLocal8Bit(alertObject->message().c_str());
          qCritical() << "unhandled_alert category: " << alertObject->category() << " msg: " << str;
        }

        alertObject = this->_session->pop_alert();
      }
    }

    void WrapperInternal::progressTimerTick()
    {
      if (!this->_torrentsMapLock.tryLock())
        return;

      QMap<QString, TorrentState*>::const_iterator it = this->_idToTorrentState.constBegin();
      QMap<QString, TorrentState*>::const_iterator end = this->_idToTorrentState.constEnd();

      for(; it != end; ++it)
      {
        TorrentState *state = it.value();

        torrent_handle handle = state->handle();
        if (!handle.is_valid())
          continue;

        torrent_status status = handle.status();

        if (handle.status().state == torrent_status::downloading 
          || handle.status().state == torrent_status::checking_files) {
          this->emitTorrentProgress(state->id(), handle);
        }

        if (handle.status().state == torrent_status::downloading) {
          if (this->_fastresumeCounter > this->_fastresumeCounterMax) {
            handle.save_resume_data();
            this->_fastresumeCounter = 0;
          } else {
            this->_fastresumeCounter++;
          }
        }
      }

      this->_torrentsMapLock.unlock();
    }

    void WrapperInternal::loadAndStartTorrent(const QString& id, const TorrentConfig &config)
    {
      error_code ec;
      const wchar_t *path = reinterpret_cast<const wchar_t*>(config.pathToTorrentFile().utf16());
      torrent_info *torrentInfo = new torrent_info(path, ec);

      if (ec) {
        QString str = QString::fromLocal8Bit(ec.message().c_str());
        qDebug() << "Can't create torrent info " << str;
        emit this->startTorrentFailed(id, ec.value());
        return;
      }

      add_torrent_params p;
      p.flags = add_torrent_params::flag_override_resume_data;// | add_torrent_params::flag_paused;
      p.ti = torrentInfo;

      // Должен быть определен дефайн UNICODE
      // http://article.gmane.org/gmane.network.bit-torrent.libtorrent/1482/match=save+path
      p.save_path = config.downloadPath().toUtf8().data();
      p.storage_mode = libtorrent::storage_mode_sparse;

      QString resumeFilePath = this->getFastResumeFilePath(id);
      std::vector<char> buf;
      if (load_file(resumeFilePath.toUtf8().data(), buf, ec) == 0)
        p.resume_data = &buf;

      torrent_handle h = this->_session->add_torrent(p, ec);

      if (ec) {
        QString str = QString::fromLocal8Bit(ec.message().c_str());
        qDebug() << "start error " << str;
        emit this->startTorrentFailed(id, ec.value());
        return;
      }

      TorrentState *state = new TorrentState();
      state->setId(id);
      state->setHandle(h);

      this->_idToTorrentState[id] = state;
      QString infohash = QString::fromStdString(torrentInfo->info_hash().to_string());
      this->_infohashToTorrentState[infohash] = state;
    }

    ProgressEventArgs::TorrentStatus WrapperInternal::convertStatus(torrent_status::state_t status)
    {
      switch(status)
      {
      case torrent_status::queued_for_checking:
        return ProgressEventArgs::QueuedForChecking;
      case torrent_status::checking_files:
        return ProgressEventArgs::CheckingFiles;
      case torrent_status::downloading_metadata:
        return ProgressEventArgs::DownloadingMetadata;
      case torrent_status::downloading:
        return ProgressEventArgs::Downloading;
      case torrent_status::finished:
        return ProgressEventArgs::Finished;
      case torrent_status::seeding:
        return ProgressEventArgs::Seeding;
      case torrent_status::allocating:
        return ProgressEventArgs::Allocating;
      case torrent_status::checking_resume_data:
        return ProgressEventArgs::CheckingResumeData;
      }

      return ProgressEventArgs::Finished;
    }

    void WrapperInternal::saveFastResume(const torrent_handle &handle, boost::shared_ptr<entry> resumeData)
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      this->saveFastResumeWithoutLock(handle, resumeData);
    }

    void WrapperInternal::saveFastResumeWithoutLock(const torrent_handle &handle, boost::shared_ptr<entry> resumeData)
    {
      TorrentState *state = this->getStateByTorrentHandle(handle);
      if (!state)
        return;

      QString resumeFilePath = this->getFastResumeFilePath(state->id());
      this->createDirectoryIfNotExists(resumeFilePath);

      const wchar_t *resumePath = reinterpret_cast<const wchar_t*>(resumeFilePath.utf16());
      boost::filesystem::ofstream out(resumePath, std::ios_base::binary);
      out.unsetf(std::ios_base::skipws);
      bencode(std::ostream_iterator<char>(out), *resumeData);
      out.close();
    }

    QString WrapperInternal::getFastResumeFilePath(const QString& id)
    {
      QString resumeFilePath = QString("%1/%2.resume").arg(this->_torrentConfigDirectoryPath, id);
      return resumeFilePath;
    }

    void WrapperInternal::shutdown()
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      this->_alertTimer.stop();
      this->_progressTimer.stop();

      this->_session->pause();
      int numResumeData = 0;

      QMap<QString, TorrentState*>::const_iterator it = this->_idToTorrentState.constBegin();
      QMap<QString, TorrentState*>::const_iterator end = this->_idToTorrentState.constEnd();

      for(; it != end; ++it)
      {
        torrent_handle h = it.value()->handle();
        if (!h.is_valid()) 
          continue;

        if (h.status().paused) 
          continue;

        if (!h.status().has_metadata)
          continue;

        h.save_resume_data();
        ++numResumeData;
      }

      while (numResumeData > 0)
      {
        alert const* a = this->_session->wait_for_alert(seconds(this->_fastResumeWaitTimeInSec));
        if (a == 0)
        {
          qDebug() << "failed to wait for all fast resume saved";
          break;
        }

        std::auto_ptr<alert> holder = this->_session->pop_alert();
        const save_resume_data_failed_alert *failAlert = alert_cast<save_resume_data_failed_alert>(a);
        if (failAlert)
        {
          qDebug() << "failed to safe fastresume " << QString::fromStdString(failAlert->message());
          --numResumeData;
          continue;
        }

        save_resume_data_alert const* rd = alert_cast<save_resume_data_alert>(a);
        if (!rd) 
          continue;

        --numResumeData;

        if (!rd->resume_data) 
          continue;

        this->saveFastResumeWithoutLock(rd->handle, rd->resume_data);
      }

      this->saveSessionState();

      delete this->_session;
      this->_session = 0;
      
      this->cleanIdToTorrentStateMap();
    }

    void WrapperInternal::createDirectoryIfNotExists(const QString& resumeFilePath)
    {
      QDir dir = QFileInfo(resumeFilePath).absoluteDir();
      if (!dir.exists())
        dir.mkpath(dir.absolutePath());
    }

    void WrapperInternal::torrentPausedAlert(const torrent_handle &handle)
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      TorrentState *state = getStateByTorrentHandle(handle);

      if (state && handle.is_valid()) {
        handle.save_resume_data();
        emitTorrentProgress(state->id(), handle);
      }
    }

    void WrapperInternal::emitTorrentProgress(const QString& id, const torrent_handle &handle)
    {
      torrent_status status = handle.status();
      ProgressEventArgs args;
      args.setId(id);
      args.setProgress(status.progress);
      args.setStatus(this->convertStatus(status.state));
      args.setDownloadRate(status.download_rate);
      args.setUploadRate(status.upload_rate);
      args.setTotalWanted(status.total_wanted);
      args.setTotalWantedDone(status.total_wanted_done);
      emit this->progressChanged(args);
    }

    void WrapperInternal::trackerErrorAlert(const torrent_handle &handle, int failCountInARow, int httpStatusCode)
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      TorrentState *state = this->getStateByTorrentHandle(handle);
      if (state)
        emit this->trackerFailed(state->id(), failCountInARow, httpStatusCode);
    }

    void WrapperInternal::fileErrorAlert(const torrent_handle &handle, const QString& filePath, int errorCode)
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      TorrentState *state = this->getStateByTorrentHandle(handle);
      if (state)
        emit this->fileError(state->id(), filePath, errorCode);
    }

    void WrapperInternal::listenFailAlert(int port, int errorCode)
    {
      emit this->listenFailed(port, errorCode);
    }

    void WrapperInternal::torrentStatusChangedAlert(const torrent_handle &handle, torrent_status::state_t oldState , torrent_status::state_t newState )
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      TorrentState *state = this->getStateByTorrentHandle(handle);
      if (state && handle.is_valid()) {
        emit this->torrentStatusChanged(state->id(), this->convertStatus(oldState), this->convertStatus(newState));
        this->emitTorrentProgress(state->id(), handle);
      }
    }

    void WrapperInternal::torrentFinishedAlert(const torrent_handle &handle)
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      TorrentState *state = this->getStateByTorrentHandle(handle);
      if (state && handle.is_valid()) {
        emit this->torrentDownloadFinished(state->id());
        this->emitTorrentProgress(state->id(), handle);
      }
    }

    void WrapperInternal::torrentResumedAlert(const torrent_handle &handle)
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      TorrentState *state = this->getStateByTorrentHandle(handle);
      if (state)
        emit this->torrentResumed(state->id());
    }

    TorrentState* WrapperInternal::getStateByTorrentHandle(const torrent_handle &handle)
    {
      if (!handle.is_valid())
        return 0;

      QString infoHash = QString::fromStdString(handle.info_hash().to_string());

      if (!this->_infohashToTorrentState.contains(infoHash)) 
        return 0;

      return this->_infohashToTorrentState[infoHash];
    }

    void WrapperInternal::changeListeningPort(unsigned short port)
    {
      error_code ec;
      this->_session->listen_on(std::make_pair(port, port), ec);
      if (ec) {
        qDebug() << "can't start listen code: " << ec;
        emit this->listenFailed(port, ec.value());
        return;
      }

      int newport = this->_session->listen_port();
      qDebug() << "start listen on " << newport;
      emit this->listeningPortChanged(newport);
    }

    void WrapperInternal::saveSessionState()
    {
      entry sessionState;
      this->_session->save_state(sessionState);

      QString resumeFilePath = this->getSessionStatePath();
      this->createDirectoryIfNotExists(resumeFilePath);

      const wchar_t *resumePath = reinterpret_cast<const wchar_t*>(resumeFilePath.utf16());
      boost::filesystem::ofstream out(resumePath, std::ios_base::binary);
      out.unsetf(std::ios_base::skipws);
      bencode(std::ostream_iterator<char>(out), sessionState);
      out.close();
    }

    unsigned short WrapperInternal::listeningPort() const
    {
      if (this->_session)
        return this->_session->listen_port();

      return 0;
    }

    void WrapperInternal::setUploadRateLimit(int bytesPerSecond)
    {
      if (!this->_session)
        return;

      this->_session->set_upload_rate_limit(bytesPerSecond);
    }

    void WrapperInternal::setDownloadRateLimit(int bytesPerSecond)
    {
      if (!this->_session)
        return;

      this->_session->set_download_rate_limit(bytesPerSecond);
    }

    int WrapperInternal::uploadRateLimit() const
    {
      if (!this->_session)
        return 0;

      return this->_session->upload_rate_limit();
    }

    int WrapperInternal::downloadRateLimit() const
    {
      if (!this->_session)
        return 0;

      return this->_session->download_rate_limit();
    }

    TorrentState* WrapperInternal::getStateById(const QString& id)
    {
      if (!this->_idToTorrentState.contains(id)) 
        return 0;

      return this->_idToTorrentState[id];
    }

    QString WrapperInternal::getSessionStatePath()
    {
      QString resumeFilePath = QString("%1/.session_state").arg(this->_torrentConfigDirectoryPath);
      return resumeFilePath;
    }

    void WrapperInternal::loadSessionState()
    {
      std::vector<char> in;
      error_code ec;
      QString sessionStatePath = this->getSessionStatePath();
      if (load_file(sessionStatePath.toUtf8().data(), in, ec) == 0) {
        lazy_entry e;
        int result = lazy_bdecode(&in[0], &in[0] + in.size(), e, ec);
        if (result == 0)
          this->_session->load_state(e);

      } else {
        qDebug() << "Can't load session state. Error code: " << ec;
      }
    }

    void WrapperInternal::torrentUrlSeedAlert(const torrent_handle &handle, const std::string& url )
    {
      qDebug() << "url seed banned " << QString::fromStdString(url);
      if (handle.is_valid())
        handle.add_url_seed(url);
    }

    void WrapperInternal::cleanIdToTorrentStateMap()
    {
      QMap<QString, TorrentState*>::const_iterator it = this->_idToTorrentState.constBegin();
      QMap<QString, TorrentState*>::const_iterator end = this->_idToTorrentState.constEnd();

      for(; it != end; ++it)
        delete it.value();
    }

  }
}
