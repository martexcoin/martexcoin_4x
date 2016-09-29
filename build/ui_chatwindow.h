/********************************************************************************
** Form generated from reading UI file 'chatwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATWINDOW_H
#define UI_CHATWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatWindowClass
{
public:
    QAction *actionQuit;
    QAction *actionCloseTab;
    QWidget *widget_2;
    QWidget *splitter;
    QTabWidget *tab;
    QWidget *widget_4;
    QWidget *tab_5;
    QLineEdit *lineEdit;
    QListView *userView;
    QPushButton *disconnect;
    QWidget *hide3;
    QWidget *layoutWidget;
    QHBoxLayout *hide1;
    QLabel *hide2;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *editPseudo;
    QLabel *label_5;
    QPushButton *buttonConnect;
    QLabel *label_10;

    void setupUi(QWidget *ChatWindowClass)
    {
        if (ChatWindowClass->objectName().isEmpty())
            ChatWindowClass->setObjectName(QStringLiteral("ChatWindowClass"));
        ChatWindowClass->resize(743, 514);
        actionQuit = new QAction(ChatWindowClass);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        actionCloseTab = new QAction(ChatWindowClass);
        actionCloseTab->setObjectName(QStringLiteral("actionCloseTab"));
        widget_2 = new QWidget(ChatWindowClass);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        widget_2->setGeometry(QRect(0, 0, 741, 511));
        splitter = new QWidget(widget_2);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setEnabled(true);
        splitter->setGeometry(QRect(10, 10, 721, 501));
        splitter->setStyleSheet(QStringLiteral(""));
        tab = new QTabWidget(splitter);
        tab->setObjectName(QStringLiteral("tab"));
        tab->setGeometry(QRect(10, 0, 541, 451));
        tab->setStyleSheet(QStringLiteral(""));
        tab->setTabShape(QTabWidget::Rounded);
        tab->setTabsClosable(false);
        widget_4 = new QWidget();
        widget_4->setObjectName(QStringLiteral("widget_4"));
        tab->addTab(widget_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QStringLiteral("tab_5"));
        tab->addTab(tab_5, QString());
        lineEdit = new QLineEdit(splitter);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(10, 460, 701, 30));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lineEdit->sizePolicy().hasHeightForWidth());
        lineEdit->setSizePolicy(sizePolicy);
        lineEdit->setMaximumSize(QSize(16777215, 30));
        userView = new QListView(splitter);
        userView->setObjectName(QStringLiteral("userView"));
        userView->setGeometry(QRect(560, 20, 161, 391));
        userView->setMinimumSize(QSize(0, 300));
        userView->setStyleSheet(QStringLiteral("border-radius:10px;"));
        disconnect = new QPushButton(splitter);
        disconnect->setObjectName(QStringLiteral("disconnect"));
        disconnect->setGeometry(QRect(642, 420, 71, 28));
        hide3 = new QWidget(widget_2);
        hide3->setObjectName(QStringLiteral("hide3"));
        hide3->setGeometry(QRect(0, 0, 741, 511));
        hide3->setStyleSheet(QStringLiteral("font-family:Proxima Nova Rg;"));
        layoutWidget = new QWidget(hide3);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 6, 731, 41));
        hide1 = new QHBoxLayout(layoutWidget);
        hide1->setSpacing(7);
        hide1->setContentsMargins(11, 11, 11, 11);
        hide1->setObjectName(QStringLiteral("hide1"));
        hide1->setContentsMargins(0, 0, 0, 0);
        hide2 = new QLabel(hide3);
        hide2->setObjectName(QStringLiteral("hide2"));
        hide2->setGeometry(QRect(60, 70, 631, 91));
        hide2->setWordWrap(true);
        hide2->setOpenExternalLinks(true);
        label = new QLabel(hide3);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(70, 380, 71, 51));
        label->setMargin(5);
        label->setOpenExternalLinks(true);
        label_2 = new QLabel(hide3);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(60, 160, 291, 211));
        editPseudo = new QLineEdit(hide3);
        editPseudo->setObjectName(QStringLiteral("editPseudo"));
        editPseudo->setGeometry(QRect(470, 250, 151, 22));
        label_5 = new QLabel(hide3);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(370, 250, 61, 21));
        buttonConnect = new QPushButton(hide3);
        buttonConnect->setObjectName(QStringLiteral("buttonConnect"));
        buttonConnect->setGeometry(QRect(470, 300, 151, 41));
        buttonConnect->setStyleSheet(QStringLiteral(""));
        label_10 = new QLabel(hide3);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(370, 200, 161, 21));

        retranslateUi(ChatWindowClass);

        tab->setCurrentIndex(0);
        buttonConnect->setDefault(true);


        QMetaObject::connectSlotsByName(ChatWindowClass);
    } // setupUi

    void retranslateUi(QWidget *ChatWindowClass)
    {
        actionQuit->setText(QApplication::translate("ChatWindowClass", "Quitter", 0));
        actionQuit->setShortcut(QApplication::translate("ChatWindowClass", "Ctrl+Q", 0));
        actionCloseTab->setText(QApplication::translate("ChatWindowClass", "Fermer l'onglet", 0));
        tab->setTabText(tab->indexOf(widget_4), QApplication::translate("ChatWindowClass", "Tab 1", 0));
        tab->setTabText(tab->indexOf(tab_5), QApplication::translate("ChatWindowClass", "Tab 2", 0));
        disconnect->setText(QApplication::translate("ChatWindowClass", "Disconnect", 0));
        hide2->setText(QApplication::translate("ChatWindowClass", "<html><head/><body><p align=\"justify\"><span style=\" font-size:9pt;\">Connect directly to the IRC channel from your wallet by inputing a Nickname here and pressing 'Connect to IRC'</span></p></body></html>", 0));
        label->setText(QApplication::translate("ChatWindowClass", "<html><head/><body><p align=\"center\"><a href=\"https://twitter.com/Martexcoin\"><span style=\" text-decoration: underline; color:#0000ff;\"><img src=\":/icons/twitter\"/></span></a></p></body></html>", 0));
        label_2->setText(QApplication::translate("ChatWindowClass", "<html><head/><body><p>A few IRC commands :</p><ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\"><li style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">/JOIN #channel <span style=\" color:#9a9a9a;\">Join a channel</span></li><li style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">/NICK nickname <span style=\" color:#9a9a9a;\">Change nickname</span></li><li style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">/PRIVMSG nickname message <span style=\" color:#9a9a9a;\">Send PM </span></li><li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">If userlist gets buggy, switch tabs to fix</li></ul></body></html>", 0));
        editPseudo->setText(QString());
        label_5->setText(QApplication::translate("ChatWindowClass", "Nickname", 0));
        buttonConnect->setText(QApplication::translate("ChatWindowClass", "Connect to IRC", 0));
        label_10->setText(QApplication::translate("ChatWindowClass", "<html><head/><body><p><span style=\" font-size:12pt;\">Connect to IRC :</span></p></body></html>", 0));
        Q_UNUSED(ChatWindowClass);
    } // retranslateUi

};

namespace Ui {
    class ChatWindowClass: public Ui_ChatWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATWINDOW_H
