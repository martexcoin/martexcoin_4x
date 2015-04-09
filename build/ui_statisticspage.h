/********************************************************************************
** Form generated from reading UI file 'statisticspage.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STATISTICSPAGE_H
#define UI_STATISTICSPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_StatisticsPage
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer;
    QFrame *frame;
    QLabel *label;
    QLabel *labelPhasePoW;
    QLabel *labelPhasePoS;
    QFrame *gridFrame_2;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_5;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer_2;
    QFormLayout *formLayout_2;
    QLabel *label_14;
    QLabel *heightBox;
    QLabel *label_12;
    QLabel *rewardBox;
    QLabel *label_13;
    QLabel *diffBox;
    QLabel *label_9;
    QLabel *diffBox2;
    QLabel *pawrateBox;
    QLabel *label3_2;
    QLabel *localBox;
    QLabel *label1_2;
    QLabel *connectionBox;
    QLabel *label_8;
    QLabel *volumeBox;
    QLabel *label_10;
    QLabel *minBox;
    QLabel *label_11;
    QLabel *maxBox;
    QLabel *label_15;
    QLabel *cBox;
    QLabel *label_16;
    QPushButton *startButton;

    void setupUi(QWidget *StatisticsPage)
    {
        if (StatisticsPage->objectName().isEmpty())
            StatisticsPage->setObjectName(QString::fromUtf8("StatisticsPage"));
        StatisticsPage->setWindowModality(Qt::NonModal);
        StatisticsPage->resize(623, 505);
        StatisticsPage->setStyleSheet(QString::fromUtf8("color:#505050;\n"
"font-family:'Open Sans,sans-serif';"));
        gridLayout = new QGridLayout(StatisticsPage);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetMaximumSize);
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 1, 0, 1, 1);

        frame = new QFrame(StatisticsPage);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setMinimumSize(QSize(213, 0));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 70, 191, 81));
        labelPhasePoW = new QLabel(frame);
        labelPhasePoW->setObjectName(QString::fromUtf8("labelPhasePoW"));
        labelPhasePoW->setGeometry(QRect(10, 160, 191, 16));
        labelPhasePoS = new QLabel(frame);
        labelPhasePoS->setObjectName(QString::fromUtf8("labelPhasePoS"));
        labelPhasePoS->setGeometry(QRect(10, 200, 191, 16));

        gridLayout->addWidget(frame, 0, 1, 1, 1);

        gridFrame_2 = new QFrame(StatisticsPage);
        gridFrame_2->setObjectName(QString::fromUtf8("gridFrame_2"));
        gridLayout_4 = new QGridLayout(gridFrame_2);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(7);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_5 = new QLabel(gridFrame_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        QFont font;
        font.setFamily(QString::fromUtf8("Open Sans,sans-serif"));
        label_5->setFont(font);

        horizontalLayout_4->addWidget(label_5);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        gridLayout_4->addLayout(horizontalLayout_4, 1, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_4->addItem(verticalSpacer_2, 2, 0, 1, 1);

        formLayout_2 = new QFormLayout();
        formLayout_2->setSpacing(6);
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
        formLayout_2->setHorizontalSpacing(139);
        formLayout_2->setVerticalSpacing(18);
        formLayout_2->setContentsMargins(-1, -1, 0, -1);
        label_14 = new QLabel(gridFrame_2);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_14);

        heightBox = new QLabel(gridFrame_2);
        heightBox->setObjectName(QString::fromUtf8("heightBox"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Open Sans,sans-serif"));
        font1.setPointSize(9);
        heightBox->setFont(font1);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, heightBox);

        label_12 = new QLabel(gridFrame_2);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_12);

        rewardBox = new QLabel(gridFrame_2);
        rewardBox->setObjectName(QString::fromUtf8("rewardBox"));
        rewardBox->setFont(font1);

        formLayout_2->setWidget(2, QFormLayout::FieldRole, rewardBox);

        label_13 = new QLabel(gridFrame_2);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_13);

        diffBox = new QLabel(gridFrame_2);
        diffBox->setObjectName(QString::fromUtf8("diffBox"));
        diffBox->setFont(font1);

        formLayout_2->setWidget(3, QFormLayout::FieldRole, diffBox);

        label_9 = new QLabel(gridFrame_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        formLayout_2->setWidget(4, QFormLayout::LabelRole, label_9);

        diffBox2 = new QLabel(gridFrame_2);
        diffBox2->setObjectName(QString::fromUtf8("diffBox2"));
        diffBox2->setFont(font1);

        formLayout_2->setWidget(4, QFormLayout::FieldRole, diffBox2);

        pawrateBox = new QLabel(gridFrame_2);
        pawrateBox->setObjectName(QString::fromUtf8("pawrateBox"));
        pawrateBox->setFont(font1);

        formLayout_2->setWidget(5, QFormLayout::FieldRole, pawrateBox);

        label3_2 = new QLabel(gridFrame_2);
        label3_2->setObjectName(QString::fromUtf8("label3_2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label3_2->sizePolicy().hasHeightForWidth());
        label3_2->setSizePolicy(sizePolicy);

        formLayout_2->setWidget(6, QFormLayout::LabelRole, label3_2);

        localBox = new QLabel(gridFrame_2);
        localBox->setObjectName(QString::fromUtf8("localBox"));
        localBox->setFont(font1);

        formLayout_2->setWidget(6, QFormLayout::FieldRole, localBox);

        label1_2 = new QLabel(gridFrame_2);
        label1_2->setObjectName(QString::fromUtf8("label1_2"));

        formLayout_2->setWidget(7, QFormLayout::LabelRole, label1_2);

        connectionBox = new QLabel(gridFrame_2);
        connectionBox->setObjectName(QString::fromUtf8("connectionBox"));
        connectionBox->setFont(font1);

        formLayout_2->setWidget(7, QFormLayout::FieldRole, connectionBox);

        label_8 = new QLabel(gridFrame_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout_2->setWidget(8, QFormLayout::LabelRole, label_8);

        volumeBox = new QLabel(gridFrame_2);
        volumeBox->setObjectName(QString::fromUtf8("volumeBox"));
        volumeBox->setFont(font1);

        formLayout_2->setWidget(8, QFormLayout::FieldRole, volumeBox);

        label_10 = new QLabel(gridFrame_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        formLayout_2->setWidget(9, QFormLayout::LabelRole, label_10);

        minBox = new QLabel(gridFrame_2);
        minBox->setObjectName(QString::fromUtf8("minBox"));
        minBox->setFont(font1);

        formLayout_2->setWidget(9, QFormLayout::FieldRole, minBox);

        label_11 = new QLabel(gridFrame_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        formLayout_2->setWidget(10, QFormLayout::LabelRole, label_11);

        maxBox = new QLabel(gridFrame_2);
        maxBox->setObjectName(QString::fromUtf8("maxBox"));
        maxBox->setFont(font1);

        formLayout_2->setWidget(10, QFormLayout::FieldRole, maxBox);

        label_15 = new QLabel(gridFrame_2);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        formLayout_2->setWidget(11, QFormLayout::LabelRole, label_15);

        cBox = new QLabel(gridFrame_2);
        cBox->setObjectName(QString::fromUtf8("cBox"));
        cBox->setFont(font1);

        formLayout_2->setWidget(11, QFormLayout::FieldRole, cBox);

        label_16 = new QLabel(gridFrame_2);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        formLayout_2->setWidget(5, QFormLayout::LabelRole, label_16);


        gridLayout_4->addLayout(formLayout_2, 3, 0, 1, 1);


        gridLayout->addWidget(gridFrame_2, 0, 0, 1, 1);

        startButton = new QPushButton(StatisticsPage);
        startButton->setObjectName(QString::fromUtf8("startButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(startButton->sizePolicy().hasHeightForWidth());
        startButton->setSizePolicy(sizePolicy1);
        startButton->setMinimumSize(QSize(210, 30));
        startButton->setStyleSheet(QString::fromUtf8("background:lightgrey;"));
        startButton->setDefault(true);

        gridLayout->addWidget(startButton, 1, 1, 1, 1);


        retranslateUi(StatisticsPage);

        QMetaObject::connectSlotsByName(StatisticsPage);
    } // setupUi

    void retranslateUi(QWidget *StatisticsPage)
    {
        StatisticsPage->setWindowTitle(QApplication::translate("StatisticsPage", "Digging", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("StatisticsPage", "<html><head/><body><p><span style=\" font-size:12pt; font-weight:600; text-decoration: underline;\">MarteXcoin phases:</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        labelPhasePoW->setText(QApplication::translate("StatisticsPage", "labelPhasePoW", 0, QApplication::UnicodeUTF8));
        labelPhasePoS->setText(QApplication::translate("StatisticsPage", "labelPhasePoS", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("StatisticsPage", "<html><head/><body><p><span style=\" font-size:20px; font-weight:600; color:#000000;\">MarteXcoin Statistics</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("StatisticsPage", "<html><head/><body><p><span style=\" font-size:11pt;\">Block Height</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        heightBox->setText(QApplication::translate("StatisticsPage", "00000", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("StatisticsPage", "<html><head/><body><p><span style=\" font-size:11pt;\">PoW Block Reward</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        rewardBox->setText(QApplication::translate("StatisticsPage", "250", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("StatisticsPage", "<html><head/><body><p><span style=\" font-size:11pt;\">PoW difficulty</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        diffBox->setText(QApplication::translate("StatisticsPage", "0.0002", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("StatisticsPage", "<html><head/><body><p><span style=\" font-size:11pt;\">PoS difficulty</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        diffBox2->setText(QApplication::translate("StatisticsPage", "0.0002", 0, QApplication::UnicodeUTF8));
        pawrateBox->setText(QApplication::translate("StatisticsPage", "0 MH/s", 0, QApplication::UnicodeUTF8));
        label3_2->setText(QApplication::translate("StatisticsPage", "<html><head/><body><p><span style=\" font-size:11pt;\">Last block time</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        localBox->setText(QApplication::translate("StatisticsPage", "0 KH/s", 0, QApplication::UnicodeUTF8));
        label1_2->setText(QApplication::translate("StatisticsPage", "<html><head/><body><p><span style=\" font-size:11pt;\">Connections</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        connectionBox->setText(QApplication::translate("StatisticsPage", "0", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("StatisticsPage", "<html><head/><body><p><span style=\" font-size:11pt;\">Total Coin Volume</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        volumeBox->setText(QApplication::translate("StatisticsPage", "250", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("StatisticsPage", "<html><head/><body><p><span style=\" font-size:11pt;\">Total stake weight</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        minBox->setText(QApplication::translate("StatisticsPage", "0", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("StatisticsPage", "<html><head/><body><p><span style=\" font-size:11pt;\">Network stake weight</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        maxBox->setText(QApplication::translate("StatisticsPage", "0", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("StatisticsPage", "<html><head/><body><p><span style=\" font-size:11pt;\">Mining Phase</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        cBox->setText(QApplication::translate("StatisticsPage", "POW", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("StatisticsPage", "<html><head/><body><p><span style=\" font-size:11pt;\">Net Hashrate</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        startButton->setText(QApplication::translate("StatisticsPage", "Update Statistics", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class StatisticsPage: public Ui_StatisticsPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STATISTICSPAGE_H
