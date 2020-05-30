#ifndef ANONSENDCONFIG_H
#define ANONSENDCONFIG_H

#include <QDialog>

namespace Ui {
    class AnonsendConfig;
}
class WalletModel;

/** Multifunctional dialog to ask for passphrases. Used for encryption, unlocking, and changing the passphrase.
 */
class AnonsendConfig : public QDialog
{
    Q_OBJECT

public:

    AnonsendConfig(QWidget *parent = 0);
    ~AnonsendConfig();

    void setModel(WalletModel *model);


private:
    Ui::AnonsendConfig *ui;
    WalletModel *model;
    void configure(bool enabled, int coins, int rounds);

private Q_SLOTS:

    void clickBasic();
    void clickHigh();
    void clickMax();
};

#endif // ANONSENDCONFIG_H
