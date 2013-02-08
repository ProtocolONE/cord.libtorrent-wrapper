#include <LibtorrentWrapper/ResumeInfo.h>

namespace GGS {
  namespace Libtorrent {

    ResumeInfo::ResumeInfo(QObject *parent) 
      : QObject(parent)
      , _finished(false)
    {
    }

    ResumeInfo::ResumeInfo(const ResumeInfo &info)
    {
      this->operator=(info);
    }

    ResumeInfo::~ResumeInfo()
    {
    }

    const QString& ResumeInfo::savePath() const
    {
      return this->_savePath;
    }

    void ResumeInfo::setSavePath(const QString& value)
    {
      this->_savePath = value;
    }

    const QString& ResumeInfo::torrentPath() const
    {
      return this->_torrentPath;
    }

    void ResumeInfo::setTorrentPath(const QString& value)
    {
      this->_torrentPath = value;
    }

    const QString& ResumeInfo::id() const
    {
      return this->_id;
    }

    void ResumeInfo::setId(const QString& value)
    {
      this->_id = value;
    }

    bool ResumeInfo::finished() const
    {
      return this->_finished;
    }

    void ResumeInfo::setFinished(bool value)
    {
      this->_finished = value;
    }

    ResumeInfo& ResumeInfo::operator=(const ResumeInfo& info)
    {
      if (this == &info)
        return *this; // return on same object 

      this->_id = info._id;
      this->_savePath = info._savePath;
      this->_torrentPath = info._torrentPath;
      this->_finished = info._finished;

      return *this;
    }
  }
}