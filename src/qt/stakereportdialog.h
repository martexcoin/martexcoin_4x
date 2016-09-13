//*****************************************************
//
// Dialog which report the earning made with stake over time
// Original coding by Remy5
//

#ifndef STAKEREPORTDIALOG_H
#define STAKEREPORTDIALOG_H

#include <QDialog>

namespace Ui {
class StakeReportDialog;
}

class WalletModel;

class StakeReportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StakeReportDialog(QWidget *parent = 0);
    ~StakeReportDialog();

    void setModel(WalletModel *model);
    void showEvent(QShowEvent* event);

private:
    Ui::StakeReportDialog *ui;
    WalletModel *ex_model;

    qint64 nLastReportUpdate;
    bool disablereportupdate;
    bool alreadyConnected;

    void updateStakeReport(bool fImmediate);

private slots:
    void updateStakeReportTimer();

public slots:
    void updateStakeReportbalanceChanged(qint64, qint64, qint64, qint64);
    void updateStakeReportNow();
    void updateDisplayUnit(int);
    void CopyAllToClipboard();


};

#endif // STAKEREPORTDIALOG_H
