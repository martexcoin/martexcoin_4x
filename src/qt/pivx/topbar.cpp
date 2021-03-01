// Copyright (c) 2019-2020 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "qt/pivx/topbar.h"
#include "qt/pivx/forms/ui_topbar.h"
#include <QPixmap>
#include "qt/pivx/lockunlock.h"
#include "qt/pivx/qtutils.h"
#include "qt/pivx/receivedialog.h"
#include "askpassphrasedialog.h"

#include "bitcoinunits.h"
#include "clientmodel.h"
#include "checkpoints.h"
#include "qt/guiconstants.h"
#include "qt/guiutil.h"
#include "optionsmodel.h"
#include "qt/platformstyle.h"
#include "wallet/wallet.h"
#include "walletmodel.h"
#include "addresstablemodel.h"
#include "guiinterface.h"

#include "chainparams.h"


TopBar::TopBar(MARTEXGUI* _mainWindow, QWidget *parent) :
    PWidget(_mainWindow, parent),
    ui(new Ui::TopBar)
{
    ui->setupUi(this);

    // Set parent stylesheet
    this->setStyleSheet(_mainWindow->styleSheet());
    /* Containers */
    ui->containerTop->setContentsMargins(10, 4, 10, 10);
#ifdef Q_OS_MAC
    ui->containerTop->load("://bg-dashboard-banner");
    setCssProperty(ui->containerTop,"container-topbar-no-image");
#else
    ui->containerTop->setProperty("cssClass", "container-top");
#endif

    std::initializer_list<QWidget*> lblTitles = {ui->labelTitle1, ui->labelTitle3, ui->labelTitle4};
    setCssProperty(lblTitles, "text-title-topbar");
    QFont font;
    font.setWeight(QFont::Light);
    Q_FOREACH (QWidget* w, lblTitles) { w->setFont(font); }

    // Amount information top
    ui->widgetTopAmount->setVisible(false);
    setCssProperty({ui->labelAmountTopMXT}, "amount-small-topbar");
    setCssProperty({ui->labelAmountMXT}, "amount-topbar");
    setCssProperty({ui->labelPendingMXT, ui->labelLockedMXT}, "amount-small-topbar");

    // Progress Sync
    progressBar = new QProgressBar(ui->layoutSync);
    progressBar->setRange(1, 10);
    progressBar->setValue(4);
    progressBar->setTextVisible(false);
    progressBar->setMaximumHeight(2);
    progressBar->setMaximumWidth(36);
    setCssProperty(progressBar, "progress-sync");
    progressBar->show();
    progressBar->raise();
    progressBar->move(0, 34);

    // New button
    ui->pushButtonFAQ->setButtonClassStyle("cssClass", "btn-check-faq");
    ui->pushButtonFAQ->setButtonText("FAQ");

    ui->pushButtonConnection->setButtonClassStyle("cssClass", "btn-check-connect-inactive");
    ui->pushButtonConnection->setButtonText("No Connection");

    ui->pushButtonTor->setButtonClassStyle("cssClass", "btn-check-tor-inactive");
    ui->pushButtonTor->setButtonText("Tor Disabled");
    ui->pushButtonTor->setChecked(false);
    ui->pushButtonTor->setVisible(false);

    ui->pushButtonStack->setButtonClassStyle("cssClass", "btn-check-stack-inactive");
    ui->pushButtonStack->setButtonText("Staking Disabled");

    ui->pushButtonColdStaking->setButtonClassStyle("cssClass", "btn-check-cold-staking-inactive");
    ui->pushButtonColdStaking->setButtonText("Cold Staking Disabled");

    ui->pushButtonSync->setButtonClassStyle("cssClass", "btn-check-sync");
    ui->pushButtonSync->setButtonText(" %54 Synchronizing..");

    ui->pushButtonHDEnabled->setButtonClassStyle("cssClass", "btn-check-hd-inactive");
    ui->pushButtonHDEnabled->setButtonText("HD is Disabled");

    ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-lock");

    if(isLightTheme()){
        ui->pushButtonTheme->setButtonClassStyle("cssClass", "btn-check-theme-light");
        ui->pushButtonTheme->setButtonText("Light Theme");
    }else{
        ui->pushButtonTheme->setButtonClassStyle("cssClass", "btn-check-theme-dark");
        ui->pushButtonTheme->setButtonText("Dark Theme");
    }

    setCssProperty(ui->qrContainer, "container-qr");
    setCssProperty(ui->pushButtonQR, "btn-qr");

    // QR image
    QPixmap pixmap("://img-qr-test");
    ui->btnQr->setIcon(
                QIcon(pixmap.scaled(
                         70,
                         70,
                         Qt::KeepAspectRatio))
                );

    ui->pushButtonLock->setButtonText("Wallet Locked  ");
    ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-status-lock");


    connect(ui->pushButtonQR, SIGNAL(clicked()), this, SLOT(onBtnReceiveClicked()));
    connect(ui->btnQr, SIGNAL(clicked()), this, SLOT(onBtnReceiveClicked()));
    connect(ui->pushButtonLock, SIGNAL(Mouse_Pressed()), this, SLOT(onBtnLockClicked()));
    connect(ui->pushButtonTheme, SIGNAL(Mouse_Pressed()), this, SLOT(onThemeClicked()));
    connect(ui->pushButtonFAQ, SIGNAL(Mouse_Pressed()), _mainWindow, SLOT(openFAQ()));
    connect(ui->pushButtonColdStaking, SIGNAL(Mouse_Pressed()), this, SLOT(onColdStakingClicked()));
    connect(ui->pushButtonHDEnabled, SIGNAL(Mouse_Pressed()), this, SLOT(onHDEnabledClicked()));
    connect(ui->pushButtonSync, &ExpandableButton::Mouse_HoverLeave, this, &TopBar::refreshProgressBarSize);
    connect(ui->pushButtonSync, &ExpandableButton::Mouse_Hover, this, &TopBar::refreshProgressBarSize);
    connect(ui->pushButtonSync, &ExpandableButton::Mouse_Pressed, [this](){window->goToSettingsInfo();});
    connect(ui->pushButtonConnection, &ExpandableButton::Mouse_Pressed, [this](){window->openNetworkMonitor();});

    updateHDStatus();
}

void TopBar::onThemeClicked(){
    // Store theme
    bool lightTheme = !isLightTheme();

    setTheme(lightTheme);

    if(lightTheme){
        ui->pushButtonTheme->setButtonClassStyle("cssClass", "btn-check-theme-light",  true);
        ui->pushButtonTheme->setButtonText("Light Theme");
    }else{
        ui->pushButtonTheme->setButtonClassStyle("cssClass", "btn-check-theme-dark", true);
        ui->pushButtonTheme->setButtonText("Dark Theme");
    }
    updateStyle(ui->pushButtonTheme);

    Q_EMIT themeChanged(lightTheme);
}

void TopBar::onBtnLockClicked(){
    if(walletModel) {
        if (walletModel->getEncryptionStatus() == WalletModel::Unencrypted) {
            encryptWallet();
        } else {
            if (!lockUnlockWidget) {
                lockUnlockWidget = new LockUnlock(window);
                lockUnlockWidget->setStyleSheet("margin:0px; padding:0px;");
                connect(lockUnlockWidget, SIGNAL(Mouse_Leave()), this, SLOT(lockDropdownMouseLeave()));
                connect(ui->pushButtonLock, &ExpandableButton::Mouse_HoverLeave, [this](){
                    QMetaObject::invokeMethod(this, "lockDropdownMouseLeave", Qt::QueuedConnection);
                });
                connect(lockUnlockWidget, SIGNAL(lockClicked(
                const StateClicked&)),this, SLOT(lockDropdownClicked(
                const StateClicked&)));
            }

            lockUnlockWidget->updateStatus(walletModel->getEncryptionStatus());
            if (ui->pushButtonLock->width() <= 40) {
                ui->pushButtonLock->setExpanded();
            }
            // Keep it open
            ui->pushButtonLock->setKeepExpanded(true);
            QMetaObject::invokeMethod(this, "openLockUnlock", Qt::QueuedConnection);
        }
    }
}

void TopBar::openLockUnlock(){
    lockUnlockWidget->setFixedWidth(ui->pushButtonLock->width());
    lockUnlockWidget->adjustSize();

    lockUnlockWidget->move(
            ui->pushButtonLock->pos().rx() + window->getNavWidth() + 10,
            ui->pushButtonLock->y() + 36
    );

    lockUnlockWidget->raise();
    lockUnlockWidget->activateWindow();
    lockUnlockWidget->show();
}

void TopBar::openPassPhraseDialog(AskPassphraseDialog::Mode mode, AskPassphraseDialog::Context ctx) {
    if (!walletModel)
        return;

    showHideOp(true);
    AskPassphraseDialog *dlg = new AskPassphraseDialog(mode, window, walletModel, ctx);
    dlg->adjustSize();
    openDialogWithOpaqueBackgroundY(dlg, window);

    refreshStatus();
    dlg->deleteLater();
}

void TopBar::encryptWallet()
{
    return openPassPhraseDialog(AskPassphraseDialog::Mode::Encrypt, AskPassphraseDialog::Context::Encrypt);
}

void TopBar::unlockWallet()
{
    if(!walletModel)
        return;
    // Unlock wallet when requested by wallet model (if unlocked or unlocked for staking only)
    if (walletModel->isWalletLocked(false))
        return openPassPhraseDialog(AskPassphraseDialog::Mode::Unlock, AskPassphraseDialog::Context::Unlock_Full);
}

static bool isExecuting = false;
void TopBar::lockDropdownClicked(const StateClicked& state){
    lockUnlockWidget->close();
    if(walletModel && !isExecuting) {
        isExecuting = true;

        switch (lockUnlockWidget->lock) {
            case 0: {
                if (walletModel->getEncryptionStatus() == WalletModel::Locked)
                    break;
                walletModel->setWalletLocked(true);
                ui->pushButtonLock->setButtonText("Wallet Locked");
                ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-status-lock", true);
                // Directly update the staking status icon when the wallet is manually locked here
                // so the feedback is instant (no need to wait for the polling timeout)
                setStakingStatusActive(false);
                break;
            }
            case 1: {
                if (walletModel->getEncryptionStatus() == WalletModel::Unlocked)
                    break;
                showHideOp(true);
                AskPassphraseDialog *dlg = new AskPassphraseDialog(AskPassphraseDialog::Mode::Unlock, window, walletModel,
                                        AskPassphraseDialog::Context::ToggleLock);
                dlg->adjustSize();
                openDialogWithOpaqueBackgroundY(dlg, window);
                if (walletModel->getEncryptionStatus() == WalletModel::Unlocked) {
                    ui->pushButtonLock->setButtonText("Wallet Unlocked");
                    ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-status-unlock", true);
                }
                dlg->deleteLater();
                break;
            }
            case 2: {
                WalletModel::EncryptionStatus status = walletModel->getEncryptionStatus();
                if (status == WalletModel::UnlockedForStaking)
                    break;

                if (status == WalletModel::Unlocked) {
                    walletModel->lockForStakingOnly();
                } else {
                    showHideOp(true);
                    AskPassphraseDialog *dlg = new AskPassphraseDialog(AskPassphraseDialog::Mode::UnlockAnonymize,
                                                                       window, walletModel,
                                                                       AskPassphraseDialog::Context::ToggleLock);
                    dlg->adjustSize();
                    openDialogWithOpaqueBackgroundY(dlg, window);
                    dlg->deleteLater();
                }
                if (walletModel->getEncryptionStatus() == WalletModel::UnlockedForStaking) {
                    ui->pushButtonLock->setButtonText(tr("Wallet Unlocked for staking"));
                    ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-status-staking", true);
                }
                break;
            }
        }

        ui->pushButtonLock->setKeepExpanded(false);
        ui->pushButtonLock->setSmall();
        ui->pushButtonLock->update();

        isExecuting = false;
    }
}

void TopBar::lockDropdownMouseLeave(){
    if (lockUnlockWidget->isVisible() && !lockUnlockWidget->isHovered()) {
        lockUnlockWidget->hide();
        ui->pushButtonLock->setKeepExpanded(false);
        ui->pushButtonLock->setSmall();
        ui->pushButtonLock->update();
    }
}

void TopBar::onBtnReceiveClicked(){
    if(walletModel) {
        QString addressStr = walletModel->getAddressTableModel()->getAddressToShow();
        if (addressStr.isNull()) {
            inform(tr("Error generating address"));
            return;
        }
        showHideOp(true);
        ReceiveDialog *receiveDialog = new ReceiveDialog(window);
        receiveDialog->updateQr(addressStr);
        if (openDialogWithOpaqueBackground(receiveDialog, window)) {
            inform(tr("Address Copied"));
        }
        receiveDialog->deleteLater();
    }
}

void TopBar::showTop(){
    if(ui->bottom_container->isVisible()){
        ui->bottom_container->setVisible(false);
        ui->widgetTopAmount->setVisible(true);
        this->setFixedHeight(75);
    }
}

void TopBar::showBottom(){
    ui->widgetTopAmount->setVisible(false);
    ui->bottom_container->setVisible(true);
    this->setFixedHeight(200);
    this->adjustSize();
}

void TopBar::onColdStakingClicked() {

    if (!walletModel->hdEnabled()) {
        bool isColdStakingEnabled = walletModel->isColdStaking();
        ui->pushButtonColdStaking->setChecked(isColdStakingEnabled);

        bool show = (isInitializing) ? walletModel->getOptionsModel()->isColdStakingScreenEnabled() :
                    walletModel->getOptionsModel()->invertColdStakingScreenStatus();
        QString className;
        QString text;

        if (isColdStakingEnabled) {
            text = "Cold Staking Active";
            className = (show) ? "btn-check-cold-staking-checked" : "btn-check-cold-staking-unchecked";
        } else if (show) {
            className = "btn-check-cold-staking";
            text = "Cold Staking Enabled";
        } else {
            className = "btn-check-cold-staking-inactive";
            text = "Cold Staking Disabled";
        }

        ui->pushButtonColdStaking->setButtonClassStyle("cssClass", className, true);
        ui->pushButtonColdStaking->setButtonText(text);
        updateStyle(ui->pushButtonColdStaking);

        Q_EMIT onShowHideColdStakingChanged(show);
    } else {
        inform(tr("Can't use cold staking with HD till supported"));
    }
}

TopBar::~TopBar(){
    if(timerStakingIcon){
        timerStakingIcon->stop();
    }
    if(timerHDIcon){
        timerHDIcon->stop();
    }
    delete ui;
}

void TopBar::loadClientModel(){
    if(clientModel){
        // Keep up to date with client
        setNumConnections(clientModel->getNumConnections());
        connect(clientModel, SIGNAL(numConnectionsChanged(int)), this, SLOT(setNumConnections(int)));

        setNumBlocks(clientModel->getNumBlocks());
        connect(clientModel, SIGNAL(numBlocksChanged(int)), this, SLOT(setNumBlocks(int)));

        timerStakingIcon = new QTimer(ui->pushButtonStack);
        connect(timerStakingIcon, SIGNAL(timeout()), this, SLOT(updateStakingStatus()));
        timerStakingIcon->start(50000);
        updateStakingStatus();

        timerHDIcon = new QTimer(ui->pushButtonHDEnabled);
        connect(timerHDIcon, SIGNAL(timeout()), this, SLOT(updateHDStatus()));
        timerHDIcon->start(10000);
        updateHDStatus();
    }
}

void TopBar::setStakingStatusActive(bool fActive)
{
    if (ui->pushButtonStack->isChecked() != fActive) {
        ui->pushButtonStack->setButtonText(fActive ? tr("Staking active") : tr("Staking not active"));
        ui->pushButtonStack->setChecked(fActive);
        ui->pushButtonStack->setButtonClassStyle("cssClass", (fActive ?
                                                                "btn-check-stack" :
                                                                "btn-check-stack-inactive"), true);
    }
}

void TopBar::updateStakingStatus(){
    setStakingStatusActive(walletModel &&
                           !walletModel->isWalletLocked() &&
                           walletModel->isStakingStatusActive());

    // Taking advantage of this timer to update Tor status if needed.
    updateTorIcon();
}

void TopBar::setHDStatus(bool fActive)
{
    if (ui->pushButtonHDEnabled->isChecked() != fActive) {
        ui->pushButtonHDEnabled->setButtonText(fActive ? tr("HD active") : tr("HD not active"));
        ui->pushButtonHDEnabled->setChecked(fActive);
        ui->pushButtonHDEnabled->setButtonClassStyle("cssClass", (fActive ?
                                                              "btn-check-hd" :
                                                              "btn-check-hd-inactive"), true);
    }
}

void TopBar::updateHDStatus(){
    setHDStatus(walletModel && walletModel->hdEnabled());

    if (walletModel && walletModel->hdEnabled()) {
        timerHDIcon->stop();
    }
}

void TopBar::setNumConnections(int count) {
    if(count > 0){
        if(!ui->pushButtonConnection->isChecked()) {
            ui->pushButtonConnection->setChecked(true);
            ui->pushButtonConnection->setButtonClassStyle("cssClass", "btn-check-connect", true);
        }
    }else{
        if(ui->pushButtonConnection->isChecked()) {
            ui->pushButtonConnection->setChecked(false);
            ui->pushButtonConnection->setButtonClassStyle("cssClass", "btn-check-connect-inactive", true);
        }
    }

    ui->pushButtonConnection->setButtonText(tr("%n active connection(s)", "", count));
}

void TopBar::setNumBlocks(int count) {
    if (!clientModel)
        return;

    // Acquire current block source
    enum BlockSource blockSource = clientModel->getBlockSource();
    std::string text = "";
    switch (blockSource) {
        case BLOCK_SOURCE_NETWORK:
            text = "Synchronizing..";
            break;
        case BLOCK_SOURCE_DISK:
            text = "Importing blocks from disk..";
            break;
        case BLOCK_SOURCE_REINDEX:
            text = "Reindexing blocks on disk..";
            break;
        case BLOCK_SOURCE_NONE:
            // Case: not Importing, not Reindexing and no network connection
            text = "No block source available..";
            ui->pushButtonSync->setChecked(false);
            break;
    }

    bool needState = true;
    if (masternodeSync.IsBlockchainSynced()) {
        // chain synced
        Q_EMIT walletSynced(true);
        if (masternodeSync.IsSynced()) {
            // Node synced
            ui->pushButtonSync->setButtonText(tr("Synchronized - Block: %1").arg(QString::number(count)));
            progressBar->setRange(0,100);
            progressBar->setValue(100);
            return;
        } else {

            // TODO: Show out of sync warning
            int nAttempt = masternodeSync.RequestedMasternodeAttempt < MASTERNODE_SYNC_THRESHOLD ?
                       masternodeSync.RequestedMasternodeAttempt + 1 :
                       MASTERNODE_SYNC_THRESHOLD;
            int progress = nAttempt + (masternodeSync.RequestedMasternodeAssets - 1) * MASTERNODE_SYNC_THRESHOLD;
            if(progress >= 0){
                // todo: MN progress..
                text = strprintf("%s - Block: %d", masternodeSync.GetSyncStatus(), count);
                //progressBar->setMaximum(4 * MASTERNODE_SYNC_THRESHOLD);
                //progressBar->setValue(progress);
                needState = false;
            }
        }
    } else {
        Q_EMIT walletSynced(false);
    }

    if(needState) {
        // Currently using alot assumptions in this 'sync estimator'...
        // (E.g: This assumes each calculation is at 1-sec intervals, when it slightly differs in reality)
        int blocksPerSec = 0;
        if (!vBlocksPerSec.empty())
            blocksPerSec = count - lastBlockCount;
        else
            blocksPerSec = 50;
        vBlocksPerSec.push_back(blocksPerSec);
        lastBlockCount = count;

        // We only store the last 10 calculations, no need for more than this
        // The more blocks we store, the more accurate this calculation becomes
        if (vBlocksPerSec.size() > 10) {
            vBlocksPerSec.pop_back();
        } else if (vBlocksPerSec.size() <= 2) {
            // Not enough blocks to calculate an average with... Use a dummy number
            blocksPerSec = 50;
        }

        // (If capable) Now calculate the average blocks over the stored blocks
        if (vBlocksPerSec.size() > 2)
            blocksPerSec = std::round(1 * std::accumulate(vBlocksPerSec.begin(), vBlocksPerSec.end(), 0) / vBlocksPerSec.size());

        // Now to calculate the human-readable time-til-synced
        // Reduced to 70% to display more 'optimistic' syncing times
        long millisecsUntilSynced = (blocksPerSec * (Checkpoints::GetTotalBlocksEstimate() - count)) * 0.7;
        // 3600000 milliseconds in an hour
        long hr = millisecsUntilSynced / 3600000;
        millisecsUntilSynced = millisecsUntilSynced - 3600000 * hr;
        // 60000 milliseconds in a minute
        long min = millisecsUntilSynced / 60000;
        millisecsUntilSynced = millisecsUntilSynced - 60000 * min;
        // 1000 milliseconds in a second
        long sec = millisecsUntilSynced / 1000;
        millisecsUntilSynced = millisecsUntilSynced - 1000 * sec;

        QString timeBehindText;
        if (vBlocksPerSec.size() <= 2) {
            timeBehindText = tr("Starting Sync");
        } else if (hr <= 0 && min <= 0 && sec <= 0) {
            timeBehindText = tr("Finishing Sync");
        } else {
            // To prevent cutting off the "tailing zero" we stringify the results and append it to a "0" if below 10
            QString secStr = sec < 10 ? "0" + QString::number(sec) : QString::number(sec);
            QString minStr = min < 10 ? "0" + QString::number(min) : QString::number(min);
            QString hrStr = hr < 10 ? "0" + QString::number(hr) : QString::number(hr);
            timeBehindText = tr("%1:%2:%3 left").arg(hrStr).arg(minStr).arg(secStr);
        }
        QString timeBehind(" - Scanning block ");
        QString str = timeBehindText + timeBehind + QString::number(count);
        text = str.toStdString();

        progressBar->setMaximum(1000000000);
        progressBar->setValue(clientModel->getVerificationProgress() * 1000000000.0 + 0.5);
    }

    if(text.empty()){
        text = "No block source available..";
    }

    ui->pushButtonSync->setButtonText(tr(text.data()));
}

void TopBar::loadWalletModel() {
    connect(walletModel, SIGNAL(balanceChanged(CAmount, CAmount, CAmount, CAmount, CAmount, CAmount, CAmount, CAmount, CAmount)), this,
                           SLOT(updateBalances(CAmount, CAmount, CAmount, CAmount, CAmount, CAmount, CAmount, CAmount, CAmount)));
    connect(walletModel->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));
    connect(walletModel, &WalletModel::encryptionStatusChanged, this, &TopBar::refreshStatus);
    // Ask for passphrase if needed
    connect(walletModel, &WalletModel::requireUnlock, this, &TopBar::unlockWallet);
    // update the display unit, to not use the default ("MXT")
    updateDisplayUnit();

    refreshStatus();
    onColdStakingClicked();

    isInitializing = false;
}

void TopBar::updateTorIcon() {
    std::string ip_port;
    bool torEnabled = clientModel->getTorInfo(ip_port);

    if (torEnabled) {
        ui->pushButtonTor->setVisible(true);
        if(!ui->pushButtonTor->isChecked()) {
            ui->pushButtonTor->setChecked(true);
            ui->pushButtonTor->setButtonClassStyle("cssClass", "btn-check-tor", true);
        }
        QString ip_port_q = QString::fromStdString(ip_port);
        ui->pushButtonTor->setButtonText(tr("Tor is <b>enabled</b>: %1").arg(ip_port_q));
    } else {
        if (ui->pushButtonTor->isChecked()) {
            ui->pushButtonTor->setChecked(false);
            ui->pushButtonTor->setButtonClassStyle("cssClass", "btn-check-tor-inactive", true);
        }
        ui->pushButtonTor->setVisible(false);
    }
}

void TopBar::refreshStatus(){
    // Check lock status
    if (!this->walletModel)
        return;

    WalletModel::EncryptionStatus encStatus = walletModel->getEncryptionStatus();

    switch (encStatus){
        case WalletModel::EncryptionStatus::Unencrypted:
            ui->pushButtonLock->setButtonText("Wallet Unencrypted");
            ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-status-unlock", true);
            break;
        case WalletModel::EncryptionStatus::Locked:
            ui->pushButtonLock->setButtonText("Wallet Locked");
            ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-status-lock", true);
            break;
        case WalletModel::EncryptionStatus::UnlockedForStaking:
            ui->pushButtonLock->setButtonText("Wallet Unlocked for staking");
            ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-status-staking", true);
            break;
        case WalletModel::EncryptionStatus::Unlocked:
            ui->pushButtonLock->setButtonText("Wallet Unlocked");
            ui->pushButtonLock->setButtonClassStyle("cssClass", "btn-check-status-unlock", true);
            break;
    }
    updateStyle(ui->pushButtonLock);
}

void TopBar::updateDisplayUnit() {
    if (walletModel && walletModel->getOptionsModel()) {
        int displayUnitPrev = nDisplayUnit;
        nDisplayUnit = walletModel->getOptionsModel()->getDisplayUnit();
        if (displayUnitPrev != nDisplayUnit)
            updateBalances(walletModel->getBalance(), walletModel->getLockedBalance(), walletModel->getUnconfirmedBalance(), walletModel->getImmatureBalance(),
                           walletModel->getWatchBalance(), walletModel->getWatchUnconfirmedBalance(), walletModel->getWatchImmatureBalance(),
                           walletModel->getDelegatedBalance(), walletModel->getColdStakedBalance());
    }
}

void TopBar::updateBalances(const CAmount& balance, const CAmount& lockedBalance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance,
                            const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance,
                            const CAmount& delegatedBalance, const CAmount& coldStakedBalance) {

    ui->labelTitle1->setText(tr("Available"));

    /* MXT Total */
    // MARTEX excludes "locked" MXT from the Available balance to improve UX
    CAmount znzAvailableBalance = balance - lockedBalance;
    QString totalMXT = GUIUtil::formatBalance(znzAvailableBalance, nDisplayUnit);

    /* MXT Available Balance */
    // Top
    ui->labelAmountTopMXT->setText(totalMXT);
    // Expanded
    ui->labelAmountMXT->setText(totalMXT);

    /* MARTEX merged "Pending" and "Immature" into a single GUI balance, to simplify the experience for the user */
    // Locked
    ui->labelLockedMXT->setText(GUIUtil::formatBalance(lockedBalance, nDisplayUnit));
    // Pending + Immature
    ui->labelPendingMXT->setText(GUIUtil::formatBalance((unconfirmedBalance + immatureBalance), nDisplayUnit));
}

void TopBar::resizeEvent(QResizeEvent *event){
    if (lockUnlockWidget && lockUnlockWidget->isVisible()) lockDropdownMouseLeave();
    QWidget::resizeEvent(event);
}

void TopBar::refreshProgressBarSize() {
    QMetaObject::invokeMethod(this, "expandSync", Qt::QueuedConnection);
}

void TopBar::expandSync() {
    if (progressBar) {
        progressBar->setMaximumWidth(ui->pushButtonSync->maximumWidth());
        progressBar->setFixedWidth(ui->pushButtonSync->width());
        progressBar->setMinimumWidth(ui->pushButtonSync->width() - 2);
    }
}
