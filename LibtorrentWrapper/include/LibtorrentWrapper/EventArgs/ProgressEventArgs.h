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

      public slots:
        void setProgress(float progress);
        float progress() const;

        void setStatus(TorrentStatus status);
        TorrentStatus status() const;

        void setId(const QString& id);
        const QString& id() const;

        void setDownloadRate(int downloadRate);
        int downloadRate() const;

        void setUploadRate(int uploadRate);
        int uploadRate() const;

        void setTotalWanted(qint64 totalWanted);
        qint64 totalWanted() const;

        void setTotalWantedDone(qint64 totalWantedDone);
        qint64 totalWantedDone() const;

        void setPayloadTotalDownload(qint64 totalDownload);
        qint64 payloadTotalDownload() const;

        void setPayloadDownloadRate(int payloadDownloadRate);
        int payloadDownloadRate() const; 

        void setPeerPayloadDownloadRate(int payloadDownloadRate);
        int peerPayloadDownloadRate() const;

        void setDirectPayloadDownloadRate(int downloadRate);
        int directPayloadDownloadRate() const;

        void setPayloadUploadRate(int payloadUploadRate);
        int payloadUploadRate() const;

        void setTotalPayloadUpload(qint64 totalUpload);
        qint64 totalPayloadUpload() const;

        void setDirectTotalDownload(qint64 totalDownload);
        qint64 directTotalDownload() const;

        void setPeerTotalDownload(qint64 totalDownload); 
        qint64 peerTotalDownload() const;

      private:
        QString _id;
        TorrentStatus _status;
        float _progress;

        int _downloadRate;
        int _uploadRate;

        qint64 _totalWanted;
        qint64 _totalWantedDone;

        qint64 _directTotalDownload;
        qint64 _peerTotalDownload;

        qint64 _payloadTotalDownload;
        int _peerPayloadDownloadRate;
        int _payloadDownloadRate;
        int _directPayloadDownloadRate;
        int _playloadUploadRate;
        qint64 _totalPayloadUpload; 
      };

      Q_DECLARE_METATYPE(ProgressEventArgs);
    }
  }
}
#endif // _GGS_LIBTORRENT_PROGRESSEVENTARGS_H_