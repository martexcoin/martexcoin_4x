#if defined(HAVE_CONFIG_H)
#include <config/martex-config.h>
#endif

#include <startoptionsdialog.h>
#include <ui_startoptionsdialog.h>
#include "guiutil.h"




#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>

StartOptionsDialog::StartOptionsDialog(const QString error_words, QWidget* parent)
    : QDialog(parent), ui(new Ui::StartOptionsDialog) {
    ui->setupUi(this);

    /* Open CSS when configured */
    this->setStyleSheet(GUIUtil::loadStyleSheet());

    ui->ErrorLable->setText(error_words);
}

StartOptionsDialog::~StartOptionsDialog() {
    delete ui;
}

void StartOptionsDialog::accept() {
    close();
}
