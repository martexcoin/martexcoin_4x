// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "qt/pivx/settings/settingswalletrepairwidget.h"
#include "qt/pivx/settings/forms/ui_settingswalletrepairwidget.h"
#include "qt/pivx/qtutils.h"
#include "util.h"
#include "walletmodel.h"
#include <startoptionsmain.h>
#include "askpassphrasedialog.h"

SettingsWalletRepairWidget::SettingsWalletRepairWidget(MARTEXGUI* _window, QWidget *parent) :
    PWidget(_window, parent),
    ui(new Ui::SettingsWalletRepairWidget)
{
    ui->setupUi(this);
    this->setStyleSheet(parent->styleSheet());

    // Containers
    ui->left->setProperty("cssClass", "container");
    ui->left->setContentsMargins(10,10,10,10);
    ui->scrollStack->setProperty("cssClass", "container");

    // Title
    ui->labelTitle->setText(tr("Wallet Repair"));
    ui->labelTitle->setProperty("cssClass", "text-title-screen");

    // Subtitle
    ui->labelSubtitle1->setText(tr("The buttons below will restart the wallet with command-line options to repair this wallet, fix issues with corrupt blockchain files or missing/obsolete transactions."));
    ui->labelSubtitle1->setProperty("cssClass", "text-subtitle");

    // Labels
    ui->labelMessageSalvage->setText(tr("Attempt to recover private keys from a corrupt wallet.dat."));
    ui->labelMessageSalvage->setProperty("cssClass", "text-main-settings");

    ui->labelMessageRescan->setText(tr("Rescan the blockchain for missing wallet transactions."));
    ui->labelMessageRescan->setProperty("cssClass", "text-main-settings");

    ui->labelMessageRecover1->setText(tr("Recover transactions from blockchain (keep-meta-data, e.g. account owner)."));
    ui->labelMessageRecover1->setProperty("cssClass", "text-main-settings");

    ui->labelMessageRecover2->setText(tr("Recover transactions from blockchain (drop meta-data)."));
    ui->labelMessageRecover2->setProperty("cssClass", "text-main-settings");

    ui->labelMessageUpgrade->setText(tr("Upgrade wallet to latest format on startup. (Note: this is NOT an update of the wallet itself)"));
    ui->labelMessageUpgrade->setProperty("cssClass", "text-main-settings");

    ui->labelMessageRebuild->setText(tr("Rebuild blockchain index from current blk000???.dat files."));
    ui->labelMessageRebuild->setProperty("cssClass", "text-main-settings");

    ui->labelMessageDelete->setText(tr("Deletes all local blockchain folders so the wallet synchronizes from scratch."));
    ui->labelMessageDelete->setProperty("cssClass", "text-main-settings");

    ui->labelMessageUpgradeToHD->setText(tr("Upgrade non-hd wallet to hd. You will no longer be able to cold-stake with HD as it isn't supported yet."));
    ui->labelMessageUpgradeToHD->setProperty("cssClass", "text-main-settings");

    // Buttons
    ui->pushButtonSalvage->setText(tr("Salvage wallet"));
    setCssBtnPrimary(ui->pushButtonSalvage);

    ui->pushButtonRescan->setText(tr("Rescan blockchain file"));
    setCssBtnPrimary(ui->pushButtonRescan);

    ui->pushButtonRecover1->setText(tr("Recover transactions 1"));
    setCssBtnPrimary(ui->pushButtonRecover1);

    ui->pushButtonRecover2->setText(tr("Recover transactions 2"));
    setCssBtnPrimary(ui->pushButtonRecover2);

    ui->pushButtonUpgrade->setText(tr("Upgrade wallet format"));
    setCssBtnPrimary(ui->pushButtonUpgrade);

    ui->pushButtonRebuild->setText(tr("Rebuild index"));
    setCssBtnPrimary(ui->pushButtonRebuild);

    ui->pushButtonDelete->setText(tr("Delete local blockchain "));
    setCssBtnPrimary(ui->pushButtonDelete);

    ui->pushButtonUpgradeToHd->setText(tr("Upgrade non-HD Wallet to HD"));
    setCssBtnPrimary(ui->pushButtonUpgradeToHd);


    // Wallet Repair Buttons
    connect(ui->pushButtonSalvage, SIGNAL(clicked()), this, SLOT(walletSalvage()));
    connect(ui->pushButtonRescan, SIGNAL(clicked()), this, SLOT(walletRescan()));
    connect(ui->pushButtonRecover1, SIGNAL(clicked()), this, SLOT(walletZaptxes1()));
    connect(ui->pushButtonRecover2, SIGNAL(clicked()), this, SLOT(walletZaptxes2()));
    connect(ui->pushButtonUpgrade, SIGNAL(clicked()), this, SLOT(walletUpgrade()));
    connect(ui->pushButtonRebuild, SIGNAL(clicked()), this, SLOT(walletReindex()));
    connect(ui->pushButtonDelete, SIGNAL(clicked()), this, SLOT(walletResync()));
    connect(ui->pushButtonUpgradeToHd, SIGNAL(clicked()), this, SLOT(walletUpgradeToHd()));
}

/** Restart wallet with "-salvagewallet" */
void SettingsWalletRepairWidget::walletSalvage()
{
    buildParameterlist(SALVAGEWALLET);
}

/** Restart wallet with "-rescan" */
void SettingsWalletRepairWidget::walletRescan()
{
    buildParameterlist(RESCAN);
}

/** Restart wallet with "-zapwallettxes=1" */
void SettingsWalletRepairWidget::walletZaptxes1()
{
    buildParameterlist(ZAPTXES1);
}

/** Restart wallet with "-zapwallettxes=2" */
void SettingsWalletRepairWidget::walletZaptxes2()
{
    buildParameterlist(ZAPTXES2);
}

/** Restart wallet with "-upgradewallet" */
void SettingsWalletRepairWidget::walletUpgrade()
{
    buildParameterlist(UPGRADEWALLET);
}

/** Restart wallet with "-reindex" */
void SettingsWalletRepairWidget::walletReindex()
{
    buildParameterlist(REINDEX);
}

/** Restart wallet with "-resync" */
void SettingsWalletRepairWidget::walletResync()
{
    QString resyncWarning = tr("This will delete your local blockchain folders and the wallet will synchronize the complete Blockchain from scratch.<br /><br />");
    resyncWarning +=   tr("This needs quite some time and downloads a lot of data.<br /><br />");
    resyncWarning +=   tr("Your transactions and funds will be visible again after the download has completed.<br /><br />");
    resyncWarning +=   tr("Do you want to continue?.<br />");
    QMessageBox::StandardButton retval = QMessageBox::question(this, tr("Confirm resync Blockchain"),
                                                               resyncWarning,
                                                               QMessageBox::Yes | QMessageBox::Cancel,
                                                               QMessageBox::Cancel);

    if (retval != QMessageBox::Yes) {
        // Resync canceled
        return;
    }

    // Restart and resync
    buildParameterlist(RESYNC);
}

/** Restart wallet with "-resync" and upgrade to a HD wallet*/
void SettingsWalletRepairWidget::walletUpgradeToHd()
{
    QString upgradeWarning = tr("This will convert you non-HD wallet to a HD wallet<br /><br />");
    upgradeWarning +=   tr("Make sure to make a backup of your wallet ahead of time<br /><br />");
    upgradeWarning +=   tr("You shouldn't force close the wallet while this is running<br /><br />");
    upgradeWarning +=   tr("Do you want to continue?.<br />");
    QMessageBox::StandardButton retval = QMessageBox::question(this, tr("Confirm upgrade to HD wallet"),
                                                               upgradeWarning,
                                                               QMessageBox::Yes | QMessageBox::Cancel,
                                                               QMessageBox::Cancel);

    if (retval != QMessageBox::Yes) {
        // Resync canceled
        return;
    }

    if (IsInitialBlockDownload()) {
        LogPrintf("Cannot set a new HD seed while still in Initial Block Download");
    }

    LOCK2(cs_main, pwalletMain->cs_wallet);

    // Do not do anything to HD wallets
    if (pwalletMain->IsHDEnabled()) {
        LogPrintf("Cannot upgrade a wallet to hd if It is already upgraded to hd.");
    }

    std::vector<std::string> words;
    SecureString strWalletPass;
    strWalletPass.reserve(100);

    int prev_version = pwalletMain->GetVersion();

    int nMaxVersion = GetArg("-upgradewallet", 0);
    if (nMaxVersion == 0) // the -upgradewallet without argument case
    {
        LogPrintf("Performing wallet upgrade to %i\n", FEATURE_LATEST);
        nMaxVersion = CLIENT_VERSION;
        pwalletMain->SetMinVersion(FEATURE_LATEST); // permanently upgrade the wallet immediately
    } else
        LogPrintf("Allowing wallet upgrade up to %i\n", nMaxVersion);
    if (nMaxVersion < pwalletMain->GetVersion()) {
        LogPrintf("Cannot downgrade wallet");
    }

    pwalletMain->SetMaxVersion(nMaxVersion);

    // Do not upgrade versions to any version between HD_SPLIT and FEATURE_PRE_SPLIT_KEYPOOL unless already supporting HD_SPLIT
    int max_version = pwalletMain->GetVersion();
    if (!pwalletMain->CanSupportFeature(FEATURE_HD) && max_version >=FEATURE_HD && max_version < FEATURE_PRE_SPLIT_KEYPOOL) {
        LogPrintf("Cannot upgrade a non HD split wallet without upgrading to support pre split keypool. Please use -upgradewallet=169900 or -upgradewallet with no version specified.");
    }

    bool hd_upgrade = false;
    bool split_upgrade = false;
    if (pwalletMain->CanSupportFeature(FEATURE_HD) && !pwalletMain->IsHDEnabled()) {
        LogPrintf("Upgrading wallet to HD\n");
        pwalletMain->SetMinVersion(FEATURE_HD);

        if (walletModel->getEncryptionStatus() == WalletModel::Locked || walletModel->getEncryptionStatus() == WalletModel::UnlockedForStaking) {
            AskPassphraseDialog dlg(AskPassphraseDialog::Mode::Unlock, this, walletModel, AskPassphraseDialog::Context::ToggleLock);
            dlg.exec();
            strWalletPass = dlg.getPassword();
        } else {
            strWalletPass = std::string().c_str();
        }

        StartOptionsMain dlg(nullptr);
        dlg.exec();
        words = dlg.getWords();

        pwalletMain->GenerateNewHDChain(words, strWalletPass);

        hd_upgrade = true;
    }

    // Upgrade to HD chain split if necessary
    if (pwalletMain->CanSupportFeature(FEATURE_HD)) {
        LogPrintf("Upgrading wallet to use HD chain split\n");
        pwalletMain->SetMinVersion(FEATURE_PRE_SPLIT_KEYPOOL);
        split_upgrade = FEATURE_HD > prev_version;
    }

    // Mark all keys currently in the keypool as pre-split
    if (split_upgrade) {
        pwalletMain->MarkPreSplitKeys();
    }

    // Regenerate the keypool if upgraded to HD
    if (hd_upgrade) {
        if (!pwalletMain->TopUpKeyPool()) {
            LogPrintf("Unable to generate keys\n");
        }
    }

    buildParameterlist(RESCAN);

}

/** Build command-line parameter list for restart */
void SettingsWalletRepairWidget::buildParameterlist(QString arg)
{
    // Get command-line arguments and remove the application name
    QStringList args = QApplication::arguments();
    args.removeFirst();

    // Remove existing repair-options
    args.removeAll(SALVAGEWALLET);
    args.removeAll(RESCAN);
    args.removeAll(ZAPTXES1);
    args.removeAll(ZAPTXES2);
    args.removeAll(UPGRADEWALLET);
    args.removeAll(REINDEX);

    // Append repair parameter to command line.
    args.append(arg);

    // Send command-line arguments to MARTEXGUI::handleRestart()
    Q_EMIT handleRestart(args);
}

SettingsWalletRepairWidget::~SettingsWalletRepairWidget()
{
    delete ui;
}
