#include "valverdenodeconfigdialog.h"
#include "ui_valverdenodeconfigdialog.h"

#include <QModelIndex>

ValverdeNodeConfigDialog::ValverdeNodeConfigDialog(QWidget *parent, QString nodeAddress, QString privkey) :
    QDialog(parent),
    ui(new Ui::ValverdeNodeConfigDialog)
{
    ui->setupUi(this);
    QString desc = "<br>masternode=1<br>masternodeaddr=" + nodeAddress + "<br>masternodeprivkey=" + privkey + "<br>";
    ui->detailText->setHtml(desc);
}

ValverdeNodeConfigDialog::~ValverdeNodeConfigDialog()
{
    delete ui;
}
