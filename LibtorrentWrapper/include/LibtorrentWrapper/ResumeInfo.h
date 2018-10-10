#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

namespace P1 {
  namespace Libtorrent {

    class ResumeInfo : public QObject
    {
      Q_OBJECT
    public:
      ResumeInfo(QObject *parent = 0);
      ResumeInfo(const ResumeInfo &info);

      ~ResumeInfo();

      ResumeInfo& operator =(const ResumeInfo& info);
      
      const QString& id() const;
      void setId(const QString& value);

      const QString& savePath() const;
      void setSavePath(const QString& value);

      const QString& torrentPath() const;
      void setTorrentPath(const QString& value);

      bool finished() const;
      void setFinished(bool value);    
    
    private:
      QString _id;
      QString _savePath;
      QString _torrentPath;
      bool _finished;

    };
  }
}