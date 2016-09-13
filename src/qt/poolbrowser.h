#ifndef POOLBROWSER_H
#define POOLBROWSER_H

#include "clientmodel.h"
#include "main.h"
#include "wallet.h"
#include "base58.h"

#include <QWidget>
#include <QObject>
#include <QtNetwork/QtNetwork>



namespace Ui {
class PoolBrowser;
}
class ClientModel;


class PoolBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit PoolBrowser(QWidget *parent = 0);
    ~PoolBrowser();
    
    void setModel(ClientModel *model);

private:
    void getRequest( const QString &url );
    void getRequest2( const QString &url );
    void getRequest3( const QString &url );

signals:
    void jokeReady( const QString &jokeAsJSON );
    void networkError( QNetworkReply::NetworkError err );

    void jokeReady2( const QString &jokeAsJSON );
    void networkError2( QNetworkReply::NetworkError err );
    void jokeReady3( const QString &jokeAsJSON );
    void networkError3( QNetworkReply::NetworkError err );

public slots:
    void parseNetworkResponse( QNetworkReply *finished );
    void parseNetworkResponse2( QNetworkReply *finished );
    void parseNetworkResponse3( QNetworkReply *finished );
    void randomChuckNorrisJoke();
    void randomChuckNorrisJoke2();
    void randomChuckNorrisJoke3();
    void bittrex();
    void egaldo();

private:
    QNetworkAccessManager m_nam;
    QNetworkAccessManager m_nam2;
    QNetworkAccessManager m_nam3;
    Ui::PoolBrowser *ui;
    ClientModel *model;

};

#endif // POOLBROWSER_H
