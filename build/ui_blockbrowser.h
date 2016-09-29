/********************************************************************************
** Form generated from reading UI file 'blockbrowser.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BLOCKBROWSER_H
#define UI_BLOCKBROWSER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BlockBrowser
{
public:
    QGridLayout *gridLayout;
    QWidget *widget;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_5;
    QSpacerItem *horizontalSpacer_2;
    QWidget *layoutWidget_2;
    QGridLayout *gridLayout_3;
    QLabel *inputBox;
    QPushButton *txButton;
    QSpacerItem *verticalSpacer;
    QLabel *inputLabel;
    QLabel *pawBox;
    QLabel *valueLabel;
    QLabel *txLabel;
    QLabel *hardBox;
    QLabel *txID;
    QLabel *valueBox;
    QLabel *hardLabel;
    QSpinBox *heightBox;
    QLabel *heightLabel_2;
    QLabel *timeBox;
    QLabel *merkleLabel;
    QLabel *hashLabel;
    QLabel *feesBox;
    QLabel *pawLabel;
    QLabel *heightLabel;
    QLabel *feesLabel;
    QLabel *bitsLabel;
    QLabel *bitsBox;
    QLabel *merkleBox;
    QLabel *outputLabel;
    QLabel *outputBox;
    QLabel *nonceLabel;
    QLabel *nonceBox;
    QLabel *timeLabel;
    QLabel *hashBox;
    QLineEdit *txBox;
    QSpacerItem *verticalSpacer_2;
    QPushButton *blockButton;

    void setupUi(QWidget *BlockBrowser)
    {
        if (BlockBrowser->objectName().isEmpty())
            BlockBrowser->setObjectName(QStringLiteral("BlockBrowser"));
        BlockBrowser->setWindowModality(Qt::NonModal);
        BlockBrowser->setEnabled(true);
        BlockBrowser->resize(760, 499);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(BlockBrowser->sizePolicy().hasHeightForWidth());
        BlockBrowser->setSizePolicy(sizePolicy);
        BlockBrowser->setMinimumSize(QSize(0, 0));
        BlockBrowser->setAcceptDrops(false);
        gridLayout = new QGridLayout(BlockBrowser);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetMaximumSize);
        gridLayout->setContentsMargins(21, 21, 10, -1);
        widget = new QWidget(BlockBrowser);
        widget->setObjectName(QStringLiteral("widget"));
        layoutWidget = new QWidget(widget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 0, 699, 27));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget);
        horizontalLayout_4->setSpacing(7);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        QFont font;
        font.setFamily(QStringLiteral("Open Sans,sans-serif"));
        label_5->setFont(font);

        horizontalLayout_4->addWidget(label_5);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        layoutWidget_2 = new QWidget(widget);
        layoutWidget_2->setObjectName(QStringLiteral("layoutWidget_2"));
        layoutWidget_2->setGeometry(QRect(0, 50, 701, 400));
        gridLayout_3 = new QGridLayout(layoutWidget_2);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setSizeConstraint(QLayout::SetDefaultConstraint);
        gridLayout_3->setHorizontalSpacing(7);
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        inputBox = new QLabel(layoutWidget_2);
        inputBox->setObjectName(QStringLiteral("inputBox"));
        inputBox->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        gridLayout_3->addWidget(inputBox, 17, 1, 1, 1);

        txButton = new QPushButton(layoutWidget_2);
        txButton->setObjectName(QStringLiteral("txButton"));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        txButton->setFont(font1);
        txButton->setAutoDefault(true);

        gridLayout_3->addWidget(txButton, 12, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer, 18, 0, 1, 1);

        inputLabel = new QLabel(layoutWidget_2);
        inputLabel->setObjectName(QStringLiteral("inputLabel"));
        inputLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        gridLayout_3->addWidget(inputLabel, 17, 0, 1, 1);

        pawBox = new QLabel(layoutWidget_2);
        pawBox->setObjectName(QStringLiteral("pawBox"));

        gridLayout_3->addWidget(pawBox, 10, 1, 1, 1);

        valueLabel = new QLabel(layoutWidget_2);
        valueLabel->setObjectName(QStringLiteral("valueLabel"));

        gridLayout_3->addWidget(valueLabel, 14, 0, 1, 1);

        txLabel = new QLabel(layoutWidget_2);
        txLabel->setObjectName(QStringLiteral("txLabel"));

        gridLayout_3->addWidget(txLabel, 13, 0, 1, 1);

        hardBox = new QLabel(layoutWidget_2);
        hardBox->setObjectName(QStringLiteral("hardBox"));

        gridLayout_3->addWidget(hardBox, 9, 1, 1, 1);

        txID = new QLabel(layoutWidget_2);
        txID->setObjectName(QStringLiteral("txID"));

        gridLayout_3->addWidget(txID, 13, 1, 1, 1);

        valueBox = new QLabel(layoutWidget_2);
        valueBox->setObjectName(QStringLiteral("valueBox"));

        gridLayout_3->addWidget(valueBox, 14, 1, 1, 1);

        hardLabel = new QLabel(layoutWidget_2);
        hardLabel->setObjectName(QStringLiteral("hardLabel"));

        gridLayout_3->addWidget(hardLabel, 9, 0, 1, 1);

        heightBox = new QSpinBox(layoutWidget_2);
        heightBox->setObjectName(QStringLiteral("heightBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(heightBox->sizePolicy().hasHeightForWidth());
        heightBox->setSizePolicy(sizePolicy1);
        heightBox->setMaximum(99999999);

        gridLayout_3->addWidget(heightBox, 1, 0, 1, 1);

        heightLabel_2 = new QLabel(layoutWidget_2);
        heightLabel_2->setObjectName(QStringLiteral("heightLabel_2"));

        gridLayout_3->addWidget(heightLabel_2, 3, 0, 1, 1);

        timeBox = new QLabel(layoutWidget_2);
        timeBox->setObjectName(QStringLiteral("timeBox"));

        gridLayout_3->addWidget(timeBox, 8, 1, 1, 1);

        merkleLabel = new QLabel(layoutWidget_2);
        merkleLabel->setObjectName(QStringLiteral("merkleLabel"));

        gridLayout_3->addWidget(merkleLabel, 5, 0, 1, 1);

        hashLabel = new QLabel(layoutWidget_2);
        hashLabel->setObjectName(QStringLiteral("hashLabel"));

        gridLayout_3->addWidget(hashLabel, 4, 0, 1, 1);

        feesBox = new QLabel(layoutWidget_2);
        feesBox->setObjectName(QStringLiteral("feesBox"));

        gridLayout_3->addWidget(feesBox, 15, 1, 1, 1);

        pawLabel = new QLabel(layoutWidget_2);
        pawLabel->setObjectName(QStringLiteral("pawLabel"));

        gridLayout_3->addWidget(pawLabel, 10, 0, 1, 1);

        heightLabel = new QLabel(layoutWidget_2);
        heightLabel->setObjectName(QStringLiteral("heightLabel"));
        heightLabel->setCursor(QCursor(Qt::IBeamCursor));
        heightLabel->setMouseTracking(false);
        heightLabel->setAcceptDrops(true);
        heightLabel->setFrameShape(QFrame::NoFrame);

        gridLayout_3->addWidget(heightLabel, 3, 1, 1, 1);

        feesLabel = new QLabel(layoutWidget_2);
        feesLabel->setObjectName(QStringLiteral("feesLabel"));

        gridLayout_3->addWidget(feesLabel, 15, 0, 1, 1);

        bitsLabel = new QLabel(layoutWidget_2);
        bitsLabel->setObjectName(QStringLiteral("bitsLabel"));

        gridLayout_3->addWidget(bitsLabel, 7, 0, 1, 1);

        bitsBox = new QLabel(layoutWidget_2);
        bitsBox->setObjectName(QStringLiteral("bitsBox"));

        gridLayout_3->addWidget(bitsBox, 7, 1, 1, 1);

        merkleBox = new QLabel(layoutWidget_2);
        merkleBox->setObjectName(QStringLiteral("merkleBox"));
        merkleBox->setMinimumSize(QSize(187, 0));

        gridLayout_3->addWidget(merkleBox, 5, 1, 1, 1);

        outputLabel = new QLabel(layoutWidget_2);
        outputLabel->setObjectName(QStringLiteral("outputLabel"));
        outputLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        gridLayout_3->addWidget(outputLabel, 16, 0, 1, 1);

        outputBox = new QLabel(layoutWidget_2);
        outputBox->setObjectName(QStringLiteral("outputBox"));
        outputBox->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        gridLayout_3->addWidget(outputBox, 16, 1, 1, 1);

        nonceLabel = new QLabel(layoutWidget_2);
        nonceLabel->setObjectName(QStringLiteral("nonceLabel"));

        gridLayout_3->addWidget(nonceLabel, 6, 0, 1, 1);

        nonceBox = new QLabel(layoutWidget_2);
        nonceBox->setObjectName(QStringLiteral("nonceBox"));

        gridLayout_3->addWidget(nonceBox, 6, 1, 1, 1);

        timeLabel = new QLabel(layoutWidget_2);
        timeLabel->setObjectName(QStringLiteral("timeLabel"));

        gridLayout_3->addWidget(timeLabel, 8, 0, 1, 1);

        hashBox = new QLabel(layoutWidget_2);
        hashBox->setObjectName(QStringLiteral("hashBox"));
        hashBox->setEnabled(true);
        sizePolicy.setHeightForWidth(hashBox->sizePolicy().hasHeightForWidth());
        hashBox->setSizePolicy(sizePolicy);
        hashBox->setMinimumSize(QSize(0, 0));

        gridLayout_3->addWidget(hashBox, 4, 1, 1, 1);

        txBox = new QLineEdit(layoutWidget_2);
        txBox->setObjectName(QStringLiteral("txBox"));
        sizePolicy1.setHeightForWidth(txBox->sizePolicy().hasHeightForWidth());
        txBox->setSizePolicy(sizePolicy1);

        gridLayout_3->addWidget(txBox, 12, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer_2, 11, 0, 1, 1);

        blockButton = new QPushButton(layoutWidget_2);
        blockButton->setObjectName(QStringLiteral("blockButton"));
        blockButton->setMinimumSize(QSize(400, 0));
        blockButton->setMaximumSize(QSize(16777215, 16777215));
        blockButton->setFont(font1);
        blockButton->setAutoDefault(true);
        blockButton->setFlat(false);

        gridLayout_3->addWidget(blockButton, 1, 1, 1, 1);


        gridLayout->addWidget(widget, 1, 0, 1, 1);


        retranslateUi(BlockBrowser);

        QMetaObject::connectSlotsByName(BlockBrowser);
    } // setupUi

    void retranslateUi(QWidget *BlockBrowser)
    {
        BlockBrowser->setWindowTitle(QApplication::translate("BlockBrowser", "Form", 0));
        label_5->setText(QApplication::translate("BlockBrowser", "<html><head/><body><p><span style=\" font-size:20px; font-weight:600; color:#000000;\">MarteXcoin Block Explorer </span></p></body></html>", 0));
        inputBox->setText(QString());
        txButton->setText(QApplication::translate("BlockBrowser", "Decode Transaction", 0));
        inputLabel->setText(QApplication::translate("BlockBrowser", "Inputs:", 0));
        pawBox->setText(QApplication::translate("BlockBrowser", "0000 KH/s", 0));
        valueLabel->setText(QApplication::translate("BlockBrowser", "Value out:", 0));
        txLabel->setText(QApplication::translate("BlockBrowser", "Transaction ID:", 0));
        hardBox->setText(QApplication::translate("BlockBrowser", "0.00", 0));
        txID->setText(QApplication::translate("BlockBrowser", "000", 0));
        valueBox->setText(QString());
        hardLabel->setText(QApplication::translate("BlockBrowser", "Block Difficulty:", 0));
        heightLabel_2->setText(QApplication::translate("BlockBrowser", "Block Height:", 0));
        timeBox->setText(QApplication::translate("BlockBrowser", "0", 0));
        merkleLabel->setText(QApplication::translate("BlockBrowser", "Block Merkle:", 0));
        hashLabel->setText(QApplication::translate("BlockBrowser", "Block Hash:", 0));
        feesBox->setText(QString());
        pawLabel->setText(QApplication::translate("BlockBrowser", "Block Hashrate:", 0));
        heightLabel->setText(QApplication::translate("BlockBrowser", "0", 0));
        feesLabel->setText(QApplication::translate("BlockBrowser", "Fees:", 0));
        bitsLabel->setText(QApplication::translate("BlockBrowser", "Block nBits:", 0));
        bitsBox->setText(QApplication::translate("BlockBrowser", "0", 0));
        merkleBox->setText(QApplication::translate("BlockBrowser", "0x0", 0));
        outputLabel->setText(QApplication::translate("BlockBrowser", "Outputs:", 0));
        outputBox->setText(QString());
        nonceLabel->setText(QApplication::translate("BlockBrowser", "Block nNonce:", 0));
        nonceBox->setText(QApplication::translate("BlockBrowser", "0", 0));
        timeLabel->setText(QApplication::translate("BlockBrowser", "Block Timestamp:", 0));
        hashBox->setText(QApplication::translate("BlockBrowser", "0x0", 0));
        txBox->setInputMask(QString());
        txBox->setPlaceholderText(QString());
        blockButton->setText(QApplication::translate("BlockBrowser", "Jump to Block", 0));
    } // retranslateUi

};

namespace Ui {
    class BlockBrowser: public Ui_BlockBrowser {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BLOCKBROWSER_H
