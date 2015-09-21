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
#include <libtorrent/storage.hpp>

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
      this->_sessionsSettings.dont_count_slow_torrents = false; //!
      this->_sessionsSettings.ban_web_seeds = false; //!
      this->_sessionsSettings.allow_reordered_disk_operations = true; //!
      this->_sessionsSettings.no_connect_privileged_ports = true; //! DDOS safe, true default
      this->_sessionsSettings.lock_files = false; //! We can`t disable it due security reasons


      this->_sessionsSettings.prefer_udp_trackers = true; //! In out config false, using tcp is prefered
      this->_sessionsSettings.rate_limit_utp = true; //! Should be true, we limit speed in config of app

      this->_sessionsSettings.tick_interval = 80; //! Default is 100ms

      //! Default 3 and 2 is upload 2 times great then download. Can`t be less then 2.
      this->_sessionsSettings.share_mode_target = 2; 

      //! But may be bittyrant_choker would be better
      this->_sessionsSettings.choking_algorithm = session_settings::fixed_slots_choker; 

      this->_sessionsSettings.torrent_connect_boost = 30;  //! Bet  ter warm up downloading, DO NOT DISABLE!!!
      this->_sessionsSettings.utp_num_resends = 4; //! Need to disable ddos user lan

      this->_sessionsSettings.allow_multiple_connections_per_ip = true;

      //don't retry peers if they fail once. Let them connect to us if they want to
      this->_sessionsSettings.max_failcount = 1; 
      
      // don't let connections linger for too long
      this->_sessionsSettings.min_reconnect_time = 30; //! Default 60
      this->_sessionsSettings.peer_connect_timeout = 5; //! Default 10
      this->_sessionsSettings.peer_timeout = 20; //!
      this->_sessionsSettings.inactivity_timeout = 20;  //Was 180
      this->_sessionsSettings.request_timeout = 10; //! Was 20
      this->_sessionsSettings.mixed_mode_algorithm = session_settings::prefer_tcp;
     
      this->_sessionsSettings.use_disk_cache_pool = true; //! the disk cache performs better with the pool allocator
      
      //! True by default and allow the buffer size to grow for the uTP socket
      this->_sessionsSettings.utp_dynamic_sock_buf = true; 
      this->_sessionsSettings.anonymous_mode = false;

      this->_sessionsSettings.unchoke_slots_limit = 500; //! Was 20, unchoke many peers
      this->_sessionsSettings.connection_speed = 50; //Was 30

      this->_sessionsSettings.handshake_timeout = 3; //! App specific, much of peer is offline
      this->_sessionsSettings.connections_slack = 50; //! Was 15

      // allow peers to request a lot of blocks at a time,
      // to be more likely to saturate the bandwidth-delay-
      // product.
      this->_sessionsSettings.max_allowed_in_request_queue = 2000;
      this->_sessionsSettings.max_out_request_queue = 1000;

      this->_sessionsSettings.file_pool_size = 500; //!By default
      this->_sessionsSettings.alert_queue_size = 5000; //! They prefer 50 000, was 1 000
            
      this->_sessionsSettings.connections_limit = 400; //! Was 200, they prefer 8 000 (!!!)
      this->_sessionsSettings.listen_queue_size = 200; //! // allow lots of peers to try to connect simultaneously

      //! Was 4000, but we need more DHT capacity to ping more peers candidates before trying to connect
      this->_sessionsSettings.dht_upload_rate_limit = 20000; 

      // we're more interested in downloading than seeding only service a read job every 1000 write job (when
      // disk is congested). Presumably on a big box, writes are extremely cheap and reads are relatively expensive
      // so that's the main reason this ratio should be adjusted
      this->_sessionsSettings.read_job_every = 100; //! ONE OF MOST IMPORTANT!!!
      this->_sessionsSettings.use_read_cache = true;
      this->_sessionsSettings.cache_buffer_chunk_size = 128;
      this->_sessionsSettings.read_cache_line_size = 32;
      this->_sessionsSettings.write_cache_line_size = 32;

      this->_sessionsSettings.cache_size = 1024 * 4; //! We use 64mb, they 1gb, default is 16mb
      this->_sessionsSettings.low_prio_disk = true; //! True in they config, NEVER SET FALSE!!!
      this->_sessionsSettings.cache_expiry = 60 * 60;

      // this is expensive and could add significant delays when freeing a large number of buffers
      this->_sessionsSettings.lock_disk_cache = false;

      //! the max number of bytes pending write before we throttle download rate, default is 1024 * 1024
      this->_sessionsSettings.max_queued_disk_bytes = 10 * 1024 * 1024; 

      //! It`s default. Flush write cache in a way to minimize the amount we need to read back once we want to hash-check the piece.
      // i.e. try to flush all blocks in-order
      this->_sessionsSettings.disk_cache_algorithm = session_settings::avoid_readback;
      this->_sessionsSettings.explicit_read_cache = 0;

      //! Was 10, now 0. Prevent fast pieces to interfere with suggested pieces since we unchoke everyone,
      // we don't need fast pieces anyway
      this->_sessionsSettings.allowed_fast_set_size = 0;
      
      //! suggest pieces in the read cache for higher cache hit rate
      this->_sessionsSettings.suggest_mode = session_settings::suggest_read_cache;

      this->_sessionsSettings.close_redundant_connections = true; //True by default
      this->_sessionsSettings.max_rejects = 10; //By default

      //! Next 2 default is 16 * 1024, specifies the buffer sizes set on peer sockets
      this->_sessionsSettings.recv_socket_buffer_size = 1024 * 1024;
      this->_sessionsSettings.send_socket_buffer_size = 1024 * 1024;

      //! We do not use auto managed torrents so it`s just copy paste from thet prefer config.
      this->_sessionsSettings.active_limit = 2000;
      this->_sessionsSettings.active_tracker_limit = 2000;
      this->_sessionsSettings.active_dht_limit = 600;
      this->_sessionsSettings.active_seeds = 2000;

      //! in order to be able to deliver very high upload rates, this should be able to cover the bandwidth delay
      // product. Assuming an RTT of 500 ms, and a send rate of 20 MB/s, the upper limit should be 10 MB. 
      // DEFAULT WAS 10 kb !!!
      this->_sessionsSettings.send_buffer_watermark = 3 * 1024 * 1024;

      //! put 1.5 seconds worth of data in the send buffer this gives the disk I/O more heads-up on disk reads, and 
      // can maximize throughput.
      // DEFAULT IS 50 !!!
      this->_sessionsSettings.send_buffer_watermark_factor = 150;
      
      //! always stuff at least 1 MiB down each peer pipe, to quickly ramp up send rates
      this->_sessionsSettings.send_buffer_low_watermark = 1 * 1024 * 1024;
      
      //! max 'bottled' http receive buffer/url torrent size, was 4 mb
      this->_sessionsSettings.max_http_recv_buffer_size = 6 * 1024 * 1024;
      
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
      
      this->_session->start_dht();
      this->_session->add_dht_router(std::make_pair(std::string("router.bittorrent.com"), 6881));
      this->_session->add_dht_router(std::make_pair(std::string("router.utorrent.com"), 6881));
      this->_session->add_dht_router(std::make_pair(std::string("router.bitcomet.com"), 6881));
      
      error_code ec;
      this->_session->listen_on(std::make_pair(this->_startupListeningPort, this->_startupListeningPort), ec);
      if (ec) {
        DEBUG_LOG << "can't listen on " << this->_startupListeningPort << " error code " << ec;
        emit this->listenFailed(this->_startupListeningPort, ec.value());
      }
           

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

    void WrapperInternal::createFastResume(const QString& id, TorrentConfig& config)
    {
      error_code ec;
      torrent_info torrentInfo(config.pathToTorrentFile().toUtf8().data(), ec);
      
      if (ec) {
        WARNING_LOG << "Can't create torrent info from file" << config.pathToTorrentFile() 
          << "with reasons" << QString::fromLocal8Bit(ec.message().c_str());
        return;
      }

      libtorrent::entry entry;
      entry["file-format"] = "libtorrent resume file";
      entry["file-version"] = 1;
      entry["libtorrent-version"] = LIBTORRENT_VERSION;
      entry["sequential_download"] = 0;
      entry["save_path"] = config.downloadPath().toUtf8().data();
      entry["info-hash"] = torrentInfo.info_hash().to_string();
            
      std::vector<std::pair<size_type, std::time_t>> fileSizes 
        = libtorrent::get_filesizes(torrentInfo.files(), config.downloadPath().toStdString());

      libtorrent::entry::list_type& fl = entry["file sizes"].list();
      for (std::vector<std::pair<size_type, std::time_t> >::iterator i
        = fileSizes.begin(), end(fileSizes.end()); i != end; ++i)
      {
        libtorrent::entry::list_type p;
        p.push_back(libtorrent::entry(i->first));
        p.push_back(libtorrent::entry(i->second));
        fl.push_back(libtorrent::entry(p));
      }

      libtorrent::entry::string_type& pieces = entry["pieces"].string();
      pieces.resize(torrentInfo.num_pieces());
      std::memset(&pieces[0], 1, pieces.size());
      
      this->saveFastResumeEntry(this->getFastResumeFilePath(id), entry);
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
          if (tracker_error_alert* p = alert_cast<tracker_error_alert>(alertObject.get()))
              this->_errorNotificationHandler(*p);
          else if (tracker_warning_alert* p = alert_cast<tracker_warning_alert>(alertObject.get()))
              this->_errorNotificationHandler(*p);
          else if (scrape_failed_alert* p = alert_cast<scrape_failed_alert>(alertObject.get()))
              this->_errorNotificationHandler(*p);
          else if (torrent_delete_failed_alert* p = alert_cast<torrent_delete_failed_alert>(alertObject.get()))
              this->_errorNotificationHandler(*p);
          if (save_resume_data_failed_alert* p = alert_cast<save_resume_data_failed_alert>(alertObject.get()))
              this->_errorNotificationHandler(*p);
          else if (url_seed_alert* p = alert_cast<url_seed_alert>(alertObject.get()))
              this->_errorNotificationHandler(*p);

          else if (file_error_alert* p = alert_cast<file_error_alert>(alertObject.get()))
              this->_errorNotificationHandler(*p);
          else if (metadata_failed_alert* p = alert_cast<metadata_failed_alert>(alertObject.get()))
              this->_errorNotificationHandler(*p);
          else if (udp_error_alert* p = alert_cast<udp_error_alert>(alertObject.get()))
              this->_errorNotificationHandler(*p);
          else if (listen_failed_alert* p = alert_cast<listen_failed_alert>(alertObject.get()))
              this->_errorNotificationHandler(*p);
          else if (portmap_error_alert* p = alert_cast<portmap_error_alert>(alertObject.get()))
              this->_errorNotificationHandler(*p);
          else if (fastresume_rejected_alert* p = alert_cast<fastresume_rejected_alert>(alertObject.get()))
              this->_errorNotificationHandler(*p);
          else if (torrent_error_alert* p = alert_cast<torrent_error_alert>(alertObject.get()))
              this->_errorNotificationHandler(*p);

          else if (state_changed_alert* p = alert_cast<state_changed_alert>(alertObject.get()))
              this->_statusNotificationHandler(*p);
          else if (hash_failed_alert* p = alert_cast<hash_failed_alert>(alertObject.get()))
              this->_statusNotificationHandler(*p);
          else if (torrent_finished_alert* p = alert_cast<torrent_finished_alert>(alertObject.get()))
              this->_statusNotificationHandler(*p);
          else if (torrent_paused_alert* p = alert_cast<torrent_paused_alert>(alertObject.get()))
              this->_statusNotificationHandler(*p);
          else if (torrent_resumed_alert* p = alert_cast<torrent_resumed_alert>(alertObject.get()))
              this->_statusNotificationHandler(*p);
          else if (torrent_checked_alert* p = alert_cast<torrent_checked_alert>(alertObject.get()))
              this->_statusNotificationHandler(*p);
          else if (metadata_received_alert* p = alert_cast<metadata_received_alert>(alertObject.get()))
              this->_statusNotificationHandler(*p);
          else if (external_ip_alert* p = alert_cast<external_ip_alert>(alertObject.get()))
              this->_statusNotificationHandler(*p);
          else if (listen_succeeded_alert* p = alert_cast<listen_succeeded_alert>(alertObject.get()))
              this->_statusNotificationHandler(*p);
          else if (torrent_added_alert* p = alert_cast<torrent_added_alert>(alertObject.get()))
              this->_statusNotificationHandler(*p);
          else if (trackerid_alert* p = alert_cast<trackerid_alert>(alertObject.get()))
              this->_statusNotificationHandler(*p);
          else if (torrent_removed_alert* p = alert_cast<torrent_removed_alert>(alertObject.get()))
              this->_statusNotificationHandler(*p);

          else if (tracker_announce_alert* p = alert_cast<tracker_announce_alert>(alertObject.get()))
              this->_trackerNotificationHandler(*p);
          else if (scrape_reply_alert* p = alert_cast<scrape_reply_alert>(alertObject.get()))
              this->_trackerNotificationHandler(*p);
          else if (tracker_reply_alert* p = alert_cast<tracker_reply_alert>(alertObject.get()))
              this->_trackerNotificationHandler(*p);
          else if (dht_reply_alert* p = alert_cast<dht_reply_alert>(alertObject.get()))
              this->_trackerNotificationHandler(*p);

          else if (read_piece_alert* p = alert_cast<read_piece_alert>(alertObject.get()))
              this->_storageNotificationHandler(*p);
          else if (file_renamed_alert* p = alert_cast<file_renamed_alert>(alertObject.get()))
              this->_storageNotificationHandler(*p);
          else if (file_rename_failed_alert* p = alert_cast<file_rename_failed_alert>(alertObject.get()))
              this->_storageNotificationHandler(*p);
          else if (storage_moved_alert* p = alert_cast<storage_moved_alert>(alertObject.get()))
              this->_storageNotificationHandler(*p);
          else if (storage_moved_failed_alert* p = alert_cast<storage_moved_failed_alert>(alertObject.get()))
              this->_storageNotificationHandler(*p);
          else if (torrent_deleted_alert* p = alert_cast<torrent_deleted_alert>(alertObject.get()))
              this->_storageNotificationHandler(*p);
          else if (save_resume_data_alert* p = alert_cast<save_resume_data_alert>(alertObject.get()))
              this->_storageNotificationHandler(*p);
          else if (cache_flushed_alert* p = alert_cast<cache_flushed_alert>(alertObject.get()))
              this->_storageNotificationHandler(*p);
          else {
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

    int WrapperInternal::loadFile(std::string const& filename, std::vector<char>& v, error_code& ec, int limit)
    {
        ec.clear();

        file f;
        if (!f.open(filename, file::read_only, ec)) 
          return -1;

        size_type s = f.get_size(ec);

        if (ec) 
          return -1;

        if (s > limit) {
            ec = error_code(errors::metadata_too_large, get_libtorrent_category());
            return -2;
        }

        v.resize(s);

        if (s == 0) 
          return 0;

        file::iovec_t b = {&v[0], s};
        size_type read = f.readv(0, &b, 1, ec);

        if (read != s) 
          return -3;

        if (ec) 
          return -3;

        return 0;
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
        if (this->loadFile(resumeFilePath.toUtf8().data(), buf, ec) == 0)
          p.resume_data = buf;
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
      this->saveFastResumeEntry(resumeFilePath, *resumeData);
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

      entry::dictionary_type& resume = sessionState["GGSResumeInfo"].dict();
      Q_FOREACH(ResumeInfo resumeInfo, this->_resumeInfo) {
        if (!resumeInfo.finished())
          continue;
          
        std::string id(resumeInfo.id().toUtf8());
        std::string torrentPath(resumeInfo.torrentPath().toUtf8());
        std::string savePath(resumeInfo.savePath().toUtf8());
        
        entry::dictionary_type& resumeService = resume[id].dict();
        resumeService["torrentPath"].string() = torrentPath;
        resumeService["savePath"].string() = savePath;
      }

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

      if (this->loadFile(sessionStatePath.toUtf8().data(), in, ec) != 0) {
        CRITICAL_LOG << "Can't load session state from" << sessionStatePath <<"with error code:" << ec;
        return;
      }

      if (in.size() == 0)
        return;

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

      if (this->loadFile(path.toUtf8().data(), in, ec) != 0)
        return false;

      if (in.size() == 0)
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

    void WrapperInternal::saveFastResumeEntry(const QString &resumeFilePath, const libtorrent::entry &resumeData)
    {
      this->createDirectoryIfNotExists(resumeFilePath);
      try {
        const wchar_t *resumePath = reinterpret_cast<const wchar_t*>(resumeFilePath.utf16());
        boost::filesystem::ofstream out(resumePath, std::ios_base::binary);
        out.unsetf(std::ios_base::skipws);
        bencode(std::ostream_iterator<char>(out), resumeData);
        out.close();
      } catch (boost::filesystem::filesystem_error& err) {
        DEBUG_LOG << err.what();
      } catch (std::exception& stdExc) {
        DEBUG_LOG << stdExc.what();
      }
    }
  }
}
