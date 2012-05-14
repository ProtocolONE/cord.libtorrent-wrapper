#include <LibtorrentWrapper/EventArgs/ProgressEventArgs>

#include "gtest/gtest.h"
#include <QtCore/QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qRegisterMetaType<GGS::Libtorrent::EventArgs::ProgressEventArgs>();

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
