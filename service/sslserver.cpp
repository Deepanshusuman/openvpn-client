#include "sslserver.h"

#include "debug.h"

#include <QCoreApplication>
#include <QTimer>

extern bool g_bPortable;

SslServer::SslServer(quint16 port, QObject *parent)
    : QTcpServer(parent),
      connections (new SslConnections)
{
    //
    // Building a TCP Server which is listen at the port
    //
    listen(QHostAddress::LocalHost, port);

    Debug::log(QLatin1String("SSL server seems started succesfully"));
}

SslServer::~SslServer()
{
    //
    // Clear the connections list when programs ends
    //
    this->connections->clear();
}

void SslServer::incomingConnection(int socketDescriptor)
{
    //
    // A new connection is incomming, create a new connection object and append it to the connection list
    //
    Debug::log(QLatin1String("New connection"));
    SslServerConnection *newIncommingConnection = new SslServerConnection (socketDescriptor, this);

    // Add the connection to the current connected list
    this->connections->add(newIncommingConnection);

    // When we receives the connectionIsClosed signal we exit the eventloop in the thread
    QObject::connect(newIncommingConnection, SIGNAL(connectionIsClosed()), this, SLOT(slotQuitThread()));
    // When the thread is finished cleanup
    QObject::connect(newIncommingConnection, SIGNAL(finished()), this, SLOT(slotThreadFinished()));

    // Start thread now
    newIncommingConnection->start(QThread::NormalPriority);
    newIncommingConnection->setId(this->connections->indexOf(newIncommingConnection));
    Debug::log(QLatin1String("Concurrent threads: ") + QString::number(this->connections->count()));
}

SslConnections *SslServer::connectionsList() const
{
    //
    // Return the connection list
    //
    return this->connections;
}

void SslServer::slotThreadFinished()
{
    //
    // Thread is done, mark the object to delete
    //

    // Remove the connection from the connection list
    SslServerConnection *con = qobject_cast<SslServerConnection*>(sender());
    Q_ASSERT(con);

    if (con) {
        // Remove item with thread id from the list
        this->connections->remove(con->threadId());
    }
    // mark object to delete
    sender()->deleteLater();

    if(connections->count() == 0)
    {
        // Close the serivce if we are in portable mode as the last client has disconnected
        if(g_bPortable)
        {
            //
            QTimer::singleShot(300, Qt::TimerType::CoarseTimer, this, SLOT(OnStopService()));
        }
    }
}

void SslServer::OnStopService()
{
    QCoreApplication::exit();
}

void SslServer::slotQuitThread()
{
    //
    // Stop the thread
    //
    SslServerConnection *con = qobject_cast<SslServerConnection*>(sender());
    Q_ASSERT(con);

    if (!con) {
        return;
    }
    // Object is valid
    con->quit();
    // Wait 5 seconds to end the thread, then terminate it
    if (!con->wait(5000)) {
        con->terminate();
        // Wait 2 sec
        con->wait(2000);
    }
}
