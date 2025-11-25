/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_30;
    QVBoxLayout *verticalLayout_7;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *DevTypecomboBox;
    QHBoxLayout *horizontalLayout_17;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QComboBox *DevIdexcomboBox;
    QSpacerItem *horizontalSpacer_5;
    QHBoxLayout *horizontalLayout_18;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QComboBox *CANIdexcomboBox;
    QSpacerItem *horizontalSpacer_6;
    QHBoxLayout *horizontalLayout_32;
    QHBoxLayout *horizontalLayout_31;
    QLabel *label_15;
    QComboBox *CANBaudratecomboBox;
    QSpacerItem *horizontalSpacer_16;
    QHBoxLayout *horizontalLayout_16;
    QPushButton *OpenCANpushButton;
    QSpacerItem *horizontalSpacer_4;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_29;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_4;
    QComboBox *TxFrameTypecomboBox;
    QSpacerItem *horizontalSpacer_15;
    QHBoxLayout *horizontalLayout_19;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_5;
    QLineEdit *TxFrameIDlineEdit;
    QSpacerItem *horizontalSpacer_7;
    QHBoxLayout *horizontalLayout_20;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_10;
    QComboBox *DLCcomboBox;
    QSpacerItem *horizontalSpacer_8;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_11;
    QLineEdit *CANDatalineEdit;
    QHBoxLayout *horizontalLayout_21;
    QPushButton *CANSendpushButton;
    QSpacerItem *horizontalSpacer_9;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_28;
    QHBoxLayout *horizontalLayout_22;
    QLabel *label_12;
    QComboBox *RxFrameTypecomboBox;
    QSpacerItem *horizontalSpacer_13;
    QHBoxLayout *horizontalLayout_23;
    QHBoxLayout *horizontalLayout_24;
    QLabel *label_13;
    QLineEdit *RxFrameIDlineEdit;
    QSpacerItem *horizontalSpacer_10;
    QHBoxLayout *horizontalLayout_26;
    QHBoxLayout *horizontalLayout_25;
    QLabel *label_14;
    QComboBox *RxDataFormatecomboBox;
    QSpacerItem *horizontalSpacer_11;
    QHBoxLayout *horizontalLayout_27;
    QPushButton *SetRxFramepushButton;
    QSpacerItem *horizontalSpacer_12;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_6;
    QLineEdit *MaxYlineEdit;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_7;
    QLineEdit *MinYlineEdit;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_8;
    QLineEdit *WidthXlineEdit;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *SetPlotpushButton;
    QSpacerItem *horizontalSpacer_14;
    QPushButton *ClearGraphpushButton;
    QVBoxLayout *verticalLayout_8;
    QWidget *widget;
    QVBoxLayout *verticalLayout_4;
    QWidget *chartwidget;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_10;
    QHBoxLayout *horizontalLayout_8;
    QLabel *AxisXlabel;
    QLabel *AxisYlabel;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *GraphStoppushButton;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_9;
    QSpacerItem *horizontalSpacer;
    QPushButton *ClearLogpushButton;
    QPlainTextEdit *LogplainTextEdit;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(832, 696);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout_30 = new QHBoxLayout(centralwidget);
        horizontalLayout_30->setObjectName(QString::fromUtf8("horizontalLayout_30"));
        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        groupBox->setAutoFillBackground(true);
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(-1, -1, 9, -1);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        DevTypecomboBox = new QComboBox(groupBox);
        DevTypecomboBox->addItem(QString());
        DevTypecomboBox->addItem(QString());
        DevTypecomboBox->setObjectName(QString::fromUtf8("DevTypecomboBox"));

        horizontalLayout->addWidget(DevTypecomboBox);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setObjectName(QString::fromUtf8("horizontalLayout_17"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        DevIdexcomboBox = new QComboBox(groupBox);
        DevIdexcomboBox->addItem(QString());
        DevIdexcomboBox->addItem(QString());
        DevIdexcomboBox->setObjectName(QString::fromUtf8("DevIdexcomboBox"));

        horizontalLayout_2->addWidget(DevIdexcomboBox);


        horizontalLayout_17->addLayout(horizontalLayout_2);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_17->addItem(horizontalSpacer_5);


        verticalLayout->addLayout(horizontalLayout_17);

        horizontalLayout_18 = new QHBoxLayout();
        horizontalLayout_18->setObjectName(QString::fromUtf8("horizontalLayout_18"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        CANIdexcomboBox = new QComboBox(groupBox);
        CANIdexcomboBox->addItem(QString());
        CANIdexcomboBox->addItem(QString());
        CANIdexcomboBox->setObjectName(QString::fromUtf8("CANIdexcomboBox"));

        horizontalLayout_3->addWidget(CANIdexcomboBox);


        horizontalLayout_18->addLayout(horizontalLayout_3);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_18->addItem(horizontalSpacer_6);


        verticalLayout->addLayout(horizontalLayout_18);

        horizontalLayout_32 = new QHBoxLayout();
        horizontalLayout_32->setObjectName(QString::fromUtf8("horizontalLayout_32"));
        horizontalLayout_31 = new QHBoxLayout();
        horizontalLayout_31->setObjectName(QString::fromUtf8("horizontalLayout_31"));
        label_15 = new QLabel(groupBox);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        horizontalLayout_31->addWidget(label_15);

        CANBaudratecomboBox = new QComboBox(groupBox);
        CANBaudratecomboBox->addItem(QString());
        CANBaudratecomboBox->addItem(QString());
        CANBaudratecomboBox->addItem(QString());
        CANBaudratecomboBox->addItem(QString());
        CANBaudratecomboBox->addItem(QString());
        CANBaudratecomboBox->addItem(QString());
        CANBaudratecomboBox->setObjectName(QString::fromUtf8("CANBaudratecomboBox"));
        CANBaudratecomboBox->setEditable(false);

        horizontalLayout_31->addWidget(CANBaudratecomboBox);


        horizontalLayout_32->addLayout(horizontalLayout_31);

        horizontalSpacer_16 = new QSpacerItem(40, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        horizontalLayout_32->addItem(horizontalSpacer_16);


        verticalLayout->addLayout(horizontalLayout_32);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setObjectName(QString::fromUtf8("horizontalLayout_16"));
        OpenCANpushButton = new QPushButton(groupBox);
        OpenCANpushButton->setObjectName(QString::fromUtf8("OpenCANpushButton"));

        horizontalLayout_16->addWidget(OpenCANpushButton);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_16->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_16);


        verticalLayout_7->addWidget(groupBox);

        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy1);
        groupBox_2->setAutoFillBackground(true);
        verticalLayout_5 = new QVBoxLayout(groupBox_2);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        horizontalLayout_29 = new QHBoxLayout();
        horizontalLayout_29->setObjectName(QString::fromUtf8("horizontalLayout_29"));
        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_11->addWidget(label_4);

        TxFrameTypecomboBox = new QComboBox(groupBox_2);
        TxFrameTypecomboBox->addItem(QString());
        TxFrameTypecomboBox->addItem(QString());
        TxFrameTypecomboBox->setObjectName(QString::fromUtf8("TxFrameTypecomboBox"));

        horizontalLayout_11->addWidget(TxFrameTypecomboBox);


        horizontalLayout_29->addLayout(horizontalLayout_11);

        horizontalSpacer_15 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_29->addItem(horizontalSpacer_15);


        verticalLayout_5->addLayout(horizontalLayout_29);

        horizontalLayout_19 = new QHBoxLayout();
        horizontalLayout_19->setObjectName(QString::fromUtf8("horizontalLayout_19"));
        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_12->addWidget(label_5);

        TxFrameIDlineEdit = new QLineEdit(groupBox_2);
        TxFrameIDlineEdit->setObjectName(QString::fromUtf8("TxFrameIDlineEdit"));
        TxFrameIDlineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_12->addWidget(TxFrameIDlineEdit);


        horizontalLayout_19->addLayout(horizontalLayout_12);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_19->addItem(horizontalSpacer_7);

        horizontalLayout_19->setStretch(0, 2);
        horizontalLayout_19->setStretch(1, 1);

        verticalLayout_5->addLayout(horizontalLayout_19);

        horizontalLayout_20 = new QHBoxLayout();
        horizontalLayout_20->setObjectName(QString::fromUtf8("horizontalLayout_20"));
        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        label_10 = new QLabel(groupBox_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        horizontalLayout_13->addWidget(label_10);

        DLCcomboBox = new QComboBox(groupBox_2);
        DLCcomboBox->addItem(QString());
        DLCcomboBox->addItem(QString());
        DLCcomboBox->addItem(QString());
        DLCcomboBox->addItem(QString());
        DLCcomboBox->addItem(QString());
        DLCcomboBox->addItem(QString());
        DLCcomboBox->addItem(QString());
        DLCcomboBox->addItem(QString());
        DLCcomboBox->setObjectName(QString::fromUtf8("DLCcomboBox"));

        horizontalLayout_13->addWidget(DLCcomboBox);


        horizontalLayout_20->addLayout(horizontalLayout_13);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_20->addItem(horizontalSpacer_8);


        verticalLayout_5->addLayout(horizontalLayout_20);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        label_11 = new QLabel(groupBox_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        horizontalLayout_14->addWidget(label_11);

        CANDatalineEdit = new QLineEdit(groupBox_2);
        CANDatalineEdit->setObjectName(QString::fromUtf8("CANDatalineEdit"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(CANDatalineEdit->sizePolicy().hasHeightForWidth());
        CANDatalineEdit->setSizePolicy(sizePolicy2);
        CANDatalineEdit->setMinimumSize(QSize(150, 0));

        horizontalLayout_14->addWidget(CANDatalineEdit);


        verticalLayout_5->addLayout(horizontalLayout_14);

        horizontalLayout_21 = new QHBoxLayout();
        horizontalLayout_21->setObjectName(QString::fromUtf8("horizontalLayout_21"));
        CANSendpushButton = new QPushButton(groupBox_2);
        CANSendpushButton->setObjectName(QString::fromUtf8("CANSendpushButton"));

        horizontalLayout_21->addWidget(CANSendpushButton);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_21->addItem(horizontalSpacer_9);


        verticalLayout_5->addLayout(horizontalLayout_21);


        verticalLayout_7->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        sizePolicy1.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy1);
        groupBox_3->setAutoFillBackground(true);
        verticalLayout_6 = new QVBoxLayout(groupBox_3);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        horizontalLayout_28 = new QHBoxLayout();
        horizontalLayout_28->setObjectName(QString::fromUtf8("horizontalLayout_28"));
        horizontalLayout_22 = new QHBoxLayout();
        horizontalLayout_22->setObjectName(QString::fromUtf8("horizontalLayout_22"));
        label_12 = new QLabel(groupBox_3);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        horizontalLayout_22->addWidget(label_12);

        RxFrameTypecomboBox = new QComboBox(groupBox_3);
        RxFrameTypecomboBox->addItem(QString());
        RxFrameTypecomboBox->addItem(QString());
        RxFrameTypecomboBox->setObjectName(QString::fromUtf8("RxFrameTypecomboBox"));

        horizontalLayout_22->addWidget(RxFrameTypecomboBox);


        horizontalLayout_28->addLayout(horizontalLayout_22);

        horizontalSpacer_13 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_28->addItem(horizontalSpacer_13);


        verticalLayout_6->addLayout(horizontalLayout_28);

        horizontalLayout_23 = new QHBoxLayout();
        horizontalLayout_23->setObjectName(QString::fromUtf8("horizontalLayout_23"));
        horizontalLayout_24 = new QHBoxLayout();
        horizontalLayout_24->setObjectName(QString::fromUtf8("horizontalLayout_24"));
        label_13 = new QLabel(groupBox_3);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        horizontalLayout_24->addWidget(label_13);

        RxFrameIDlineEdit = new QLineEdit(groupBox_3);
        RxFrameIDlineEdit->setObjectName(QString::fromUtf8("RxFrameIDlineEdit"));
        RxFrameIDlineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_24->addWidget(RxFrameIDlineEdit);


        horizontalLayout_23->addLayout(horizontalLayout_24);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        horizontalLayout_23->addItem(horizontalSpacer_10);

        horizontalLayout_23->setStretch(0, 2);
        horizontalLayout_23->setStretch(1, 1);

        verticalLayout_6->addLayout(horizontalLayout_23);

        horizontalLayout_26 = new QHBoxLayout();
        horizontalLayout_26->setObjectName(QString::fromUtf8("horizontalLayout_26"));
        horizontalLayout_25 = new QHBoxLayout();
        horizontalLayout_25->setObjectName(QString::fromUtf8("horizontalLayout_25"));
        label_14 = new QLabel(groupBox_3);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        horizontalLayout_25->addWidget(label_14);

        RxDataFormatecomboBox = new QComboBox(groupBox_3);
        RxDataFormatecomboBox->addItem(QString());
        RxDataFormatecomboBox->addItem(QString());
        RxDataFormatecomboBox->setObjectName(QString::fromUtf8("RxDataFormatecomboBox"));
        sizePolicy.setHeightForWidth(RxDataFormatecomboBox->sizePolicy().hasHeightForWidth());
        RxDataFormatecomboBox->setSizePolicy(sizePolicy);

        horizontalLayout_25->addWidget(RxDataFormatecomboBox);


        horizontalLayout_26->addLayout(horizontalLayout_25);

        horizontalSpacer_11 = new QSpacerItem(40, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        horizontalLayout_26->addItem(horizontalSpacer_11);

        horizontalLayout_26->setStretch(0, 3);
        horizontalLayout_26->setStretch(1, 1);

        verticalLayout_6->addLayout(horizontalLayout_26);

        horizontalLayout_27 = new QHBoxLayout();
        horizontalLayout_27->setObjectName(QString::fromUtf8("horizontalLayout_27"));
        SetRxFramepushButton = new QPushButton(groupBox_3);
        SetRxFramepushButton->setObjectName(QString::fromUtf8("SetRxFramepushButton"));

        horizontalLayout_27->addWidget(SetRxFramepushButton);

        horizontalSpacer_12 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_27->addItem(horizontalSpacer_12);


        verticalLayout_6->addLayout(horizontalLayout_27);


        verticalLayout_7->addWidget(groupBox_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout_4->addWidget(label_6);

        MaxYlineEdit = new QLineEdit(centralwidget);
        MaxYlineEdit->setObjectName(QString::fromUtf8("MaxYlineEdit"));
        sizePolicy.setHeightForWidth(MaxYlineEdit->sizePolicy().hasHeightForWidth());
        MaxYlineEdit->setSizePolicy(sizePolicy);
        MaxYlineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(MaxYlineEdit);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_7 = new QLabel(centralwidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_5->addWidget(label_7);

        MinYlineEdit = new QLineEdit(centralwidget);
        MinYlineEdit->setObjectName(QString::fromUtf8("MinYlineEdit"));
        MinYlineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_5->addWidget(MinYlineEdit);


        verticalLayout_2->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_8 = new QLabel(centralwidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_6->addWidget(label_8);

        WidthXlineEdit = new QLineEdit(centralwidget);
        WidthXlineEdit->setObjectName(QString::fromUtf8("WidthXlineEdit"));
        WidthXlineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(WidthXlineEdit);


        verticalLayout_2->addLayout(horizontalLayout_6);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        SetPlotpushButton = new QPushButton(centralwidget);
        SetPlotpushButton->setObjectName(QString::fromUtf8("SetPlotpushButton"));

        horizontalLayout_7->addWidget(SetPlotpushButton);

        horizontalSpacer_14 = new QSpacerItem(40, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_14);

        ClearGraphpushButton = new QPushButton(centralwidget);
        ClearGraphpushButton->setObjectName(QString::fromUtf8("ClearGraphpushButton"));

        horizontalLayout_7->addWidget(ClearGraphpushButton);


        verticalLayout_2->addLayout(horizontalLayout_7);


        verticalLayout_7->addLayout(verticalLayout_2);

        verticalLayout_7->setStretch(0, 3);
        verticalLayout_7->setStretch(1, 1);

        horizontalLayout_30->addLayout(verticalLayout_7);

        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        widget = new QWidget(centralwidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        verticalLayout_4 = new QVBoxLayout(widget);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        chartwidget = new QWidget(widget);
        chartwidget->setObjectName(QString::fromUtf8("chartwidget"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(chartwidget->sizePolicy().hasHeightForWidth());
        chartwidget->setSizePolicy(sizePolicy3);
        chartwidget->setMouseTracking(false);
        gridLayout_2 = new QGridLayout(chartwidget);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        verticalLayout_4->addWidget(chartwidget);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        AxisXlabel = new QLabel(widget);
        AxisXlabel->setObjectName(QString::fromUtf8("AxisXlabel"));

        horizontalLayout_8->addWidget(AxisXlabel);

        AxisYlabel = new QLabel(widget);
        AxisYlabel->setObjectName(QString::fromUtf8("AxisYlabel"));

        horizontalLayout_8->addWidget(AxisYlabel);


        horizontalLayout_10->addLayout(horizontalLayout_8);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_2);

        GraphStoppushButton = new QPushButton(widget);
        GraphStoppushButton->setObjectName(QString::fromUtf8("GraphStoppushButton"));

        horizontalLayout_10->addWidget(GraphStoppushButton);

        horizontalLayout_10->setStretch(0, 1);
        horizontalLayout_10->setStretch(1, 3);

        verticalLayout_4->addLayout(horizontalLayout_10);

        verticalLayout_4->setStretch(0, 30);
        verticalLayout_4->setStretch(1, 1);

        verticalLayout_8->addWidget(widget);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        label_9 = new QLabel(centralwidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        horizontalLayout_9->addWidget(label_9);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer);

        ClearLogpushButton = new QPushButton(centralwidget);
        ClearLogpushButton->setObjectName(QString::fromUtf8("ClearLogpushButton"));

        horizontalLayout_9->addWidget(ClearLogpushButton);


        verticalLayout_3->addLayout(horizontalLayout_9);

        LogplainTextEdit = new QPlainTextEdit(centralwidget);
        LogplainTextEdit->setObjectName(QString::fromUtf8("LogplainTextEdit"));
        QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(LogplainTextEdit->sizePolicy().hasHeightForWidth());
        LogplainTextEdit->setSizePolicy(sizePolicy4);
        LogplainTextEdit->setMinimumSize(QSize(600, 0));

        verticalLayout_3->addWidget(LogplainTextEdit);


        verticalLayout_8->addLayout(verticalLayout_3);

        verticalLayout_8->setStretch(0, 3);
        verticalLayout_8->setStretch(1, 1);

        horizontalLayout_30->addLayout(verticalLayout_8);

        horizontalLayout_30->setStretch(0, 1);
        horizontalLayout_30->setStretch(1, 5);
        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        CANBaudratecomboBox->setCurrentIndex(3);
        TxFrameTypecomboBox->setCurrentIndex(1);
        DLCcomboBox->setCurrentIndex(7);
        RxFrameTypecomboBox->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        groupBox->setTitle(QApplication::translate("MainWindow", "CAN\351\205\215\347\275\256", nullptr));
        label->setText(QApplication::translate("MainWindow", "\350\256\276\345\244\207\347\261\273\345\236\213", nullptr));
        DevTypecomboBox->setItemText(0, QApplication::translate("MainWindow", "CANalyst-\342\205\241", nullptr));
        DevTypecomboBox->setItemText(1, QApplication::translate("MainWindow", "HCANBUS Mini-\342\205\241", nullptr));

        label_2->setText(QApplication::translate("MainWindow", "\350\256\276\345\244\207\347\264\242\345\274\225", nullptr));
        DevIdexcomboBox->setItemText(0, QApplication::translate("MainWindow", "0", nullptr));
        DevIdexcomboBox->setItemText(1, QApplication::translate("MainWindow", "1", nullptr));

        label_3->setText(QApplication::translate("MainWindow", "\351\200\232\351\201\223\347\264\242\345\274\225", nullptr));
        CANIdexcomboBox->setItemText(0, QApplication::translate("MainWindow", "0", nullptr));
        CANIdexcomboBox->setItemText(1, QApplication::translate("MainWindow", "1", nullptr));

        label_15->setText(QApplication::translate("MainWindow", "CAN\346\263\242\347\211\271\347\216\207", nullptr));
        CANBaudratecomboBox->setItemText(0, QApplication::translate("MainWindow", "100", nullptr));
        CANBaudratecomboBox->setItemText(1, QApplication::translate("MainWindow", "125", nullptr));
        CANBaudratecomboBox->setItemText(2, QApplication::translate("MainWindow", "250", nullptr));
        CANBaudratecomboBox->setItemText(3, QApplication::translate("MainWindow", "500", nullptr));
        CANBaudratecomboBox->setItemText(4, QApplication::translate("MainWindow", "800", nullptr));
        CANBaudratecomboBox->setItemText(5, QApplication::translate("MainWindow", "1000", nullptr));

        CANBaudratecomboBox->setCurrentText(QApplication::translate("MainWindow", "500", nullptr));
        OpenCANpushButton->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200", nullptr));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "CAN\345\217\221\351\200\201", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "\345\270\247\347\261\273\345\236\213", nullptr));
        TxFrameTypecomboBox->setItemText(0, QApplication::translate("MainWindow", "\346\240\207\345\207\206\345\270\247", nullptr));
        TxFrameTypecomboBox->setItemText(1, QApplication::translate("MainWindow", "\346\211\251\345\261\225\345\270\247", nullptr));

        label_5->setText(QApplication::translate("MainWindow", "\345\270\247ID(0x)", nullptr));
        TxFrameIDlineEdit->setText(QApplication::translate("MainWindow", "00000000", nullptr));
        label_10->setText(QApplication::translate("MainWindow", "DLC", nullptr));
        DLCcomboBox->setItemText(0, QApplication::translate("MainWindow", "1", nullptr));
        DLCcomboBox->setItemText(1, QApplication::translate("MainWindow", "2", nullptr));
        DLCcomboBox->setItemText(2, QApplication::translate("MainWindow", "3", nullptr));
        DLCcomboBox->setItemText(3, QApplication::translate("MainWindow", "4", nullptr));
        DLCcomboBox->setItemText(4, QApplication::translate("MainWindow", "5", nullptr));
        DLCcomboBox->setItemText(5, QApplication::translate("MainWindow", "6", nullptr));
        DLCcomboBox->setItemText(6, QApplication::translate("MainWindow", "7", nullptr));
        DLCcomboBox->setItemText(7, QApplication::translate("MainWindow", "8", nullptr));

        DLCcomboBox->setCurrentText(QApplication::translate("MainWindow", "8", nullptr));
        label_11->setText(QApplication::translate("MainWindow", "\346\225\260\346\215\256", nullptr));
        CANDatalineEdit->setText(QApplication::translate("MainWindow", "00 00 00 00 00 00 00 00", nullptr));
        CANSendpushButton->setText(QApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        groupBox_3->setTitle(QApplication::translate("MainWindow", "CAN\346\216\245\346\224\266\351\205\215\347\275\256", nullptr));
        label_12->setText(QApplication::translate("MainWindow", "\345\270\247\347\261\273\345\236\213", nullptr));
        RxFrameTypecomboBox->setItemText(0, QApplication::translate("MainWindow", "\346\240\207\345\207\206\345\270\247", nullptr));
        RxFrameTypecomboBox->setItemText(1, QApplication::translate("MainWindow", "\346\211\251\345\261\225\345\270\247", nullptr));

        label_13->setText(QApplication::translate("MainWindow", "\345\270\247ID(0x)", nullptr));
        RxFrameIDlineEdit->setText(QApplication::translate("MainWindow", "00000000", nullptr));
        label_14->setText(QApplication::translate("MainWindow", "\346\225\260\346\215\256\346\240\274\345\274\217", nullptr));
        RxDataFormatecomboBox->setItemText(0, QApplication::translate("MainWindow", "int32", nullptr));
        RxDataFormatecomboBox->setItemText(1, QApplication::translate("MainWindow", "ascii", nullptr));

        RxDataFormatecomboBox->setCurrentText(QApplication::translate("MainWindow", "int32", nullptr));
        SetRxFramepushButton->setText(QApplication::translate("MainWindow", "\350\256\276\347\275\256", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "Y\350\275\264\346\234\200\345\244\247\345\200\274", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "Y\350\275\264\346\234\200\345\260\217\345\200\274", nullptr));
        label_8->setText(QApplication::translate("MainWindow", "X\350\275\264\345\256\275\345\272\246", nullptr));
        SetPlotpushButton->setText(QApplication::translate("MainWindow", "\350\256\276\347\275\256", nullptr));
        ClearGraphpushButton->setText(QApplication::translate("MainWindow", "\346\270\205\351\231\244\346\233\262\347\272\277", nullptr));
        AxisXlabel->setText(QString());
        AxisYlabel->setText(QString());
        GraphStoppushButton->setText(QApplication::translate("MainWindow", "\346\232\202\345\201\234", nullptr));
        label_9->setText(QApplication::translate("MainWindow", "\346\227\245\345\277\227", nullptr));
        ClearLogpushButton->setText(QApplication::translate("MainWindow", "\346\270\205\347\251\272\346\227\245\345\277\227", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
