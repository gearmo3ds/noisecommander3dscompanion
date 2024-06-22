#ifndef NETWORKLISTENER_H
#define NETWORKLISTENER_H

#include <QThread>

//#define BUFFER_SIZE 10000
#define LINK3DS_COMM_PORT 17491 ///< 3dslink TCP server port

class NetworkListener : public QThread
{
public:
    Q_OBJECT
    void handleData(const QByteArray &data);
public slots:
    void onConnect(QString ipAddress, QString port);
    void close();
signals:
    void dataReceived(const QByteArray& data);
    void connectionSucceeded();
    void signalConnectionLost();
protected:
    void run() override;
private:
    QString ipAddress;
    bool stop = false;
};

#endif // NETWORKLISTENER_H
