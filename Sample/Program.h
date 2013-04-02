#ifndef _GGS_SAMPLE_PROGRAM_H_
#define _GGS_SAMPLE_PROGRAM_H_

#include <LibtorrentWrapper/Wrapper>
#include <LibtorrentWrapper/TorrentConfig>
#include <LibtorrentWrapper/EventArgs/ProgressEventArgs>

#include <QtCore/QObject>
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtCore/QDebug>
#include <QtCore/QTextStream>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtWidgets/QMainWindow>

class Program : public QMainWindow
{
  Q_OBJECT

  Q_PROPERTY(int progress1 READ progress1 NOTIFY progress1Changed)
  Q_PROPERTY(int progress2 READ progress2 NOTIFY progress2Changed)

  Q_PROPERTY(QString statusText1 READ statusText1 NOTIFY statusText1Changed)
  Q_PROPERTY(QString statusText2 READ statusText2 NOTIFY statusText2Changed)

  Q_PROPERTY(QString port READ port WRITE setPort NOTIFY portChanged)

public:
  Program();
  ~Program();

  int progress1() const { return this->_progress1; }
  int progress2() const { return this->_progress2; }

  QString statusText1() const { return this->_statusText1; }
  QString statusText2() const { return this->_statusText2; }

  QString port() const { return this->_port; }
public slots:
  void setPort(QString port) {
    if(this->_port != port) {
      this->_port = port;
      emit this->portChanged();
    }
  }

    void start();
    void reloadSameTorrent();
    
    void quit();
    void beforeShutdown();
    
    void startTorrent(QString id);
    void stopTorrent(QString id);
    void restartTorrent(QString id);

    void torrentProgress(GGS::Libtorrent::EventArgs::ProgressEventArgs args);
    void torrentFileError(QString id, QString path, int errorCode);
    void torrentStatusChanged(QString id, GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus oldState, GGS::Libtorrent::EventArgs::ProgressEventArgs::TorrentStatus newState);
    void torrentDownloadFinished(QString id);
    void torrentPaused(QString id);

    void torrentChangePort();

    void torrentListeningPortChanged(unsigned short newPort);

    void setUploadRateLimit(QString bytesPerSecond);
    void setDownloadRateLimit(QString bytesPerSecond);

signals:
    void progress1Changed();
    void progress2Changed();

    void statusText1Changed();
    void statusText2Changed();

    void portChanged();

private:
  int _progress1;
  int _progress2;

  QString _port;

  QString _statusText1;
  QString _statusText2;

  QDeclarativeView *nQMLContainer;
  GGS::Libtorrent::Wrapper _wrapper;
  GGS::Libtorrent::TorrentConfig _config;

  void setProgress1(int progress) {
    if(this->_progress1 != progress) {
      this->_progress1 = progress;
      emit this->progress1Changed();
    }
  }

  void setProgress2(int progress) {
    if(this->_progress2 != progress) {
      this->_progress2 = progress;
      emit this->progress2Changed();
    }
  }

  void setStatusText1(QString status) {
    if(this->_statusText1 != status) {
      this->_statusText1 = status;
      emit this->statusText1Changed();
    }
  }

  void setStatusText2(QString status) {
    if(this->_statusText2 != status) {
      this->_statusText2 = status;
      emit this->statusText2Changed();
    }
  }


};

#endif