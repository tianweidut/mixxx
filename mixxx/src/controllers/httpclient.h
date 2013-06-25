﻿/**
* @file dlgprefcontroller.h
* @author Xin Wei  weixindlut@gmail.com
* @date  June 25 2013
* @brief Httpclient for preset manage issues
*/
#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QTextCodec>
#include <QThread>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class SleepThread : public QThread {
  public:
    static void usleep(int usec) {QThread::usleep(usec);}
    static void msleep(int msec) {QThread::msleep(msec);}
    static void sleep(int sec) {QThread::sleep(sec);}
};

class HttpClient : public QObject {
    Q_OBJECT

  public:
    explicit HttpClient(QObject *parent = 0);
    ~HttpClient();

    QString post(const QString &url, const QByteArray &postData);
    QString post(const QString &url, QMap<QString, QString> &postData);
    QString get(const QString &url);
    void setTextCodec(const QString &encoding);
    QMap<QString, QString> allCookies(const QString &url);
    QMap<QString, QString> defaultValuesFromName(const QString &html, const QString &name);

  private:
    void waitForFinish(QNetworkReply *reply);

    QNetworkAccessManager *m_manager;
    QTextCodec *m_textCodec;
};

#endif
