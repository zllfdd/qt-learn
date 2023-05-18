#include "httpdownloader.h"

#include <QAuthenticator>
#include <QFileInfo>

//https://doc.qt.io/qt-5/qtnetwork-http-example.html

HttpDownloader::HttpDownloader(QObject *parent)
    : QObject{parent}
{
    m_manager = new QNetworkAccessManager(this);

//    需要用户名密码
//    connect(&m_manager, &QNetworkAccessManager::authenticationRequired,
//                this, &HttpDownloader::slotAuthenticationRequired);
}

std::unique_ptr<QFile> HttpDownloader::openFileForWrite(const QString &fileName)
{
    std::unique_ptr<QFile> file(new QFile(fileName));
    file->open(QIODevice::WriteOnly);

    return file;
}

void HttpDownloader::startRequest(const QUrl &requestedUrl)
{
    m_url = requestedUrl;
    m_reply = m_manager->get(QNetworkRequest(m_url));

    connect(m_reply, &QNetworkReply::readyRead, this, [=](){
        while(!m_reply->atEnd())
        {
            QByteArray ba = m_reply->readAll();
            m_file->write(ba);
        }
    });
    connect(m_reply, &QNetworkReply::finished, this, &HttpDownloader::httpFinished);
    connect(m_reply, &QNetworkReply::downloadProgress, this, [=](qint64 received, qint64 total){
        qDebug() << "received:" << received << "total:" << total;
        if(total != 0){
            emit downloadProgress(received*100/total);
        }
    });
}

void HttpDownloader::httpFinished()
{
    QFileInfo fi;
    // 关闭文件
    if (m_file) {
        fi.setFile(m_file->fileName());
        m_file->close();
        m_file.reset();
    }
    // 如果　abort 删除　replay
    if (httpRequestAborted) {
        m_reply->deleteLater();
        m_reply = nullptr;
        emit downloadAbout();
        return;
    }
    // 发生错误
    if (m_reply->error()) {
        QFile::remove(fi.absoluteFilePath());
        qDebug() << tr("Download failed:%1.").arg(m_reply->errorString());
        emit downloadError(m_reply->errorString());
        m_reply->deleteLater();
        m_reply = nullptr;
        return;
    }
    //　处理跳转
    const QVariant redirectionTarget = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    m_reply->deleteLater();
    m_reply = nullptr;
    if (!redirectionTarget.isNull()) {
        const QUrl redirectedUrl = m_url.resolved(redirectionTarget.toUrl());
        qDebug() << tr("Redirect to %1 .").arg(redirectedUrl.toString());
        // 重新打开文件
        m_file = openFileForWrite(fi.absoluteFilePath());
        if (!m_file) {
            return;
        }
        // 请求新 url
        startRequest(redirectedUrl);
        return;
    }
    // 下载完成
    emit downloadFinished(fi.absoluteFilePath(), fi.size());
}

void HttpDownloader::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (m_file)
        m_file->write(m_reply->readAll());
}

void HttpDownloader::slotAuthenticationRequired(QNetworkReply *, QAuthenticator *authenticator)
{
    // todo:
//    authenticator->setUser("user");
//    authenticator->setPassword("password");
}

void HttpDownloader::downloadFile(const QString url)
{
    const QUrl newUrl = QUrl::fromUserInput(url);
    if (!newUrl.isValid()) {
        qDebug() << tr("Error") << tr("Invalid URL: %1: %2").arg(url, newUrl.errorString());
        return;
    }

    QString fileName = newUrl.fileName();
    if (fileName.isEmpty())
            fileName = "index.html";

    if (QFile::exists(fileName)) {
        QFile::remove((fileName));
    }
    m_file = openFileForWrite(fileName);
    if (!m_file) {
        return;
    }
    startRequest(newUrl);
}

void HttpDownloader::cancelDownload()
{
    httpRequestAborted = true;
    m_reply->abort();
}

