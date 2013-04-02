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
				_totalWantedDone(p._totalWantedDone),
				_directTotalDownload(p._directTotalDownload),
				_peerTotalDownload(p._peerTotalDownload), 

				_payloadTotalDownload(p._payloadTotalDownload),
				_peerPayloadDownloadRate(p._peerPayloadDownloadRate), 
				_payloadDownloadRate(p._payloadDownloadRate), 
				_directPayloadDownloadRate(p._directPayloadDownloadRate),
				_playloadUploadRate(p._playloadUploadRate),
				_totalPayloadUpload(p._totalPayloadUpload)
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

			void ProgressEventArgs::setPayloadTotalDownload(qint64 totalDownload) 
			{
				this->_payloadTotalDownload = totalDownload;
			}

			qint64 ProgressEventArgs::payloadTotalDownload() const 
			{
				return this->_payloadTotalDownload;
			}

			void ProgressEventArgs::setPayloadDownloadRate(int payloadDownloadRate) 
			{
				this->_payloadDownloadRate = payloadDownloadRate;
			}

			int ProgressEventArgs::payloadDownloadRate() const 
			{
				return this->_payloadDownloadRate;
			}

			void ProgressEventArgs::setPeerPayloadDownloadRate(int payloadDownloadRate) 
			{
				this->_peerPayloadDownloadRate = payloadDownloadRate;
			}

			int ProgressEventArgs::peerPayloadDownloadRate() const 
			{
				return this->_peerPayloadDownloadRate;
			}

			void ProgressEventArgs::setDirectPayloadDownloadRate(int downloadRate) 
			{
				this->_directPayloadDownloadRate = downloadRate;
			}

			int ProgressEventArgs::directPayloadDownloadRate() const 
			{
				return this->_directPayloadDownloadRate;
			}

			void ProgressEventArgs::setPayloadUploadRate(int payloadUploadRate) 
			{
				this->_playloadUploadRate = payloadUploadRate;
			}

			int ProgressEventArgs::payloadUploadRate() const
			{
				return this->_playloadUploadRate;
			}

			void ProgressEventArgs::setTotalPayloadUpload(qint64 totalUpload) 
			{
				this->_totalPayloadUpload = totalUpload;
			}

			qint64 ProgressEventArgs::totalPayloadUpload() const 
			{
				return this->_totalPayloadUpload;
			}

			void ProgressEventArgs::setDirectTotalDownload(qint64 totalDownload) 
			{
				this->_directTotalDownload = totalDownload;
			}

			qint64 ProgressEventArgs::directTotalDownload() const 
			{
				return this->_directTotalDownload;
			}

			void ProgressEventArgs::setPeerTotalDownload(qint64 totalDownload) 
			{
				this->_peerTotalDownload = totalDownload;
			}

			qint64 ProgressEventArgs::peerTotalDownload() const 
			{
				return this->_peerTotalDownload;
			}
		}
	}
}