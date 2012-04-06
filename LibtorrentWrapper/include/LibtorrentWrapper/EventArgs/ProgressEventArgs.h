/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#ifndef _GGS_LIBTORRENT_PROGRESSEVENTARGS_H_
#define _GGS_LIBTORRENT_PROGRESSEVENTARGS_H_

#include <LibtorrentWrapper/libtorrentwrapper_global>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QMetaObject>

namespace GGS 
{
  namespace Libtorrent
  {
    namespace EventArgs 
    {
      class LIBTORRENTWRAPPER_EXPORT ProgressEventArgs : public QObject
      {
        Q_OBJECT
          Q_ENUMS(TorrentStatus)
      public:
        ProgressEventArgs(QObject *parrent = 0);
        ProgressEventArgs(const ProgressEventArgs &p);
        ~ProgressEventArgs();

        enum TorrentStatus {
          QueuedForChecking,
          CheckingFiles,
          DownloadingMetadata,
          Downloading,
          Finished,
          Seeding,
          Allocating,
          CheckingResumeData
        };

        void setProgress(float progress) { this->_progress = progress; }
        float progress() const { return this->_progress; }

        void setStatus(TorrentStatus status) { this->_status = status; }
        TorrentStatus status() const { return this->_status; }

        void setId(const QString& id) { this->_id = id; }
        const QString& id() const { return this->_id; }

        void setDownloadRate(int downloadRate) { this->_downloadRate = downloadRate; }
        int downloadRate() const { return this->_downloadRate; }

        void setUploadRate(int uploadRate) { this->_uploadRate = uploadRate; }
        int uploadRate() const { return this->_uploadRate; }

        void setTotalWanted(qint64 totalWanted) { this->_totalWanted = totalWanted; }
        qint64 totalWanted() const { return this->_totalWanted; }

        void setTotalWantedDone(qint64 totalWantedDone) { this->_totalWantedDone = totalWantedDone; }
        qint64 totalWantedDone() const { return this->_totalWantedDone; }

      private:
        QString _id;
        TorrentStatus _status;
        float _progress;

        int _downloadRate;
        int _uploadRate;

        qint64 _totalWanted;
        qint64 _totalWantedDone;
      };

      Q_DECLARE_METATYPE(ProgressEventArgs);
    }
  }
}
#endif // _GGS_LIBTORRENT_PROGRESSEVENTARGS_H_