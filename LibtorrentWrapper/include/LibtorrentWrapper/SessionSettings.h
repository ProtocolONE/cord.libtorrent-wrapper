#pragma once
#include <libtorrent/session.hpp>

namespace GGS {
  namespace Libtorrent {
    libtorrent::session_settings minMemoryUsage();
    libtorrent::session_settings highPerformanceSeed();
    libtorrent::session_settings defaultProfile();
  }
}
