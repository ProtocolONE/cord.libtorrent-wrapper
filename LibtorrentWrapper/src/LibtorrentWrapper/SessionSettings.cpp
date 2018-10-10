#include <LibtorrentWrapper/SessionSettings.h>

#include <QtCore/QSysInfo>

namespace P1 {
  namespace Libtorrent {

    using namespace libtorrent;
  
    void updateCustomBehaviour(session_settings &settings) 
    {
      settings.user_agent = std::string("qgna/").append(LIBTORRENT_VERSION);

      settings.ban_web_seeds = false; 
      settings.lock_files = false; //! We can`t disable it due security reasons
      settings.prefer_udp_trackers = true; //! In out config false, using tcp is prefered
      settings.rate_limit_utp = true; //! Should be true, we limit speed in config of app
      settings.share_mode_target = 2; //! Default 3 and 2 is upload 2 times great then download. Can`t be less then 2.
      settings.torrent_connect_boost = 30;  //! Bet ter warm up downloading, DO NOT DISABLE!!! 10 by defaul.

      settings.tick_interval = 80; //! Default is 100ms

      // don't let connections linger for too long
      settings.min_reconnect_time = 30; //! Default 60
      settings.peer_connect_timeout = 10; //! Default 15

      settings.anonymous_mode = false;

      //! Was 4000, but we need more DHT capacity to ping more peers candidates before trying to connect
      settings.dht_upload_rate_limit = 20000;

      settings.handshake_timeout = 5; //! App specific, much of peer is offline
      settings.connections_slack = 20; //! Was 10

      // QGNA-278 Ограничим полуоткрытые для XP
      if (QSysInfo::windowsVersion() == QSysInfo::WV_XP)
        settings.half_open_limit = 5;
    }
 
    libtorrent::session_settings minMemoryUsage()
    {
      session_settings settings;

      settings.alert_queue_size = 2000;
      settings.max_allowed_in_request_queue = 200;

      // setting this to a low limit, means more
      // peers are more likely to request from the
      // same piece. Which means fewer partial
      // pieces and fewer entries in the partial
      // piece list
      settings.whole_pieces_threshold = 2;
      settings.use_parole_mode = false;
      settings.prioritize_partial_pieces = true;

      // connect to 5 peers per second
      settings.connection_speed = 15;

      // be extra nice on the hard drive when running
      // on embedded devices. This might slow down
      // torrent checking
      settings.file_checks_delay_per_block = 5;
      settings.file_pool_size = 15;

      // we want to keep the peer list as small as possible
      settings.allow_multiple_connections_per_ip = false;
      settings.max_failcount = 2;
      settings.inactivity_timeout = 120;
      settings.send_buffer_watermark = 50 * 1024;

      // use 1 GB of cache
      settings.cache_size = 1024;
      settings.use_read_cache = true;
      settings.cache_buffer_chunk_size = 16;
      settings.low_prio_disk = true;

      // one hour expiration
      settings.cache_expiry = 60 * 60;

      // this is expensive and could add significant
      // delays when freeing a large number of buffers
      settings.lock_disk_cache = false;

      settings.close_redundant_connections = true;
      settings.max_peerlist_size = 500;
      settings.max_paused_peerlist_size = 50;
      settings.max_rejects = 10;

      settings.recv_socket_buffer_size = 16 * 1024;
      settings.send_socket_buffer_size = 16 * 1024;
      settings.optimize_hashing_for_speed = false;

      settings.max_http_recv_buffer_size = 2 * 1024 * 1024;

      updateCustomBehaviour(settings);

      return settings;
    }

    libtorrent::session_settings highPerformanceSeed()
    {
      session_settings settings = high_performance_seed();
      updateCustomBehaviour(settings);      

      settings.cache_size = 8192 * 4; //! We use 256mb, they 1gb, default is 16mb

      return settings;
    }

    libtorrent::session_settings defaultProfile()
    {
      session_settings settings;
      updateCustomBehaviour(settings);

      return settings;
    }
  }
}