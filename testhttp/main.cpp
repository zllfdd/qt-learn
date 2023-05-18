#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include "httpdownloader.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    HttpDownloader download;
    download.downloadFile("https://github.com/msys2/msys2-installer/releases/download/2023-03-18/msys2-x86_64-20230318.exe");

    QObject::connect(&download, &HttpDownloader::downloadProgress, [](int percent){
        qDebug() << percent;
    });
    QObject::connect(&download, &HttpDownloader::downloadFinished, [](QString file, int size){
        qDebug() << file << size;
        qApp->quit();
    });
    QObject::connect(&download, &HttpDownloader::downloadError, [](QString str){
        qDebug() << str;
        qApp->quit();
    });
    QObject::connect(&download, &HttpDownloader::downloadAbout, [](){
        qApp->quit();
    });
    return a.exec();
}
