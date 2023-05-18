#ifndef HTTPDOWNLOADER_H
#define HTTPDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFile>

class HttpDownloader: public QObject
{
    Q_OBJECT
public:
    explicit HttpDownloader(QObject *parent = nullptr);

    void downloadFile(const QString url);
    void cancelDownload();

private:
    QUrl m_url;
    QNetworkAccessManager *m_manager;
    QNetworkReply *m_reply;
    bool httpRequestAborted;
    std::unique_ptr<QFile> m_file;

    std::unique_ptr<QFile> openFileForWrite(const QString &fileName);
    void startRequest(const QUrl &requestedUrl);
    void httpFinished();
    void httpReadyRead();
    void slotAuthenticationRequired(QNetworkReply *, QAuthenticator *authenticator);
signals:
//    void downloadStarted(int fileSize);
    void downloadFinished(QString fileName, int fileSize);
    void downloadProgress(int percent);
    void downloadError(QString errString);
    void downloadAbout();
};

#endif // HTTPDOWNLOADER_H
