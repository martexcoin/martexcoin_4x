/********************************************************************************
** Form generated from reading UI file 'messagepage.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESSAGEPAGE_H
#define UI_MESSAGEPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTableView>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MessagePage
{
public:
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer;
    QLabel *labelExplanation;
    QTableView *tableView;
    QVBoxLayout *verticalLayout;
    QGroupBox *messageDetails;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *backButton;
    QLabel *labelContact;
    QLabel *contactLabel;
    QSpacerItem *horizontalSpacer_3;
    QListView *listConversation;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QTextEdit *messageEdit;
    QPushButton *newButton;
    QPushButton *sendButton;
    QPushButton *deleteButton;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *MessagePage)
    {
        if (MessagePage->objectName().isEmpty())
            MessagePage->setObjectName(QString::fromUtf8("MessagePage"));
        MessagePage->resize(793, 413);
        verticalLayout_2 = new QVBoxLayout(MessagePage);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalSpacer = new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Minimum);

        verticalLayout_2->addItem(verticalSpacer);

        labelExplanation = new QLabel(MessagePage);
        labelExplanation->setObjectName(QString::fromUtf8("labelExplanation"));
        labelExplanation->setTextFormat(Qt::PlainText);
        labelExplanation->setWordWrap(true);

        verticalLayout_2->addWidget(labelExplanation);

        tableView = new QTableView(MessagePage);
        tableView->setObjectName(QString::fromUtf8("tableView"));
        tableView->setContextMenuPolicy(Qt::CustomContextMenu);
        tableView->setTabKeyNavigation(false);
        tableView->setAlternatingRowColors(true);
        tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->setSortingEnabled(true);
        tableView->verticalHeader()->setVisible(false);

        verticalLayout_2->addWidget(tableView);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        messageDetails = new QGroupBox(MessagePage);
        messageDetails->setObjectName(QString::fromUtf8("messageDetails"));
        verticalLayout_4 = new QVBoxLayout(messageDetails);
        verticalLayout_4->setSpacing(2);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 6, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        backButton = new QPushButton(messageDetails);
        backButton->setObjectName(QString::fromUtf8("backButton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/quit"), QSize(), QIcon::Normal, QIcon::Off);
        backButton->setIcon(icon);

        horizontalLayout_3->addWidget(backButton);

        labelContact = new QLabel(messageDetails);
        labelContact->setObjectName(QString::fromUtf8("labelContact"));

        horizontalLayout_3->addWidget(labelContact);

        contactLabel = new QLabel(messageDetails);
        contactLabel->setObjectName(QString::fromUtf8("contactLabel"));

        horizontalLayout_3->addWidget(contactLabel);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        verticalLayout_4->addLayout(horizontalLayout_3);

        listConversation = new QListView(messageDetails);
        listConversation->setObjectName(QString::fromUtf8("listConversation"));

        verticalLayout_4->addWidget(listConversation);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));

        verticalLayout_4->addLayout(verticalLayout_3);


        verticalLayout->addWidget(messageDetails);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        messageEdit = new QTextEdit(MessagePage);
        messageEdit->setObjectName(QString::fromUtf8("messageEdit"));
        messageEdit->setMinimumSize(QSize(30, 0));

        horizontalLayout->addWidget(messageEdit);

        newButton = new QPushButton(MessagePage);
        newButton->setObjectName(QString::fromUtf8("newButton"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/add"), QSize(), QIcon::Normal, QIcon::Off);
        newButton->setIcon(icon1);

        horizontalLayout->addWidget(newButton);

        sendButton = new QPushButton(MessagePage);
        sendButton->setObjectName(QString::fromUtf8("sendButton"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/send"), QSize(), QIcon::Normal, QIcon::Off);
        sendButton->setIcon(icon2);

        horizontalLayout->addWidget(sendButton);

        deleteButton = new QPushButton(MessagePage);
        deleteButton->setObjectName(QString::fromUtf8("deleteButton"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/remove"), QSize(), QIcon::Normal, QIcon::Off);
        deleteButton->setIcon(icon3);

        horizontalLayout->addWidget(deleteButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(MessagePage);

        QMetaObject::connectSlotsByName(MessagePage);
    } // setupUi

    void retranslateUi(QWidget *MessagePage)
    {
        MessagePage->setWindowTitle(QApplication::translate("MessagePage", "Address Book", 0, QApplication::UnicodeUTF8));
        labelExplanation->setText(QApplication::translate("MessagePage", "These are your sent and received encrypted messages. Click on an item to read it.", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        tableView->setToolTip(QApplication::translate("MessagePage", "Click on a message to view it", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        messageDetails->setTitle(QString());
        backButton->setText(QApplication::translate("MessagePage", "&Back", 0, QApplication::UnicodeUTF8));
        labelContact->setText(QApplication::translate("MessagePage", "Contact:", 0, QApplication::UnicodeUTF8));
        contactLabel->setText(QString());
        newButton->setText(QApplication::translate("MessagePage", "&Conversation", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        sendButton->setToolTip(QApplication::translate("MessagePage", "Send a message to the current conversation", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        sendButton->setText(QApplication::translate("MessagePage", "&Send", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        deleteButton->setToolTip(QApplication::translate("MessagePage", "Delete the currently selected address from the list", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        deleteButton->setText(QApplication::translate("MessagePage", "&Delete", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MessagePage: public Ui_MessagePage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESSAGEPAGE_H
