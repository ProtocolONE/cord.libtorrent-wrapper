#include <LibtorrentWrapper/EventArgs/ProgressEventArgs.h>

#include <gtest/gtest.h>
#include <QtCore/QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qRegisterMetaType<P1::Libtorrent::EventArgs::ProgressEventArgs>();

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
