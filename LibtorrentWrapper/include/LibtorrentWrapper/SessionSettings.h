#pragma once
#include <libtorrent/session.hpp>

namespace P1 {
  namespace Libtorrent {
    libtorrent::session_settings minMemoryUsage();
    libtorrent::session_settings highPerformanceSeed();
    libtorrent::session_settings defaultProfile();
  }
}
