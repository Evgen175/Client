#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unistd.h>
#include <linux/reboot.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , ARMO_NextBlock(0)
{
    ui->setupUi(this);
    //screen

    _timerConnect = new QTimer();
    _timerLookARMO=new QTimer();
    ARMO_pTcpSocket=new QTcpSocket(this);
   // screenShot=QPixmap::grabWindow(0,0,100,100);
   qDebug()<< screenShot.size().height();
   // _IP_Host=new QHostAddress("192.168.1.13");

    _IP_Host=new QHostAddress(QHostAddress::LocalHost);
   // on_pushButton_clicked();
    ARMO_pTcpSocket->connectToHost(*_IP_Host,2323);

    connect(ARMO_pTcpSocket,SIGNAL(connected()),SLOT(slotConnected()));
    connect(ARMO_pTcpSocket,SIGNAL(readyRead()),SLOT(slorARMO_ReadyRead()));
    connect(ARMO_pTcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(slotError(QAbstractSocket::SocketError)));
    connect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(_pressEnter()));
    connect(_timerConnect,SIGNAL(timeout()),this, SLOT(_slotTimerEvent()));
    connect(_timerLookARMO,SIGNAL(timeout()),this, SLOT(slotTimerSendScreen()));
    //connect(ARMO_pTcpSocket,SIGNAL(disconnect(),SLOT(slotDickonectARMI(const QObject *receiver, const char *member = 0))));

    ui->textEdit->setReadOnly(true);
    int rr=std::thread::hardware_concurrency();
    ui->textEdit->append(QString::number( rr));

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::_pressEnter()
{
    slotSendToARMI(ui->lineEdit->text());
}

void MainWindow::slorARMO_ReadyRead()
{
   // ui->textEdit->append(ARMO_pTcpSocket->read(1024).toHex()+'\n');
    QDataStream in(ARMO_pTcpSocket);
    for (; ; )
    {
        if (!ARMO_NextBlock)
        {
            if (ARMO_pTcpSocket->bytesAvailable()<sizeof (quint16))
            {break;}
            in>>ARMO_NextBlock;
        }
        if (ARMO_pTcpSocket->bytesAvailable()<ARMO_NextBlock)
        {break;}

        QString str;
        in>>str;
        _answeName(str);

        ARMO_NextBlock=0;
    }
}

void MainWindow::_answeName(QString _name)
{
    ui->textEdit->append(_name);

    if(_name=="USER_ID")
    {
        slotSendToARMI("USER_ID|"+QString::number(qrand()%10));
    }
    if(_name.left(2)=="SC")
    {
        if (_name.split("|").last()!="STOP")
        {
            _timerLookARMO->start((_name.split("|").last().toInt()*1000));
            slotSendToARMI("");
        }
        else
        {
            _timerLookARMO->stop();
        }

    }

    if(_name.left(2)=="SH")
    {
        int num = _name.split("|").last().toInt();
        if (num >= 0)
        {
            sync();
            LINUX_REBOOT_CMD_POWER_OFF;
            LINUX_REBOOT_CMD_POWER_OFF, 0;
            QProcess process;
            process.startDetached("sudo shutdown -P now");
        }

    }
}

void MainWindow::slotTimerSendScreen()
{
    slotSendToARMI("");
}
void MainWindow::slotError(QAbstractSocket::SocketError err)
{
    QString strError="Ошибка "+(err==QAbstractSocket::HostNotFoundError?"АРМИ не найден": err==QAbstractSocket::RemoteHostClosedError?"АРМИ закрыт": err==QAbstractSocket::ConnectionRefusedError?"Отказано в доступе": QString(ARMO_pTcpSocket->errorString()));
    ui->textEdit->append(strError);
    if (!_timerConnect->isActive())
    {
        _timerConnect->start(1000);
    }
}

void MainWindow::slotSendToARMI(const QString &message)
{
    ui->textEdit->append(message);
    QByteArray arrBlock;//КАРТИНКА - ТЕКСТ
    QBuffer outBuffer(&arrBlock);//КАРТИНКА
    if (message.length()==0)
    {
        screenShot=QPixmap::grabWindow(QApplication::desktop()->winId());
        screenShot.save(&outBuffer,"PNG");//КАРТИНКА
     //   quint32 pixmapLen=(quint32)outBuffer.data().size();
     //   qDebug()<<"pixmapLen="<<pixmapLen;

    }
    else
        {
        QDataStream _outMessageDS(&arrBlock,QIODevice::WriteOnly);
        _outMessageDS.setVersion(QDataStream::Qt_4_8);
        _outMessageDS<<quint16(0)<<message;
        _outMessageDS.device()->seek(0);
        _outMessageDS<<(quint16)(arrBlock.size()-sizeof (quint16));
        ui->lineEdit->setText("");
    }
    if (ARMO_pTcpSocket->waitForBytesWritten(10000))
    {
        qDebug()<<"OTPRAVIL";
    }

    qDebug()<<"count="<<arrBlock.count();
    qDebug()<<"data="<<arrBlock.size();
    qDebug()<<"data="<<arrBlock.constData();


    ARMO_pTcpSocket->write(arrBlock);

}
/*=====================================================*/
void MainWindow::slotConnected()
{
    _timerConnect->stop();
    ui->textEdit->append("Соединение установлено signal");
}

void MainWindow::on_pushButton_clicked()
{
    static bool click=true;
    if (click)
    {

        _timerLookARMO->stop();
        _timerConnect->stop();
        ARMO_pTcpSocket->disconnectFromHost();//*_IP_Host,23023);

    }
    else
    {
        ui->textEdit->clear();
        _timerConnect->start(4000);

    }
    QString txtBool=QString(click);
    ui->textEdit->append(("Кнопка нажата"+txtBool));
    click=!click;
}

void MainWindow::_slotTimerEvent()
{
    ARMO_pTcpSocket->connectToHost(*_IP_Host,23023);
}
