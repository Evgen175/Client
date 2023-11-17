#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTime>
#include <QHostAddress>
#include <QTimer>
#include <QScreen>
#include <QPixmap>
#include <QApplication>
#include <signal.h>
#include <QtGui>
#include <qglobal.h>
#include <stdio.h>
#include <X11/Xlib.h>
//#include <X11/extensions/Xinerama.h>
#include <iostream>
#include <thread>
//#include <QScreen>

#include "netinet/tcp.h"
#include "qfile.h"

extern QList<QScreen*> screens;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    QTcpSocket *ARMO_pTcpSocket;
    quint16 ARMO_NextBlock;
    QHostAddress *_IP_Host;
    QTimer *_timerConnect;
    QScreen *screen;
    QPixmap screenShot;
    QTimer *_timerLookARMO;

private slots:
    void slorARMO_ReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotSendToARMI(const QString &mess);
    void slotConnected();
    void on_pushButton_clicked();
    void _slotTimerEvent();
    void _pressEnter();
    void _answeName(QString Name);
    void slotTimerSendScreen();
};
#endif // MAINWINDOW_H
