/****************************************************************************
** This file is a part of Syncopate Limited GameNet Application or it parts.
**
** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates.
** All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#ifndef _GGS_LIBTORRENT_WRAPPER_H_
#define _GGS_LIBTORRENT_WRAPPER_H_

#include <LibtorrentWrapper/libtorrentwrapper_global>
#include <LibtorrentWrapper/TorrentConfig>
#include <LibtorrentWrapper/EventArgs/ProgressEventArgs>

#include <QtCore/QObject>

namespace GGS {
  namespace Libtorrent
  {
    class WrapperInternal;
    using EventArgs::ProgressEventArgs;
    class LIBTORRENTWRAPPER_EXPORT Wrapper : public QObject
    {
      Q_OBJECT
    public:
      Wrapper();
      ~Wrapper();


      /// <summary>Инициализирует торрент движок. Необходимо выхвать 1 раз перед использованием.</summary>
      /// <remarks>Ilya.Tkachenko, 02.04.2012.</remarks>
      void initEngine();


      /// <summary>Скачать торрент. Где взять торрент файл и куда качать передается через TorrentConfig.
      /// 				 Id - идентификатор торрента. По нему будут приходить сигнал и с мпомщью него можно остановить торрент.
      /// 				 Если такой торрент уже есть, то закачка будет, либо продолжена, либо торрент начнется снова, 
      /// 				 в зависимости от настроек.</summary>
      /// <remarks>Ilya.Tkachenko, 02.04.2012.</remarks>
      /// <param name="id">    Id торрента.</param>
      /// <param name="config">Настройки добавляемого торрента.</param>
      void start(const QString& id, TorrentConfig& config);


      /// <summary>Останавливает торрент. Торрент переходит в режим paused.</summary>
      /// <remarks>Ilya.Tkachenko, 02.04.2012.</remarks>
      /// <param name="id">Id торрента.</param>
      void stop(const QString& id);


      /// <summary>Закрывает сессию и все зарегистрированные торренты. Происходит сохранение всех фастрезьюмов. 
      /// 				 Функция может работать до 30 секунд.</summary>
      /// <remarks>Ilya.Tkachenko, 02.04.2012.</remarks>
      void shutdown();


      /// <summary>Удаляет торрент из списка. Сохранение fast resume не происходит, поэтому, если удалять запущеный 
      /// 				 торрент, то fast resume может быть испорчен или не актуален.</summary>
      /// <remarks>Ilya.Tkachenko, 02.04.2012.</remarks>
      /// <param name="id">Id торрента.</param>
      void remove(const QString& id);


      /// <summary>Устанавливает путь до папки, в которую созхраняются файлы fastresume и настройки сессии.</summary>
      /// <remarks>Ilya.Tkachenko, 10.04.2012.</remarks>
      /// <param name="path">Полный путь до папки с настрйоками.</param>
      void setTorrentConfigDirectoryPath(const QString& path);

      /// <summary>Задает порт, с которым стартует движек торрента. Вызывать его необходимо только до initEngine 
      /// 				 Вызов этого метода не меняет порт уже запущеного 
      /// 				 движка. Для смены порта следует использовать метод changeListeningPort.</summary>
      /// <remarks>Ilya.Tkachenko, 02.04.2012.</remarks>
      /// <param name="port">The port.</param>
      void setListeningPort(unsigned short port);


      /// <summary>Торрент закрывает текущий прослушиваемый порт и пытается открыть указанный. В случаи неудачи торрент
      /// 				  пытается открыть на 0 порту, тем самым система выдает любой свободный.</summary>
      /// <remarks>Ilya.Tkachenko, 02.04.2012.</remarks>
      /// <param name="port">The port.</param>
      void changeListeningPort(unsigned short port);


      /// <summary>Gets the listening port.</summary>
      /// <remarks>Ilya.Tkachenko, 03.04.2012.</remarks>
      /// <returns>.</returns>
      unsigned short listeningPort() const;


      /// <summary>Sets a upload rate limit.</summary>
      /// <remarks>Ilya.Tkachenko, 05.04.2012.</remarks>
      /// <param name="bytesPerSecond">The bytes per second.</param>
      void setUploadRateLimit(int bytesPerSecond);


      /// <summary>Sets a download rate limit.</summary>
      /// <remarks>Ilya.Tkachenko, 05.04.2012.</remarks>
      /// <param name="bytesPerSecond">The bytes per second.</param>
      void setDownloadRateLimit(int bytesPerSecond);


      /// <summary>Uploads the rate limit.</summary>
      /// <remarks>Ilya.Tkachenko, 05.04.2012.</remarks>
      /// <returns>.</returns>
      int uploadRateLimit() const;


      /// <summary>Downloads the rate limit.</summary>
      /// <remarks>Ilya.Tkachenko, 05.04.2012.</remarks>
      /// <returns>.</returns>
      int downloadRateLimit() const;

    signals:

      /// <summary>Listening port changed. Do not call before InitEngine.</summary>
      /// <remarks>Ilya.Tkachenko, 02.04.2012.</remarks>
      /// <param name="port">The port.</param>
      void listeningPortChanged(unsigned short port);

      /// <summary>Progress.</summary>
      /// <remarks>Ilya.Tkachenko, 02.04.2012.</remarks>
      /// <param name="args">Progress event information.</param>
      void progressChanged(ProgressEventArgs args);


      /// <summary>Трекер вернул ошибку.</summary>
      /// <remarks>Ilya.Tkachenko, 02.04.2012.</remarks>
      /// <param name="id">             Id торрента.</param>
      /// <param name="failCountInARow">Количество ошибок подряд.</param>
      /// <param name="httpStatusCode"> Http код ответа от трекера.</param>
      void trackerFailed(QString id, int failCountInARow, int httpStatusCode);

      /// <summary>Ошибка чтения/записи файла.</summary>
      /// <remarks>Ilya.Tkachenko, 30.03.2012.</remarks>
      /// <param name="id">       Id торрента.</param>
      /// <param name="filePath"> Полный путь до файла.</param>
      /// <param name="errorCode">Код ошибки. Откуда брать расшифровку пока не совсем понятно. 
      /// 												Коды похожи на те что указаны в ec_xlate (%BOOST_ROOT%\interprocess\errors.hpp). 
      /// 												Например, 112 - нехватка места на диске - ERROR_DISK_FULL</param>
      void fileError(QString id, QString filePath, int errorCode);


      /// <summary>Listen fail.</summary>
      /// <remarks>Ilya.Tkachenko, 02.04.2012.</remarks>
      /// <param name="port">     The port.</param>
      /// <param name="errorCode"> Код ошибки. ОТкуда брать расшифровку пока не совсем понятно. Коды
      ///   похожи на те что указаны в ec_xlate (%BOOST_ROOT%\interprocess\errors.hpp). Например, 112 -
      ///   нехватка места на диске - ERROR_DISK_FULL.</param>
      void listenFailed(int port, int errorCode);


      /// <summary>Torrent status changed.</summary>
      /// <remarks>Ilya.Tkachenko, 02.04.2012.</remarks>
      /// <param name="id">      Id торрента.</param>
      /// <param name="oldState">State of the old.</param>
      /// <param name="newState">State of the new.</param>
      void torrentStatusChanged(QString id, ProgressEventArgs::TorrentStatus oldState, ProgressEventArgs::TorrentStatus newState);


      /// <summary>Torrent download finished.</summary>
      /// <remarks>Ilya.Tkachenko, 02.04.2012.</remarks>
      /// <param name="id">Id торрента.</param>
      void torrentDownloadFinished(QString id);


      /// <summary>Torrent resumed.</summary>
      /// <remarks>Ilya.Tkachenko, 02.04.2012.</remarks>
      /// <param name="id">Id торрента.</param>
      void torrentResumed(QString id);

    private:
      WrapperInternal *_internalWrapper;

    };

  }
}
#endif // _GGS_LIBTORRENT_WRAPPER_H_
