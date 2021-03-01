//
// Created by Kolby on 6/19/2019.
//


#include <startoptionsrevealed.h>
#include <ui_startoptionsrevealed.h>
#include "guiutil.h"
#include "qt/pivx/qtutils.h"
#include <QLineEdit>



StartOptionsRevealed::StartOptionsRevealed(std::vector<std::string>& Words, int rows, QWidget *parent)
        : QWidget(parent), ui(new Ui::StartOptionsRevealed)
        {
    ui->setupUi(this);
    /* Open CSS when configured */
    this->setStyleSheet(GUIUtil::loadStyleSheet());

    ui->gridLayoutRevealed->setProperty("cssClass", "seed-word-generator");

            for(int i=0; i<rows; i++){
                for(int k=0; k<6; k++){

                    QLabel* label = new QLabel(this);
                    if(isLightTheme()){
                        label->setStyleSheet("QLabel{background-color:transparent;padding-left:8px;padding-right:8px;border-radius:0px;color:black;border-bottom:2px solid rgb(35,136,237);}");
                    }else{
                        label->setStyleSheet("QLabel{background-color:transparent;padding-left:8px;padding-right:8px;border-radius:0px;color:#FFFFFF;border-bottom:2px solid rgb(35,136,237);}");
                    }
                    label->setMinimumSize(80,36);
                    label->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
                    label->setContentsMargins(8,12,8,12);
                    label->setAlignment(Qt::AlignCenter);
                    labelsList.push_back(label);
                    ui->gridLayoutRevealed->addWidget(label, i, k, Qt::AlignCenter);
                }
            }
            int i = 0;
            for (QLabel* label : labelsList) {
                if(isLightTheme()){
                    label->setStyleSheet("QLabel{background-color:transparent;padding-left:8px;padding-right:8px;border-radius:0px;color:black;border-bottom:2px solid rgb(35,136,237);}");
                }else{
                    label->setStyleSheet("QLabel{background-color:transparent;padding-left:8px;padding-right:8px;border-radius:0px;color:#FFFFFF;border-bottom:2px solid rgb(35,136,237);}");
                }
                label->setContentsMargins(8,12,8,12);
                label->setText(QString::fromStdString(Words[i]));
                i++;
            }


}

StartOptionsRevealed::~StartOptionsRevealed() {
    delete ui;
}

