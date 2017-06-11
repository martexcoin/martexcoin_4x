#ifndef VALVERDENODECONFIGDIALOG_H
#define VALVERDENODECONFIGDIALOG_H

#include <QDialog>

namespace Ui {
    class ValverdeNodeConfigDialog;
}

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Dialog showing transaction details. */
class ValverdeNodeConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ValverdeNodeConfigDialog(QWidget *parent = 0, QString nodeAddress = "123.456.789.123:9999", QString privkey="MASTERNODEPRIVKEY");
    ~ValverdeNodeConfigDialog();

private:
    Ui::ValverdeNodeConfigDialog *ui;
};

#endif // VALVERDENODECONFIGDIALOG_H
