#include <LibtorrentWrapper/EventArgs/ProgressEventArgs>

#include "gtest/gtest.h"
#include <QtCore/QCoreApplication>

using namespace GGS::Libtorrent::EventArgs;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qRegisterMetaType<ProgressEventArgs>();

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
