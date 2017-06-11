#ifndef CALCDIALOG_H
#define CALCDIALOG_H

#include <QDialog>

namespace Ui {
class calcDialog;
}
class ClientModel;

class calcDialog : public QDialog
{
    Q_OBJECT

public:
    explicit calcDialog(QWidget *parent = 0);
    ~calcDialog();

    void setModel(ClientModel *model);
private slots:
    void on_buttonBox_accepted();
	void pushButtonClicked();

private:
    Ui::calcDialog *ui;
};

#endif // CALCDIALOG_H