#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Инициализация интерфейса
    ui->setupUi(this);
    //Создание и настройка UDPworker
    udpWorker = new UDPworker(this);
    udpWorker->InitSocket(12345);
    //Подключение сигнала к слоту (Подключается сигнал sig_sendTimeToGUI к слоту DisplayTime)
    connect(udpWorker, &UDPworker::sig_sendTimeToGUI, this, &MainWindow::DisplayTime);
    //Создание второго UDPworker (Создается второй объект UDPworker для отправки данных и подключается сигнал sig_sendInputTextToGUI к слоту DisplayInputText)
    udpWorkerSendDatagram = new UDPworker(this);
    udpWorkerSendDatagram->InitSocket(123456);
    connect(udpWorkerSendDatagram, &UDPworker::sig_sendInputTextToGUI, this, &MainWindow::DisplayInputText);
    //Создание и настройка таймера (Создается таймер, который при каждом срабатывании отправляет текущее время через udpWorker)
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&]{

        QDateTime dateTime = QDateTime::currentDateTime();

        QByteArray dataToSend;
        QDataStream outStr(&dataToSend, QIODevice::WriteOnly);

        outStr << dateTime;

        udpWorker->SendDatagram(dataToSend);
        timer->start(TIMER_DELAY); // не думаю, что это нужно

    });

}

MainWindow::~MainWindow()
{
    //Освобождаем память, выделенную для интерфейса ui
    delete ui;
}


void MainWindow::on_pb_start_clicked()
{
    //Запускаем таймер с заданной задержкой TIMER_DELAY
    timer->start(TIMER_DELAY);
}


void MainWindow::DisplayTime(QDateTime data)
{
    //Увеличиваем счетчик пакетов counterPck, очищаем текстовое поле каждые 20 пакетов и добавляем текущее время в текстовое поле.
    counterPck++;
    if(counterPck % 20 == 0){
        ui->te_result->clear();
    }

    ui->te_result->append("Текущее время: " + data.toString() + ". "
                                                                "Принято пакетов " + QString::number(counterPck));


}

void MainWindow::DisplayInputText(QString data, QString address, int port)
{
    //Добавляем информацию о полученном сообщении
    ui->te_result->append("Принято сообщение от " + address + ":" +
                          QString::number(port) + ", размер сообщения в байтах: " +
                          QString::number(data.size()));
}


void MainWindow::on_pb_stop_clicked()
{
    //Останавливаем таймер
    timer->stop();
}

void MainWindow::on_pb_sendDatagram_clicked()
{
    //Получаем текст из поля ввода, сериализуем его и отправляет через udpWorkerSendDatagram
    QString dateInputText = ui->le_inputText->text();

    QByteArray dataToSend;
    QDataStream outStr(&dataToSend, QIODevice::WriteOnly);

    outStr << dateInputText;

    udpWorkerSendDatagram->SendDatagram(dataToSend);
}

