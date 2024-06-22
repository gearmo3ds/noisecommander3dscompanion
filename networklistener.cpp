#include "networklistener.h"
#include <QTcpSocket>
#include <qdatastream.h>
#include <iostream>
#include <QMessageBox>

void NetworkListener::run()
{
    stop = false;
    QTcpSocket socket;

    socket.connectToHost(ipAddress, 8080); // Change IP address and port as needed

    if (!socket.waitForConnected()) {
        qDebug() << "Error: " << socket.errorString();
        emit signalConnectionLost();
        QMessageBox msgBox;
        msgBox.setText(socket.errorString());
        msgBox.exec();
        return;
    }

    qDebug() << "Connected!";

    while (socket.state() == QAbstractSocket::ConnectedState)
    {
        if (socket.waitForReadyRead()) {
            QByteArray data = socket.readAll();
            handleData(data);
        }
        if (stop) {
            socket.close();
            emit signalConnectionLost();
            break;
        }
    }

    // Handle disconnection or other errors
    if (socket.state() == QAbstractSocket::UnconnectedState) {
        emit signalConnectionLost();
        qDebug() << "Disconnected from server.";
    } else {
        qDebug() << "Error reading data from server:" << socket.errorString();
        emit signalConnectionLost();
    }

}

void NetworkListener::handleData(const QByteArray &data)
{
    static QByteArray frame; // Static variable to hold partial frame data
    static uint16_t expectedSize = 0; // Expected size of the next message

    if (!data.size())
        return;
    uint32_t dataSize = data.size();
    uint32_t frameSize = frame.size();
//    if (blub < 6) {
//        bool shit = true;
//    }

    if ((frame.size() + data.size()) >= 5406) {
        bool shit = false;
    }
    // Append the new data to the frame
    frame.append(data);

    if (frame.size() >= 5406) {
        bool shit = false;
    }

    // Process the frame to extract and emit complete messages
    while (!frame.isEmpty()) {

        // When it's a new message
        if (expectedSize == 0) {
            // We need to read the size of the next message
            if (frame.size() < sizeof(uint16_t)) {
                // Not enough data to read message size, wait for more data
                return;
            }

            // Read the size of the next message
            QDataStream stream(frame.left(sizeof(uint16_t)));
            stream >> expectedSize;

            uint16_t swapped = (expectedSize>>8) | (expectedSize<<8);
            expectedSize = swapped;

            if (expectedSize != 5406) {
                bool shit = false;
            }

            // Remove the size prefix from the frame
//            frame.remove(0, sizeof(uint16_t));
        }

        if (frame.size() < expectedSize) {
            // Not enough data to complete the message, wait for more data
            return;
        }

        // Extract the complete message from the frame
        QByteArray message = frame.left(expectedSize);
        frame.remove(0, expectedSize);
        message.remove(0, 2);

        // Emit the complete message
        emit dataReceived(message);

        // Reset expectedSize for the next message
        expectedSize = 0;
    }
}

void NetworkListener::onConnect(QString ipAddress, QString port)
{
    this->ipAddress = ipAddress;
    start();
}

void NetworkListener::close()
{
    stop = true;
}

