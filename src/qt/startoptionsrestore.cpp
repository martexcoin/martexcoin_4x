//
// Created by Kolby on 6/19/2019.
//


#include <startoptionsrestore.h>
#include <ui_startoptionsrestore.h>
#include "guiutil.h"
#include "qt/pivx/qtutils.h"
#include <QLineEdit>
#include <QLabel>




QString editLineCorrectCss = "QLineEdit{border:1px solid #00aeff;}";
QString editLineInvalidCss = "QLineEdit{border:1px solid red;}";

StartOptionsRestore::StartOptionsRestore(QStringList _wordList, int rows, QWidget *parent)
        : QWidget(parent), wordList(_wordList), ui(new Ui::StartOptionsRestore)
{
    ui->setupUi(this);

    /* Open CSS when configured */
    this->setStyleSheet(GUIUtil::loadStyleSheet());

    for(int i=0; i<rows; i++){
        for(int k=0; k<6; k++){

            QLineEdit* label = new QLineEdit(this);
            label->setStyleSheet("QLabel{background-color:#808080; padding:0px; margin:0px;}");
            label->setMinimumSize(80,36);
            label->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            label->setContentsMargins(0,0,0,0);
            label->setAlignment(Qt::AlignCenter);
            editList.push_back(label);
            connect(label, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
            ui->gridLayoutRevealed->addWidget(label, i, k, Qt::AlignCenter);
        }
    }


}

void StartOptionsRestore::textChanged(const QString &text){

    QObject *senderObj = sender();
    QLineEdit* label = static_cast<QLineEdit*>(senderObj);
    if(wordList.contains(text)){
        label->setStyleSheet(editLineCorrectCss);
    }else{
        label->setStyleSheet(editLineInvalidCss);
    }

}

std::vector<std::string> StartOptionsRestore::getOrderedStrings(){
    std::vector<std::string> list;
    for(QLineEdit* label : editList){
        list.push_back(label->text().toStdString());
    }
    return list;
}

StartOptionsRestore::~StartOptionsRestore() {
    delete ui;
}

