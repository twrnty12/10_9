#include "udpworker.h"

UDPworker::UDPworker(QObject *parent) : QObject(parent)
{



}



void UDPworker::InitSocket(int bindPort)
{
    //Инициализируем сокет QUdpSocket, привязываем его к локальному адресу и порту bindPort_, и подключаем сигнал readyRead к слоту readPendingDatagrams
    bindPort_ = bindPort;
    serviceUdpSocket = new QUdpSocket(this);
  
    serviceUdpSocket->bind(QHostAddress::LocalHost, bindPort_);

    connect(serviceUdpSocket, &QUdpSocket::readyRead, this, &UDPworker::readPendingDatagrams);

}


void UDPworker::ReadDatagram(QNetworkDatagram datagram)
{
    //Читаем данные из датаграммы, десериализуем их и в зависимости от порта (12345 или 123456) отправляем соответствующий сигнал с данными.
    QByteArray data;
    data = datagram.data();

    QDataStream inStr(&data, QIODevice::ReadOnly);
    if (bindPort_ == 12345)
    {
        QDateTime dateTime;
        inStr >> dateTime;
        emit sig_sendTimeToGUI(dateTime);
    }
    else if(bindPort_ == 123456)
    {
        QString dateInputText;
        inStr >> dateInputText;
        QString localAddress = serviceUdpSocket->localAddress().toString();
        emit sig_sendInputTextToGUI(dateInputText, localAddress, bindPort_);
    }
}

void UDPworker::SendDatagram(QByteArray data)
{
    //Отправляем датаграмму с данными на локальный адрес и порт bindPort_
    serviceUdpSocket->writeDatagram(data, QHostAddress::LocalHost, bindPort_);
}


void UDPworker::readPendingDatagrams( void )
    //Проверяем наличие ожидающих датаграмм и обрабатываем их с помощью функции ReadDatagram
    while(serviceUdpSocket->hasPendingDatagrams()){
            QNetworkDatagram datagram = serviceUdpSocket->receiveDatagram();
            ReadDatagram(datagram);
    }

}
