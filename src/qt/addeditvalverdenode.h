#ifndef ADDEDITVALVERDENODE_H
#define ADDEDITVALVERDENODE_H

#include <QDialog>

namespace Ui {
class AddEditValverdeNode;
}


class AddEditValverdeNode : public QDialog
{
    Q_OBJECT

public:
    explicit AddEditValverdeNode(QWidget *parent = 0);
    ~AddEditValverdeNode();

protected:

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

signals:

private:
    Ui::AddEditValverdeNode *ui;
};

#endif // ADDEDITVALVERDENODE_H
