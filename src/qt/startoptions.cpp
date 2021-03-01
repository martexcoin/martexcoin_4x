//
// Created by Kolby on 6/19/2019.
//


#include <startoptions.h>
#include <ui_startoptions.h>
#include "guiutil.h"




StartOptions::StartOptions(QWidget *parent)
        : QWidget(parent), ui(new Ui::StartOptions)
        {
    ui->setupUi(this);
    /* Open CSS when configured */
    this->setStyleSheet(GUIUtil::loadStyleSheet());
    ui->verticalLayout->setProperty("cssClass", "seed-word-generator");


}

int StartOptions::getRows(){
    rows = 4;
    return rows;
};

StartOptions::~StartOptions() {
    delete ui;
}
