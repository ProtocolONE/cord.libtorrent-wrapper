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
#include <QtCore/QFileInfo>
#include <QtCore/QSysInfo>

#include <libtorrent/hasher.hpp>

#define SIGNAL_CONNECT_CHECK(X) { bool result = X; Q_ASSERT_X(result, __FUNCTION__ , #X); }

using namespace libtorrent;

namespace GGS {
  namespace Libtorrent
  {
    WrapperInternal::WrapperInternal(QObject *parent)
      : QObject(parent)
      , _session(0)
      , _seedEnabled(false)
      , _shuttingDown(false)
      , _initialized(false)
	  , _lastDirectDownloaded(0)
	  , _lastPeerDownloaded(0)
    {
      this->_fastResumeWaitTimeInSec = 30;
      this->_fastresumeCounterMax = 40;
      this->_fastresumeCounter = 0;
      this->_startupListeningPort = 11888;

      this->_errorNotificationHandler.wrapperInternal = this;
      this->_statusNotificationHandler.wrapperInternal = this;
      this->_trackerNotificationHandler.wrapperInternal = this;
      this->_storageNotificationHandler.wrapperInternal = this;

      SIGNAL_CONNECT_CHECK(QObject::connect(&this->_alertTimer, SIGNAL(timeout()), this, SLOT(alertTimerTick())));
      SIGNAL_CONNECT_CHECK(QObject::connect(&this->_progressTimer, SIGNAL(timeout()), this, SLOT(progressTimerTick())));
    }

    WrapperInternal::~WrapperInternal()
    {
      delete this->_session;
    }

    void WrapperInternal::initEngine()
    {
      QMutexLocker lock(&this->_torrentsMapLock);

      if (this->_shuttingDown)
        return;

      // 1. construct a session
      // 2. load_state()
      // 3. add_extension()
      // 4. start DHT, LSD, UPnP, NAT-PMP etc
      this->_sessionsSettings.user_agent = std::string("qgna/").append(LIBTORRENT_VERSION);
      this->_sessionsSettings.optimize_hashing_for_speed = true;
      this->_sessionsSettings.disk_cache_algorithm = session_settings::largest_contiguous;
      this->_sessionsSettings.dont_count_slow_torrents = true;
      this->_sessionsSettings.ban_web_seeds = false;
      this->_sessionsSettings.allow_reordered_disk_operations = true;
      this->_sessionsSettings.no_connect_privileged_ports = true;
      this->_sessionsSettings.lock_files = false;
      
      this->_sessionsSettings.prefer_udp_trackers = false;
      this->_sessionsSettings.rate_limit_utp = true;
      this->_sessionsSettings.tick_interval = 90;
      this->_sessionsSettings.share_mode_target = 2;
      this->_sessionsSettings.choking_algorithm = session_settings::bittyrant_choker;
      this->_sessionsSettings.torrent_connect_boost = 20; 
      this->_sessionsSettings.utp_num_resends = 4;
      this->_sessionsSettings.allow_multiple_connections_per_ip = true;
      this->_sessionsSettings.max_failcount = 1;
      this->_sessionsSettings.min_reconnect_time = 30;
      this->_sessionsSettings.peer_connect_timeout = 10;
      this->_sessionsSettings.inactivity_timeout = 180;
      this->_sessionsSettings.mixed_mode_algorithm = session_settings::prefer_tcp;
      
      // QGNA-278 Ограничим полуоткрытые для XP
      if (QSysInfo::windowsVersion() == QSysInfo::WV_XP)
        this->_sessionsSettings.half_open_limit = 5;

      this->_session = new session(fingerprint("LT", LIBTORRENT_VERSION_MAJOR, LIBTORRENT_VERSION_MINOR, 0, 0)
        , session::start_default_features | session::add_default_plugins
        , alert::error_notification
        + alert::status_notification
        + alert::tracker_notification
        + alert::storage_notification
        );

      // UNDONE: беда тут - а на выходе чутка подтекает.
      this->_session->start_lsd();
      this->_session->start_upnp();
      this->_session->start_natpmp();
      
      this->_session->add_dht_router(std::make_pair(std::string("router.bittorrent.com"), 6881));
      this->_session->add_dht_router(std::make_pair(std::string("router.utorrent.com"), 6881));
      this->_session->add_dht_router(std::make_pair(std::string("router.bitcomet.com"), 6881));

      error_code ec;
      this->_session->listen_on(std::make_pair(this->_startupListeningPort, this->_startupListeningPort), ec);
      if (ec) {
        DEBUG_LOG << "can't listen on " << this->_startupListeningPort << " error code " << ec;
        emit this->listenFailed(this->_startupListeningPort, ec.value());
      }

      this->_session->start_dht();

      this->loadSessionState();
      this->_session->set_settings(this->_sessionsSettings);

      if (this->_seedEnabled)
        QTimer::singleShot(300000, this, SLOT(backgroundSeedStart()));

      this->_alertTimer.start(100);
      this->_progressTimer.start(1000);

      this->_initialized = true;
    }

    void WrapperInternal::start(const QString& id, TorrentConfig& config)
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      if (!this->_initialized)
        return;

      TorrentState *state = this->getStateById(id);
      if (!state) {
        this->loadAndStartTorrent(id, config);
        return;
      }

      DEBUG_LOG << "start " << id
        << " background " << state->backgroundSeeding()
        << " reload require  " << config.isReloadRequired();

      if (state->pathToTorrent() != config.pathToTorrentFile())
        config.setIsReloadRequired(true);
      
      if (config.isReloadRequired()) {
        this->_session->remove_torrent(state->handle());
        this->_idToTorrentState.remove(id);
        QString infohash = QString::fromStdString(state->handle().info_hash().to_string());
        this->_infohashToTorrentState.remove(infohash);
        delete state;
        this->loadAndStartTorrent(id, config);
      } else {
        state->setBackgroundSeeding(false);
        torrent_handle handle = state->handle();

        // UNDONE: Тут могут остаьтся пробелмы с другими стадиями.
        if (handle.is_valid()) {

          if (config.rehashOnly()) {
            if (handle.status().state == torrent_status::seeding) {
              emit this->torrentRehashed(id, true);
            } else if (handle.status().state == torrent_status::downloading) {
              emit this->torrentRehashed(id, false);
            } else {
              handle.resume();
            }
          } else {
            if (handle.status().state == torrent_status::seeding)
              emit this->torrentDownloadFinished(id);
            else 
              handle.resume();
          }

        }
      }
    }

    void WrapperInternal::stop(const QString& id)
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      if (!this->_initialized)
        return;

      TorrentState *state = this->getStateById(id);
      if (!state)
        return;

      if (state->handle().is_valid()) {
        state->setIsStopping(true);
        state->handle().pause();
      }
    }

    void WrapperInternal::remove(const QString& id)
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      if (!this->_initialized)
        return;

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
              , torrent_error_alert
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
              , torrent_removed_alert
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
          qCritical() << "unhandled_alert category: " << alertObject->category() << typeid(*alertObject).name() << " msg: " << str;
        }

        alertObject = this->_session->pop_alert();
      }
    }

    void WrapperInternal::progressTimerTick()
    {
      if (!this->_torrentsMapLock.tryLock())
        return;

      if (!this->_initialized)
        return;

      QMap<QString, TorrentState*>::const_iterator it = this->_idToTorrentState.constBegin();
      QMap<QString, TorrentState*>::const_iterator end = this->_idToTorrentState.constEnd();

      for(; it != end; ++it) {
        TorrentState *state = it.value();
        if (state->backgroundSeeding())
          continue;

        torrent_handle handle = state->handle();
        if (!handle.is_valid())
          continue;

        torrent_status status = handle.status(0);

        if (status.paused)
          continue;

        if (status.state == torrent_status::downloading 
          || status.state == torrent_status::checking_files) {
            this->emitTorrentProgress(state->id(), handle);
        }

        if (status.state == torrent_status::downloading) {
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

    void WrapperInternal::loadAndStartTorrent(const QString& id, const TorrentConfig &config, bool backgroudSeeding)
    {
      if (config.isSeedEnable()) {
        ResumeInfo resumeInfo;
        resumeInfo.setId(id);
        resumeInfo.setSavePath(config.downloadPath());
        resumeInfo.setTorrentPath(config.pathToTorrentFile());
        this->_resumeInfo[id] = resumeInfo;
      }

      if (backgroudSeeding) 
        DEBUG_LOG << "background " << id;

      error_code ec;
      torrent_info *torrentInfo = new torrent_info(config.pathToTorrentFile().toUtf8().data(), ec);

      if (ec) {
        QString str = QString::fromLocal8Bit(ec.message().c_str());
        WARNING_LOG << "Can't create torrent info from file" << config.pathToTorrentFile() << "with reasons" << str;
        emit this->startTorrentFailed(id, ec.value());
        return;
      }

      add_torrent_params p;
      p.flags = add_torrent_params::flag_override_resume_data | 
                add_torrent_params::flag_use_resume_save_path; // Сомнительное название вводящее в заблуждение, если этот флаг выставлен
                                                               // то libtorrent не будет использовать путь из resume_data
      p.ti = torrentInfo;

      // Должен быть определен дефайн UNICODE
      // http://article.gmane.org/gmane.network.bit-torrent.libtorrent/1482/match=save+path
      QByteArray downloadPathArray = config.downloadPath().toUtf8();
      p.save_path = downloadPathArray.data();
      p.storage_mode = libtorrent::storage_mode_sparse;

      QString resumeFilePath = this->getFastResumeFilePath(id);
      std::vector<char> buf;
      if (!config.isForceRehash()) {
        if (load_file(resumeFilePath.toUtf8().data(), buf, ec) == 0)
          p.resume_data = &buf;
        else
          DEBUG_LOG << "can't load fast resume for" << id << "error" << ec.value();

      } else {
        DEBUG_LOG << "force rehash for" << id;
      }

      torrent_handle h = this->_session->add_torrent(p, ec);

      if (ec) {
        QString str = QString::fromLocal8Bit(ec.message().c_str());
        CRITICAL_LOG << "start error" << str << "in" << id;
        emit this->startTorrentFailed(id, ec.value());
        return;
      }

      TorrentState *state = new TorrentState();
      state->setId(id);
      state->setHandle(h);
      state->setBackgroundSeeding(backgroudSeeding);
      state->setRehashOnly(config.rehashOnly());
      state->setIsSeedEnable(config.isSeedEnable());
      state->setPathToTorrent(config.pathToTorrentFile());

      this->_idToTorrentState[id] = state;
      QString infohash = QString::fromStdString(torrentInfo->info_hash().to_string());
      this->_infohashToTorrentState[infohash] = state;
    }

    GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus WrapperInternal::convertStatus(torrent_status::state_t status)
    {
      switch(status)
      {
      case torrent_status::queued_for_checking:
        return GGS::Libtorrent::EventArgs::ProgressEventArgs::QueuedForChecking;
      case torrent_status::checking_files:
        return GGS::Libtorrent::EventArgs::ProgressEventArgs::CheckingFiles;
      case torrent_status::downloading_metadata:
        return GGS::Libtorrent::EventArgs::ProgressEventArgs::DownloadingMetadata;
      case torrent_status::downloading:
        return GGS::Libtorrent::EventArgs::ProgressEventArgs::Downloading;
      case torrent_status::finished:
        return GGS::Libtorrent::EventArgs::ProgressEventArgs::Finished;
      case torrent_status::seeding:
        return GGS::Libtorrent::EventArgs::ProgressEventArgs::Seeding;
      case torrent_status::allocating:
        return GGS::Libtorrent::EventArgs::ProgressEventArgs::Allocating;
      case torrent_status::checking_resume_data:
        return GGS::Libtorrent::EventArgs::ProgressEventArgs::CheckingResumeData;
      }

      return GGS::Libtorrent::EventArgs::ProgressEventArgs::Finished;
    }

    void WrapperInternal::saveFastResume(const torrent_handle &handle, boost::shared_ptr<entry> resumeData)
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      if (!this->_initialized)
        return;

      this->saveFastResumeWithoutLock(handle, resumeData);
    }

    void WrapperInternal::saveFastResumeWithoutLock(const torrent_handle &handle, boost::shared_ptr<entry> resumeData)
    {
      TorrentState *state = this->getStateByTorrentHandle(handle);
      if (!state)
        return;

      QString resumeFilePath = this->getFastResumeFilePath(state->id());
      this->createDirectoryIfNotExists(resumeFilePath);
      try {
        const wchar_t *resumePath = reinterpret_cast<const wchar_t*>(resumeFilePath.utf16());
        boost::filesystem::ofstream out(resumePath, std::ios_base::binary);
        out.unsetf(std::ios_base::skipws);
        bencode(std::ostream_iterator<char>(out), *resumeData);
        out.close();
      } catch(boost::filesystem::filesystem_error& err) {
        DEBUG_LOG << err.what();
      } catch(std::exception& stdExc) {
        DEBUG_LOG << stdExc.what();
      }
    }

    QString WrapperInternal::getFastResumeFilePath(const QString& id)
    {
      return QString("%1/%2.resume").arg(this->_torrentConfigDirectoryPath, id);
    }

    void WrapperInternal::shutdown()
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      this->_shuttingDown = true;
      if (!this->_initialized)
        return;

      this->_initialized = false;

      this->_alertTimer.stop();
      this->_progressTimer.stop();

      this->_session->pause();
      this->_session->stop_lsd();
      this->_session->stop_upnp();
      this->_session->stop_natpmp();

      int numResumeData = 0;

      QMap<QString, TorrentState*>::const_iterator it = this->_idToTorrentState.constBegin();
      QMap<QString, TorrentState*>::const_iterator end = this->_idToTorrentState.constEnd();

      for(; it != end; ++it) {
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

      while (numResumeData > 0) {
        alert const* a = this->_session->wait_for_alert(seconds(this->_fastResumeWaitTimeInSec));
        if (a == 0) {
          WARNING_LOG << "failed to wait for all fast resume saved";
          break;
        }

        std::auto_ptr<alert> holder = this->_session->pop_alert();
        const save_resume_data_failed_alert *failAlert = alert_cast<save_resume_data_failed_alert>(a);
        if (failAlert) {
          WARNING_LOG << "failed to save fast resume" << QString::fromStdString(failAlert->message());
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
      this->_session->stop_dht();

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
      if (!this->_initialized)
        return;

      TorrentState *state = getStateByTorrentHandle(handle);

      DEBUG_LOG << "torrentPausedAlert"
        << (state ? state->id() : "")
        << (state ? state->backgroundSeeding() : "");

      if (!state || state->backgroundSeeding() || !handle.is_valid() || !state->isSeedEnable())
        return;

      handle.save_resume_data();
      if (state->isStopping()) {
        state->setIsStopping(false);
        this->emitTorrentProgress(state->id(), handle);
        emit this->torrentPaused(state->id());
      }
    }

    void WrapperInternal::calcDirectSpeed(GGS::Libtorrent::EventArgs::ProgressEventArgs& args, const libtorrent::torrent_handle &handle)
    {
      std::vector<libtorrent::peer_info> peerInfo;

      handle.get_peer_info(peerInfo);

      int peerInfoSize = peerInfo.size();

      int directDownloadSpeed = 0;
      int peerDownloadSpeed = 0;

      qint64 directDownloaded = 0;
      qint64 peerDownloaded = 0;

      for (std::vector<libtorrent::peer_info>::iterator i = peerInfo.begin(); i != peerInfo.end(); ++i) {
        if ((*i).connection_type == libtorrent::peer_info::web_seed) {
          directDownloadSpeed += (*i).down_speed;
          directDownloaded += (*i).total_download;
        } 
        if ((*i).connection_type != libtorrent::peer_info::web_seed) {
          peerDownloadSpeed += (*i).down_speed;
          peerDownloaded += (*i).total_download;
        } 
      }

      if (peerInfoSize > 0) {
        this->_lastDirectDownloaded = directDownloaded;
        this->_lastPeerDownloaded = peerDownloaded;
      }

      args.setDirectPayloadDownloadRate(directDownloadSpeed);
      args.setDirectTotalDownload(directDownloaded);

      args.setPeerPayloadDownloadRate(peerDownloadSpeed);
      args.setPeerTotalDownload(peerDownloaded);
    }

    void WrapperInternal::emitTorrentProgress(const QString& id, const torrent_handle &handle)
    {
      torrent_status status = handle.status(0);
      this->emitTorrentProgress(id, handle, status, status.state);
    }

    void WrapperInternal::emitTorrentProgress(
      const QString& id, 
      const torrent_handle& handle, 
      torrent_status &status, 
      torrent_status::state_t torrentState)
    {
      GGS::Libtorrent::EventArgs::ProgressEventArgs args;
      args.setId(id);
      args.setProgress(status.progress);
      args.setStatus(this->convertStatus(torrentState));

      args.setTotalWanted(status.total_wanted);
      args.setTotalWantedDone(status.total_wanted_done);

      args.setDownloadRate(status.download_rate);
      args.setUploadRate(status.upload_rate);

      //  down\up payload rate
      args.setPayloadDownloadRate(status.download_payload_rate);
      args.setPayloadUploadRate(status.upload_payload_rate);

      this->calcDirectSpeed(args, handle);

      //  down\up payload total
      args.setPayloadTotalDownload(this->_lastDirectDownloaded + this->_lastPeerDownloaded);     
      args.setTotalPayloadUpload(status.total_payload_upload);

      //  в случае паузы вычислить скачанное по пирам невозможно, поэтому показываем последние значения
      if (status.paused) {
        args.setDirectPayloadDownloadRate(0);
        args.setPeerPayloadDownloadRate(0);
        args.setDirectTotalDownload(this->_lastDirectDownloaded);
        args.setPeerTotalDownload(this->_lastPeerDownloaded);
        args.setPayloadUploadRate(0);
      }

      emit this->progressChanged(args);
    }

    void WrapperInternal::trackerErrorAlert(const torrent_handle &handle, int failCountInARow, int httpStatusCode)
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      if (!this->_initialized)
        return;

      TorrentState *state = this->getStateByTorrentHandle(handle);
      if (state && !state->backgroundSeeding())
        emit this->trackerFailed(state->id(), failCountInARow, httpStatusCode);
    }

    void WrapperInternal::fileErrorAlert(const torrent_handle &handle, const QString& filePath, int errorCode)
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      if (!this->_initialized)
        return;

      TorrentState *state = this->getStateByTorrentHandle(handle);
      if (state && !state->backgroundSeeding())
        emit this->fileError(state->id(), filePath, errorCode);
    }

    void WrapperInternal::listenFailAlert(int port, int errorCode)
    {
      emit this->listenFailed(port, errorCode);
    }

    void WrapperInternal::torrentStatusChangedAlert(const torrent_handle &handle, torrent_status::state_t oldState , torrent_status::state_t newState)
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      if (!this->_initialized)
        return;

      TorrentState *state = this->getStateByTorrentHandle(handle);
      if (!state || !handle.is_valid())
        return;

      DEBUG_LOG << "torrentStatusChangedAlert " << state->id() 
        << " old " << oldState 
        << " new " << newState
        << " background " << state->backgroundSeeding();

      if (state->backgroundSeeding()) {
        if (newState == torrent_status::downloading) {
          torrent_status status = handle.status(0);
          if (!status.is_finished)
            state->handle().pause();
        }

        return;
      }

      if (state->rehashOnly()) {
        if (newState == torrent_status::downloading) {
          torrent_status status = handle.status(0);
          if (!status.is_finished) {
            state->handle().pause();
            emit this->torrentRehashed(state->id(), false);
          }

          return;
        } else if (newState == torrent_status::seeding) {
          emit this->torrentRehashed(state->id(), true);
          return;
        }
      }

      if (!state->isSeedEnable() && newState == torrent_status::seeding)
        state->handle().pause();

      emit this->torrentStatusChanged(state->id(), this->convertStatus(oldState), this->convertStatus(newState));
      
      torrent_status status = handle.status(0);
      this->emitTorrentProgress(state->id(), handle, status, oldState);
    }

    void WrapperInternal::torrentFinishedAlert(const torrent_handle &handle)
    {
      if (!handle.is_valid())
        return;

      handle.save_resume_data();

      QMutexLocker lock(&this->_torrentsMapLock);
      if (!this->_initialized)
        return;

      TorrentState *state = this->getStateByTorrentHandle(handle);

      if (!state)
        return;

      DEBUG_LOG << "torrentFinishedAlert " << state->id() 
        << " background " << state->backgroundSeeding();

      // Торрент скачан и готов к фоновому сидированию
      if (state->isSeedEnable()) {
        this->_resumeInfo[state->id()].setFinished(true);
        this->saveSessionState();
      }

      if (!this->_seedEnabled)
        handle.pause();

      if (state->backgroundSeeding())
        return;

      emit this->torrentDownloadFinished(state->id());
      this->emitTorrentProgress(state->id(), handle);
      
      torrent_status status = handle.status(0);
      this->emitTorrentProgress(state->id(), handle, status, torrent_status::downloading);
    }

    void WrapperInternal::torrentResumedAlert(const torrent_handle &handle)
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      if (!this->_initialized)
        return;

      TorrentState *state = this->getStateByTorrentHandle(handle);
      if (state && !state->backgroundSeeding())
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
        CRITICAL_LOG << "can't start listen code: " << ec << "for port" << port;
        emit this->listenFailed(port, ec.value());
        return;
      }

      int newport = this->_session->listen_port();
      DEBUG_LOG << "start listen on " << newport;
      emit this->listeningPortChanged(newport);
    }

    void WrapperInternal::saveSessionState()
    {
      entry sessionState;
      this->_session->save_state(sessionState, 
        libtorrent::session::save_settings |
        libtorrent::session::save_dht_settings |
        libtorrent::session::save_dht_state |
        libtorrent::session::save_feeds);

      entry resumeEntry;
      Q_FOREACH(ResumeInfo resumeInfo, this->_resumeInfo) {
        if (!resumeInfo.finished())
          continue;

        entry info;
        std::string id(resumeInfo.id().toUtf8());
        std::string torrentPath(resumeInfo.torrentPath().toUtf8());
        std::string savePath(resumeInfo.savePath().toUtf8());

        info["torrentPath"] = torrentPath;
        info["savePath"] = savePath;

        resumeEntry[id] = info;
      }

      sessionState["GGSResumeInfo"] = resumeEntry;

      QString resumeFilePath = this->getSessionStatePath();
      this->createDirectoryIfNotExists(resumeFilePath);

      const wchar_t *resumePath = reinterpret_cast<const wchar_t*>(resumeFilePath.utf16());
      try {
        boost::filesystem::ofstream out(resumePath, std::ios_base::binary);
        out.unsetf(std::ios_base::skipws);
        bencode(std::ostream_iterator<char>(out), sessionState);
        out.close();
      } catch(boost::filesystem::filesystem_error& err) {
        DEBUG_LOG << err.what();
      } catch(std::exception& stdExc) {
        DEBUG_LOG << stdExc.what();
      }
    }

    void WrapperInternal::loadSessionState()
    {
      std::vector<char> in;
      error_code ec;
      QString sessionStatePath = this->getSessionStatePath();
      QFileInfo fileInfo(sessionStatePath);
      if (!fileInfo.exists()) {
        DEBUG_LOG << "session state file" << sessionStatePath << "not exists";
        return;
      }

      if (load_file(sessionStatePath.toUtf8().data(), in, ec) != 0) {
        CRITICAL_LOG << "Can't load session state from" << sessionStatePath <<"with error code:" << ec;
        return;
      }

      lazy_entry e;
      if (lazy_bdecode(&in[0], &in[0] + in.size(), e, ec) != 0) 
        return;

      this->_session->load_state(e);

      lazy_entry *resumeInfo = e.dict_find("GGSResumeInfo");
      if (!resumeInfo) 
        return;

      int size = resumeInfo->dict_size();
      for (int i = 0; i < size; ++i) {
        std::pair<std::string, lazy_entry const*> p = resumeInfo->dict_at(i);
        QString id = QString::fromUtf8(p.first.c_str());
        QString torrentPath = QString::fromUtf8(p.second->dict_find_string_value("torrentPath").c_str());
        QString savePath = QString::fromUtf8(p.second->dict_find_string_value("savePath").c_str());
        ResumeInfo info;
        info.setId(id);
        info.setFinished(true);
        info.setTorrentPath(torrentPath);
        info.setSavePath(savePath);
        this->_resumeInfo[id] = info;
      }
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

      this->_sessionsSettings.upload_rate_limit = bytesPerSecond;
      this->_session->set_settings(this->_sessionsSettings);
    }

    void WrapperInternal::setDownloadRateLimit(int bytesPerSecond)
    {
      if (!this->_session)
        return;

      this->_sessionsSettings.download_rate_limit = bytesPerSecond;
      this->_session->set_settings(this->_sessionsSettings);
    }

    int WrapperInternal::uploadRateLimit() const
    {
      if (!this->_session)
        return 0;
      return this->_session->settings().upload_rate_limit;
    }

    int WrapperInternal::downloadRateLimit() const
    {
      if (!this->_session)
        return 0;

      return this->_session->settings().download_rate_limit;
    }

    TorrentState* WrapperInternal::getStateById(const QString& id)
    {
      if (!this->_idToTorrentState.contains(id)) 
        return 0;

      return this->_idToTorrentState[id];
    }

    QString WrapperInternal::getSessionStatePath()
    {
      return QString("%1/.session_state").arg(this->_torrentConfigDirectoryPath);
    }

    void WrapperInternal::torrentUrlSeedAlert(const torrent_handle &handle, const std::string& url )
    {
      DEBUG_LOG << "url seed banned" << QString::fromStdString(url);
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

    void WrapperInternal::torrentErrorAlert( const libtorrent::torrent_handle &handle )
    {
      if (!handle.is_valid())
        return;

      QMutexLocker lock(&this->_torrentsMapLock);
      if (!this->_initialized)
        return;

      TorrentState *state = this->getStateByTorrentHandle(handle);
      if (state && !state->backgroundSeeding())
        emit this->torrentError(state->id());
    }      

    int WrapperInternal::maxConnection()
    {
      if (!this->_session)
        return 0;

      return this->_session->settings().connections_limit;
    }

    void WrapperInternal::setMaxConnection(int maxConnection)
    {
      if (!this->_session)
        return;

      this->_sessionsSettings.connections_limit = maxConnection;
      this->_session->set_settings(this->_sessionsSettings);
    }

    void WrapperInternal::pauseSession()
    {
      if (!this->_session)
        return;

      this->_session->pause();
    }

    void WrapperInternal::resumeSession()
    {
      if (!this->_session)
        return;

      this->_session->resume();
    }

    void WrapperInternal::backgroundSeedStart()
    {
      QMutexLocker lock(&this->_torrentsMapLock);
      if (!this->_initialized)
        return;

      Q_FOREACH(ResumeInfo info, this->_resumeInfo) {
        if (!info.finished())
          continue;

        if (this->_idToTorrentState.contains(info.id()))
          continue;

        TorrentConfig config;
        config.setDownloadPath(info.savePath());
        config.setIsForceRehash(false);
        config.setIsReloadRequired(false);
        config.setPathToTorrentFile(info.torrentPath());
        this->loadAndStartTorrent(info.id(), config, true);
      }
    }

    bool WrapperInternal::seedEnabled() const
    {
      return this->_seedEnabled;
    }

    void WrapperInternal::setSeedEnabled(bool value)
    {
      this->_seedEnabled = value;
    }

    bool WrapperInternal::getInfoHash(const QString& path, QString& result)
    {
      std::vector<char> in;
      error_code ec;
      
      QFileInfo fileInfo(path);
      if (!fileInfo.exists())
        return false;

      if (load_file(path.toUtf8().data(), in, ec) != 0)
        return false;

      lazy_entry e;
      if (lazy_bdecode(&in[0], &in[0] + in.size(), e, ec) != 0) 
        return false;

      lazy_entry const* info = e.dict_find_dict("info");    
      libtorrent::hasher h;
      std::pair<char const*, int> section = info->data_section();
      h.update(section.first, section.second);
      
      sha1_hash infoHash = h.final();
      
      std::string str = to_hex(infoHash.to_string());
      result = QString::fromStdString(str);
      return true;
    }

    bool WrapperInternal::getFileList(const QString& path, QList<QString> &result)
    {
      error_code ec;
      torrent_info *torrentInfo = new torrent_info(path.toUtf8().data(), ec);
      if (ec) {
        QString str = QString::fromLocal8Bit(ec.message().c_str());
        WARNING_LOG << "Can't create torrent info from file" << path << "with reasons" << str;
        return false;
      }

      QString torrentName = QString::fromStdString(torrentInfo->name());

      int count = torrentInfo->num_files();
      for (int i = 0; i < count; ++i) {
        std::string name = torrentInfo->file_at(i).path;
        QString fileName = QString::fromUtf8(name.c_str(), name.size());
        fileName = fileName.right(fileName.length() - torrentName.length() - 1);
        result.append(fileName);
      }

      return true;
    }

  }
}
