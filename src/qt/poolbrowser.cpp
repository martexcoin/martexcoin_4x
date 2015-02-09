#include "poolbrowser.h"
#include "ui_poolbrowser.h"
#include "main.h"
#include "wallet.h"
#include "base58.h"
#include "clientmodel.h"
#include "bitcoinrpc.h"
#include <QDesktopServices>

#include <sstream>
#include <string>

using namespace json_spirit;

const QString kBaseUrl = "https://bittrex.com/api/v1/public/";
const QString kBaseUrl2 = "http://blockchain.info/tobtc?currency=USD&value=1";
const QString kBaseUrl3 = "https://bittrex.com/api/v1/public/getorderbook?market=BTC-N5X&type=both&depth=10";
QString lastp = "";
QString askp = "";
QString bidp = "";
QString highp = "";
QString lowp = "";
QString volumebp = "";
QString volumesp = "";
QString bop = "";
QString sop = "";
QString bitcoinp = "";
double bitcoin2;
double asku;
QString askus;
double lastu;
QString lastus;
double bidu;
QString bidus;
double volumeu;
QString volumeus;
double yestu;
QString yestus;

PoolBrowser::PoolBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PoolBrowser)
{
    ui->setupUi(this);
    ui->buyquan->header()->resizeSection(0,120);
    ui->sellquan->header()->resizeSection(0,120);
    setFixedSize(400, 420);
randomChuckNorrisJoke();
randomChuckNorrisJoke2();
randomChuckNorrisJoke3();
QObject::connect(&m_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseNetworkResponse(QNetworkReply*)));
QObject::connect(&m_nam2, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseNetworkResponse2(QNetworkReply*)));
QObject::connect(&m_nam3, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseNetworkResponse3(QNetworkReply*)));
connect(ui->startButton, SIGNAL(pressed()), this, SLOT( randomChuckNorrisJoke()));
connect(ui->bittrex, SIGNAL(pressed()), this, SLOT( bittrex()));
connect(ui->egal, SIGNAL(pressed()), this, SLOT( egaldo()));

}
void PoolBrowser::egaldo()
{
    QString temps = ui->egals->text();
    double totalsc = temps.toDouble();
    double totald = lastu * totalsc;
    ui->egald->setText(QString::number(totald) + " $");

}

void PoolBrowser::bittrex()
{
    QDesktopServices::openUrl(QUrl("https://www.bittrex.com/Market/Index?MarketName=BTC-N5X"));
}

void PoolBrowser::randomChuckNorrisJoke()
{
    randomChuckNorrisJoke2();
    randomChuckNorrisJoke3();
    getRequest(kBaseUrl + "/getmarketsummaries");
}
void PoolBrowser::randomChuckNorrisJoke2()
{
getRequest2(kBaseUrl2);
}
void PoolBrowser::randomChuckNorrisJoke3()
{
getRequest3(kBaseUrl3);
}

void PoolBrowser::getRequest( const QString &urlString )
{
    QUrl url ( urlString );
    QNetworkRequest req ( url );
    m_nam.get( req );
}
void PoolBrowser::getRequest2( const QString &urlString )
{
    QUrl url2 ( urlString );
    QNetworkRequest req2 ( url2 );
    m_nam2.get( req2 );
}

void PoolBrowser::getRequest3( const QString &urlString )
{
    QUrl url3 ( urlString );
    QNetworkRequest req3 ( url3 );
    m_nam3.get( req3 );
}

void PoolBrowser::parseNetworkResponse( QNetworkReply *finished )
{
    if ( finished->error() != QNetworkReply::NoError )
    {
        // A communication error has occurred
        emit networkError( finished->error() );
        return;
    }

    // QNetworkReply is a QIODevice. So we read from it just like it was a file
    QString data = finished->readAll();
    QStringList data2 = data.split("{\"MarketName\":\"BTC-N5X\",\"High\":");
    QStringList high = data2[1].split(",\"Low\":"); // high = high
    QStringList low = high[1].split(",\"Volume\":");
    QStringList volume = low[1].split(",\"Last\":");
    QStringList last = volume[1].split(",\"BaseVolume\":");
    QStringList basevolume = last[1].split(",\"TimeStamp\":\"");
    QStringList time = basevolume[1].split("\",\"Bid\":");
    QStringList bid = time[1].split(",\"Ask\":");
    QStringList ask = bid[1].split(",\"OpenBuyOrders\":");
    QStringList openbuy = ask[1].split(",\"OpenSellOrders\":");
    QStringList opensell = openbuy[1].split(",\"PrevDay\":");
    QStringList yest = opensell[1].split(",\"Created\":");

   // 0.00000978,"Low":0.00000214,"Volume":3718261.74455189,"Last":0.00000558,
   //"BaseVolume":22.42443460,"TimeStamp":"2014-05-13T10:08:06.553","Bid":0.00000558,"Ask":0.00000559,"OpenBuyOrders":42,"OpenSellOrders":42,"PrevDay":0.00000861}
    lastu = last[0].toDouble() * bitcoin2;
    lastus = QString::number(lastu);

    if(last[0] > lastp)
    {
        ui->last->setText("<b><font color=\"green\">" + last[0] + "</font></b>");
        ui->lastu->setText("<b><font color=\"green\">" + lastus + " $</font></b>");
    } else if (last[0] < lastp) {
        ui->last->setText("<b><font color=\"red\">" + last[0] + "</font></b>");
         ui->lastu->setText("<b><font color=\"red\">" + lastus + " $</font></b>");
        } else {
    ui->last->setText(last[0]);
    ui->lastu->setText(lastus + " $");
    }

    asku = ask[0].toDouble() * bitcoin2;
    askus = QString::number(asku);

    if(ask[0] > askp)
    {
        ui->ask->setText("<b><font color=\"green\">" + ask[0] + "</font></b>");
        ui->asku->setText("<b><font color=\"green\">" + askus + " $</font></b>");
    } else if (ask[0] < askp) {
        ui->ask->setText("<b><font color=\"red\">" + ask[0] + "</font></b>");
        ui->asku->setText("<b><font color=\"red\">" + askus + " $</font></b>");
        } else {
    ui->ask->setText(ask[0]);
    ui->asku->setText(askus + " $");
    }

    bidu = bid[0].toDouble() * bitcoin2;
    bidus = QString::number(bidu);


    if(bid[0] > bidp)
    {
        ui->bid->setText("<b><font color=\"green\">" + bid[0] + "</font></b>");
        ui->bidu->setText("<b><font color=\"green\">" + bidus + " $</font></b>");
    } else if (bid[0] < bidp) {
        ui->bid->setText("<b><font color=\"red\">" + bid[0] + "</font></b>");
        ui->bidu->setText("<b><font color=\"red\">" + bidus + " $</font></b>");
        } else {
    ui->bid->setText(bid[0]);
    ui->bidu->setText(bidus + " $");
    }
    if(high[0] > highp)
    {
        ui->high->setText("<b><font color=\"green\">" + high[0] + "</font></b>");
    } else if (high[0] < highp) {
        ui->high->setText("<b><font color=\"red\">" + high[0] + "</font></b>");
        } else {
    ui->high->setText(high[0]);
    }
    if(low[0] > lowp)
    {
        ui->low->setText("<b><font color=\"green\">" + low[0] + "</font></b>");
    } else if (low[0] < lowp) {
        ui->low->setText("<b><font color=\"red\">" + low[0] + "</font></b>");
        } else {
    ui->low->setText(low[0]);
    }


    if(volume[0] > volumebp)
    {
        ui->volumeb->setText("<b><font color=\"green\">" + volume[0] + "</font></b>");

    } else if (volume[0] < volumebp) {
        ui->volumeb->setText("<b><font color=\"red\">" + volume[0] + "</font></b>");
        ui->volumeu->setText("<b><font color=\"red\">" + volumeus + " $</font></b>");
        } else {
    ui->volumeb->setText(volume[0]);
    ui->volumeu->setText(volumeus + " $");
    }

    volumeu = basevolume[0].toDouble() * bitcoin2;
    volumeus = QString::number(volumeu);

    if(basevolume[0] > volumesp)
    {
        ui->volumes->setText("<b><font color=\"green\">" + basevolume[0] + "</font></b>");
        ui->volumeu->setText("<b><font color=\"green\">" + volumeus + " $</font></b>");
    } else if (basevolume[0] < volumesp) {
        ui->volumes->setText("<b><font color=\"red\">" + basevolume[0] + "</font></b>");
        ui->volumeu->setText("<b><font color=\"red\">" + volumeus + " $</font></b>");
        } else {
    ui->volumes->setText(basevolume[0]);
    ui->volumeu->setText(volumeus + " $");
    }

    if(last[0].toDouble() > yest[0].toDouble())
    {
        yestu = ((last[0].toDouble() - yest[0].toDouble())/last[0].toDouble())*100;
        yestus = QString::number(yestu);

        ui->yest->setText("<b><font color=\"green\"> + " + yestus + " %</font></b>");


    }else
    {
        yestu = ((yest[0].toDouble() - last[0].toDouble())/yest[0].toDouble())*100;
        yestus = QString::number(yestu);
        ui->yest->setText("<b><font color=\"red\"> - " + yestus + " %</font></b>");

    }

    ui->bo->setText(openbuy[0]);
    ui->so->setText(opensell[0]);

    lastp = last[0];
    askp = ask[0];
    bidp = bid[0];
    highp = high[0];
    lowp = low[0];
    volumebp = volume[0];
    volumesp = basevolume[0];
    bop = openbuy[0];
    sop = opensell[0];

    emit jokeReady( data );
}

void PoolBrowser::parseNetworkResponse2(QNetworkReply *finished )
{
    if ( finished->error() != QNetworkReply::NoError )
    {
        // A communication error has occurred
        emit networkError2( finished->error() );
        return;
    }

    // QNetworkReply is a QIODevice. So we read from it just like it was a file
    QString bitcoin = finished->readAll();
    bitcoin2 = (1 / bitcoin.toDouble());
    bitcoin = QString::number(bitcoin2);
    if(bitcoin > bitcoinp)
    {
        ui->bitcoin->setText("<b><font color=\"green\">" + bitcoin + " $</font></b>");
    } else if (bitcoin < bitcoinp) {
        ui->bitcoin->setText("<b><font color=\"red\">" + bitcoin + " $</font></b>");
        } else {
    ui->bitcoin->setText(bitcoin + " $");
    }


    bitcoinp = bitcoin;
    emit jokeReady2( bitcoin );
}

void PoolBrowser::parseNetworkResponse3(QNetworkReply *finished )
{
    if ( finished->error() != QNetworkReply::NoError )
    {
        // A communication error has occurred
        emit networkError2( finished->error() );
        return;
    }

    // QNetworkReply is a QIODevice. So we read from it just like it was a file
    QString marketd = finished->readAll();
    marketd = marketd.replace("{","");
    marketd = marketd.replace("}","");
    marketd = marketd.replace("\"","");
    marketd = marketd.replace("],\"sell\":","");
    marketd = marketd.replace(" ","");
    marketd = marketd.replace("]","");
    marketd = marketd.replace("Quantity:","");
    marketd = marketd.replace("Rate:","");
    QStringList marketd2 = marketd.split("["); // marketd2[1] => buy order marketd2[2] => sell
    QStringList marketdb = marketd2[1].split(",");
    QStringList marketds = marketd2[2].split(",");

    ui->sellquan->clear();
    ui->buyquan->clear();

      ui->sellquan->setSortingEnabled(true);
       ui->buyquan->setSortingEnabled(true);

       for (int i = 1; i < 19; i++) {

           QTreeWidgetItem * item = new QTreeWidgetItem();
           item->setText(0,marketdb[i]);
           item->setText(1,marketdb[i+1]);
           ui->buyquan->addTopLevelItem(item);

           QTreeWidgetItem * item2 = new QTreeWidgetItem();
           item2->setText(0,marketds[i+1]);
           item2->setText(1,marketds[i]);
           ui->sellquan->addTopLevelItem(item2);


           i = i + 1;
       }

    emit jokeReady3( marketd );

}


void PoolBrowser::setModel(ClientModel *model)
{
    this->model = model;
}

PoolBrowser::~PoolBrowser()
{
    delete ui;
}
