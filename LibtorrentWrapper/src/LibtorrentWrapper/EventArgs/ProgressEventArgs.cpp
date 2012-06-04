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

      void ProgressEventArgs::setProgress( float progress )
      {
        this->_progress = progress;
      }

      float ProgressEventArgs::progress() const
      {
        return this->_progress;
      }

      void ProgressEventArgs::setStatus( TorrentStatus status )
      {
        this->_status = status;
      }

      GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus ProgressEventArgs::status() const
      {
        return this->_status;
      }

      void ProgressEventArgs::setId( const QString& id )
      {
        this->_id = id;
      }

      const QString& ProgressEventArgs::id() const
      {
        return this->_id;
      }

      void ProgressEventArgs::setDownloadRate( int downloadRate )
      {
        this->_downloadRate = downloadRate;
      }

      int ProgressEventArgs::downloadRate() const
      {
        return this->_downloadRate;
      }

      void ProgressEventArgs::setUploadRate( int uploadRate )
      {
        this->_uploadRate = uploadRate;
      }

      int ProgressEventArgs::uploadRate() const
      {
        return this->_uploadRate;
      }

      void ProgressEventArgs::setTotalWanted( qint64 totalWanted )
      {
        this->_totalWanted = totalWanted;
      }

      qint64 ProgressEventArgs::totalWanted() const
      {
        return this->_totalWanted;
      }

      void ProgressEventArgs::setTotalWantedDone( qint64 totalWantedDone )
      {
        this->_totalWantedDone = totalWantedDone;
      }

      qint64 ProgressEventArgs::totalWantedDone() const
      {
        return this->_totalWantedDone;
      }

    }
  }
}
