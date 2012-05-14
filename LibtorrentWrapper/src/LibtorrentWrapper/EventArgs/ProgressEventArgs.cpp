/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include <LibtorrentWrapper/EventArgs/ProgressEventArgs>

namespace GGS 
{
  namespace Libtorrent
  {
    namespace EventArgs 
    {
      ProgressEventArgs::ProgressEventArgs(QObject *parrent)
        : QObject(parrent)
      {
      }

      ProgressEventArgs::ProgressEventArgs(const ProgressEventArgs &p)
        : _progress(p._progress),
        _status(p._status),
        _id(p._id),
        _uploadRate(p._uploadRate),
        _downloadRate(p._downloadRate),
        _totalWanted(p._totalWanted),
        _totalWantedDone(p._totalWantedDone)
      {

      }

      ProgressEventArgs::~ProgressEventArgs()
      {
      }
    }
  }
}
