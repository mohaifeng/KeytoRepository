/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_10;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QLabel *label;
    QComboBox *SerialcomboBox;
    QLabel *label_2;
    QComboBox *BaudratecomboBox;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_4;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout;
    QLabel *label_3;
    QLineEdit *AddrlineEdit;
    QPushButton *CheckAddr;
    QPushButton *ChangeAddr;
    QSpacerItem *horizontalSpacer_5;
    QGroupBox *groupBox_4;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_10;
    QLineEdit *VersionlineEdit;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_7;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QComboBox *ReadWritecomboBox;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_6;
    QLineEdit *RegAddrlineEdit;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_11;
    QLineEdit *WriteNumlineEdit;
    QPushButton *CmdSendpushButton;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_5;
    QLineEdit *DatalineEdit;
    QCheckBox *HexcheckBox;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_8;
    QGroupBox *groupBox_5;
    QHBoxLayout *horizontalLayout_9;
    QGridLayout *gridLayout_3;
    QSpacerItem *horizontalSpacer_10;
    QPushButton *ClearLinespushButton;
    QLabel *label_12;
    QLineEdit *GapTimelineEdit;
    QTableWidget *ReadMultableWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label_7;
    QLineEdit *RegStartlineEdit;
    QLabel *label_8;
    QLineEdit *ReadNumlineEdit;
    QPushButton *AddRowpushButton;
    QCheckBox *LoopReadcheckBox;
    QPushButton *StartReadpushButton;
    QSpacerItem *verticalSpacer;
    QWidget *Fncwidget;
    QGridLayout *gridLayout_5;
    QGroupBox *groupBox_6;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *groupBox_7;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_13;
    QLineEdit *TargetChlineEdit;
    QPushButton *FastSwitchpushButton;
    QPushButton *CounterclockwisepushButton;
    QPushButton *ClockwisepushButton;
    QPushButton *InitpushButton;
    QPushButton *StoppushButton;
    QPushButton *ClearErrorpushButton;
    QGroupBox *groupBox_8;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_14;
    QLineEdit *MaxSpeedlineEdit;
    QLabel *label_15;
    QLineEdit *MinSpeedlineEdit;
    QLabel *label_16;
    QLineEdit *AcclineEdit;
    QLabel *label_17;
    QLineEdit *DeclineEdit;
    QVBoxLayout *verticalLayout_3;
    QPushButton *ReadPara_pushButton;
    QPushButton *SetParapushButton;
    QPushButton *SavepushButton;
    QPushButton *ResetpushButton;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_18;
    QLineEdit *CurrentlineEdit;
    QLabel *label_19;
    QLineEdit *MaxChlineEdit;
    QLabel *label_20;
    QComboBox *CANBaudRatecomboBox;
    QLabel *label_21;
    QComboBox *SerBaudRatecomboBox;
    QGridLayout *gridLayout_7;
    QSpacerItem *horizontalSpacer_8;
    QPlainTextEdit *LogplainTextEdit;
    QPushButton *ClearLogpushButton;
    QLabel *label_9;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->setEnabled(true);
        MainWindow->resize(969, 656);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName("groupBox_2");
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setObjectName("gridLayout");
        label = new QLabel(groupBox_2);
        label->setObjectName("label");
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        SerialcomboBox = new QComboBox(groupBox_2);
        SerialcomboBox->setObjectName("SerialcomboBox");
        SerialcomboBox->setEnabled(true);

        gridLayout->addWidget(SerialcomboBox, 0, 1, 1, 1);

        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName("label_2");
        label_2->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        BaudratecomboBox = new QComboBox(groupBox_2);
        BaudratecomboBox->addItem(QString());
        BaudratecomboBox->addItem(QString());
        BaudratecomboBox->addItem(QString());
        BaudratecomboBox->addItem(QString());
        BaudratecomboBox->addItem(QString());
        BaudratecomboBox->setObjectName("BaudratecomboBox");
        BaudratecomboBox->setEnabled(true);

        gridLayout->addWidget(BaudratecomboBox, 1, 1, 1, 1);

        pushButton = new QPushButton(groupBox_2);
        pushButton->setObjectName("pushButton");
        sizePolicy.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy);
        pushButton->setCheckable(true);
        pushButton->setChecked(true);

        gridLayout->addWidget(pushButton, 0, 2, 2, 1);


        horizontalLayout_10->addWidget(groupBox_2);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_4);

        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName("groupBox_3");
        horizontalLayout = new QHBoxLayout(groupBox_3);
        horizontalLayout->setObjectName("horizontalLayout");
        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName("label_3");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(label_3);

        AddrlineEdit = new QLineEdit(groupBox_3);
        AddrlineEdit->setObjectName("AddrlineEdit");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(AddrlineEdit->sizePolicy().hasHeightForWidth());
        AddrlineEdit->setSizePolicy(sizePolicy2);
        AddrlineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(AddrlineEdit);

        CheckAddr = new QPushButton(groupBox_3);
        CheckAddr->setObjectName("CheckAddr");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(CheckAddr->sizePolicy().hasHeightForWidth());
        CheckAddr->setSizePolicy(sizePolicy3);

        horizontalLayout->addWidget(CheckAddr);

        ChangeAddr = new QPushButton(groupBox_3);
        ChangeAddr->setObjectName("ChangeAddr");
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(ChangeAddr->sizePolicy().hasHeightForWidth());
        ChangeAddr->setSizePolicy(sizePolicy4);

        horizontalLayout->addWidget(ChangeAddr);

        horizontalLayout->setStretch(0, 2);
        horizontalLayout->setStretch(1, 1);
        horizontalLayout->setStretch(2, 2);
        horizontalLayout->setStretch(3, 2);

        horizontalLayout_10->addWidget(groupBox_3);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_5);

        groupBox_4 = new QGroupBox(centralwidget);
        groupBox_4->setObjectName("groupBox_4");
        sizePolicy1.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy1);
        horizontalLayout_2 = new QHBoxLayout(groupBox_4);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label_10 = new QLabel(groupBox_4);
        label_10->setObjectName("label_10");

        horizontalLayout_2->addWidget(label_10);

        VersionlineEdit = new QLineEdit(groupBox_4);
        VersionlineEdit->setObjectName("VersionlineEdit");
        sizePolicy3.setHeightForWidth(VersionlineEdit->sizePolicy().hasHeightForWidth());
        VersionlineEdit->setSizePolicy(sizePolicy3);
        VersionlineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(VersionlineEdit);


        horizontalLayout_10->addWidget(groupBox_4);

        horizontalLayout_10->setStretch(0, 1);
        horizontalLayout_10->setStretch(1, 4);
        horizontalLayout_10->setStretch(2, 1);
        horizontalLayout_10->setStretch(3, 6);
        horizontalLayout_10->setStretch(4, 8);

        verticalLayout_2->addLayout(horizontalLayout_10);

        horizontalSpacer_2 = new QSpacerItem(1003, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        verticalLayout_2->addItem(horizontalSpacer_2);

        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        horizontalLayout_7 = new QHBoxLayout(groupBox);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        horizontalLayout_7->setContentsMargins(9, 9, 9, 9);
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName("gridLayout_2");
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        label_4 = new QLabel(groupBox);
        label_4->setObjectName("label_4");
        label_4->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(label_4);

        ReadWritecomboBox = new QComboBox(groupBox);
        ReadWritecomboBox->addItem(QString());
        ReadWritecomboBox->addItem(QString());
        ReadWritecomboBox->addItem(QString());
        ReadWritecomboBox->setObjectName("ReadWritecomboBox");
        ReadWritecomboBox->setIconSize(QSize(16, 16));

        horizontalLayout_3->addWidget(ReadWritecomboBox);


        gridLayout_2->addLayout(horizontalLayout_3, 0, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(-1, -1, -1, 0);
        label_6 = new QLabel(groupBox);
        label_6->setObjectName("label_6");

        horizontalLayout_4->addWidget(label_6);

        RegAddrlineEdit = new QLineEdit(groupBox);
        RegAddrlineEdit->setObjectName("RegAddrlineEdit");
        sizePolicy2.setHeightForWidth(RegAddrlineEdit->sizePolicy().hasHeightForWidth());
        RegAddrlineEdit->setSizePolicy(sizePolicy2);
        RegAddrlineEdit->setMinimumSize(QSize(0, 0));
        RegAddrlineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(RegAddrlineEdit);


        gridLayout_2->addLayout(horizontalLayout_4, 0, 1, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        label_11 = new QLabel(groupBox);
        label_11->setObjectName("label_11");

        horizontalLayout_5->addWidget(label_11);

        WriteNumlineEdit = new QLineEdit(groupBox);
        WriteNumlineEdit->setObjectName("WriteNumlineEdit");
        sizePolicy2.setHeightForWidth(WriteNumlineEdit->sizePolicy().hasHeightForWidth());
        WriteNumlineEdit->setSizePolicy(sizePolicy2);
        WriteNumlineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_5->addWidget(WriteNumlineEdit);


        gridLayout_2->addLayout(horizontalLayout_5, 0, 2, 1, 1);

        CmdSendpushButton = new QPushButton(groupBox);
        CmdSendpushButton->setObjectName("CmdSendpushButton");
        QSizePolicy sizePolicy5(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(CmdSendpushButton->sizePolicy().hasHeightForWidth());
        CmdSendpushButton->setSizePolicy(sizePolicy5);

        gridLayout_2->addWidget(CmdSendpushButton, 0, 3, 2, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalLayout_6->setContentsMargins(-1, -1, -1, 0);
        label_5 = new QLabel(groupBox);
        label_5->setObjectName("label_5");
        label_5->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(label_5);

        DatalineEdit = new QLineEdit(groupBox);
        DatalineEdit->setObjectName("DatalineEdit");
        DatalineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(DatalineEdit);

        HexcheckBox = new QCheckBox(groupBox);
        HexcheckBox->setObjectName("HexcheckBox");

        horizontalLayout_6->addWidget(HexcheckBox);


        gridLayout_2->addLayout(horizontalLayout_6, 1, 0, 1, 3);


        horizontalLayout_7->addLayout(gridLayout_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer);

        horizontalLayout_7->setStretch(0, 3);
        horizontalLayout_7->setStretch(1, 2);

        verticalLayout_2->addWidget(groupBox);

        horizontalSpacer_3 = new QSpacerItem(968, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        verticalLayout_2->addItem(horizontalSpacer_3);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        groupBox_5 = new QGroupBox(centralwidget);
        groupBox_5->setObjectName("groupBox_5");
        sizePolicy.setHeightForWidth(groupBox_5->sizePolicy().hasHeightForWidth());
        groupBox_5->setSizePolicy(sizePolicy);
        groupBox_5->setMaximumSize(QSize(16777215, 320));
        horizontalLayout_9 = new QHBoxLayout(groupBox_5);
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName("gridLayout_3");
        horizontalSpacer_10 = new QSpacerItem(38, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_10, 1, 2, 1, 1);

        ClearLinespushButton = new QPushButton(groupBox_5);
        ClearLinespushButton->setObjectName("ClearLinespushButton");
        sizePolicy2.setHeightForWidth(ClearLinespushButton->sizePolicy().hasHeightForWidth());
        ClearLinespushButton->setSizePolicy(sizePolicy2);

        gridLayout_3->addWidget(ClearLinespushButton, 1, 3, 1, 1);

        label_12 = new QLabel(groupBox_5);
        label_12->setObjectName("label_12");

        gridLayout_3->addWidget(label_12, 1, 0, 1, 1);

        GapTimelineEdit = new QLineEdit(groupBox_5);
        GapTimelineEdit->setObjectName("GapTimelineEdit");
        QSizePolicy sizePolicy6(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Fixed);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(GapTimelineEdit->sizePolicy().hasHeightForWidth());
        GapTimelineEdit->setSizePolicy(sizePolicy6);
        GapTimelineEdit->setMaximumSize(QSize(40, 16777215));
        GapTimelineEdit->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(GapTimelineEdit, 1, 1, 1, 1);

        ReadMultableWidget = new QTableWidget(groupBox_5);
        if (ReadMultableWidget->columnCount() < 2)
            ReadMultableWidget->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        __qtablewidgetitem->setTextAlignment(Qt::AlignCenter);
        ReadMultableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        __qtablewidgetitem1->setTextAlignment(Qt::AlignCenter);
        ReadMultableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        ReadMultableWidget->setObjectName("ReadMultableWidget");
        sizePolicy.setHeightForWidth(ReadMultableWidget->sizePolicy().hasHeightForWidth());
        ReadMultableWidget->setSizePolicy(sizePolicy);
        ReadMultableWidget->setMinimumSize(QSize(150, 0));
        ReadMultableWidget->setTextElideMode(Qt::ElideMiddle);

        gridLayout_3->addWidget(ReadMultableWidget, 0, 0, 1, 4);


        horizontalLayout_9->addLayout(gridLayout_3);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        label_7 = new QLabel(groupBox_5);
        label_7->setObjectName("label_7");
        sizePolicy2.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy2);

        verticalLayout->addWidget(label_7);

        RegStartlineEdit = new QLineEdit(groupBox_5);
        RegStartlineEdit->setObjectName("RegStartlineEdit");
        sizePolicy6.setHeightForWidth(RegStartlineEdit->sizePolicy().hasHeightForWidth());
        RegStartlineEdit->setSizePolicy(sizePolicy6);
        RegStartlineEdit->setMaximumSize(QSize(100, 16777215));
        RegStartlineEdit->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(RegStartlineEdit);

        label_8 = new QLabel(groupBox_5);
        label_8->setObjectName("label_8");
        sizePolicy2.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy2);
        label_8->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_8);

        ReadNumlineEdit = new QLineEdit(groupBox_5);
        ReadNumlineEdit->setObjectName("ReadNumlineEdit");
        sizePolicy6.setHeightForWidth(ReadNumlineEdit->sizePolicy().hasHeightForWidth());
        ReadNumlineEdit->setSizePolicy(sizePolicy6);
        ReadNumlineEdit->setMaximumSize(QSize(100, 16777215));
        ReadNumlineEdit->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(ReadNumlineEdit);

        AddRowpushButton = new QPushButton(groupBox_5);
        AddRowpushButton->setObjectName("AddRowpushButton");

        verticalLayout->addWidget(AddRowpushButton);

        LoopReadcheckBox = new QCheckBox(groupBox_5);
        LoopReadcheckBox->setObjectName("LoopReadcheckBox");
        QSizePolicy sizePolicy7(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(LoopReadcheckBox->sizePolicy().hasHeightForWidth());
        LoopReadcheckBox->setSizePolicy(sizePolicy7);
        LoopReadcheckBox->setBaseSize(QSize(0, 0));

        verticalLayout->addWidget(LoopReadcheckBox);

        StartReadpushButton = new QPushButton(groupBox_5);
        StartReadpushButton->setObjectName("StartReadpushButton");

        verticalLayout->addWidget(StartReadpushButton);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout_9->addLayout(verticalLayout);


        horizontalLayout_8->addWidget(groupBox_5);

        Fncwidget = new QWidget(centralwidget);
        Fncwidget->setObjectName("Fncwidget");
        Fncwidget->setEnabled(true);
        gridLayout_5 = new QGridLayout(Fncwidget);
        gridLayout_5->setObjectName("gridLayout_5");
        groupBox_6 = new QGroupBox(Fncwidget);
        groupBox_6->setObjectName("groupBox_6");
        groupBox_6->setEnabled(true);
        sizePolicy.setHeightForWidth(groupBox_6->sizePolicy().hasHeightForWidth());
        groupBox_6->setSizePolicy(sizePolicy);
        groupBox_6->setMaximumSize(QSize(16777215, 320));
        verticalLayout_4 = new QVBoxLayout(groupBox_6);
        verticalLayout_4->setObjectName("verticalLayout_4");
        groupBox_7 = new QGroupBox(groupBox_6);
        groupBox_7->setObjectName("groupBox_7");
        sizePolicy.setHeightForWidth(groupBox_7->sizePolicy().hasHeightForWidth());
        groupBox_7->setSizePolicy(sizePolicy);
        horizontalLayout_12 = new QHBoxLayout(groupBox_7);
        horizontalLayout_12->setObjectName("horizontalLayout_12");
        label_13 = new QLabel(groupBox_7);
        label_13->setObjectName("label_13");
        sizePolicy3.setHeightForWidth(label_13->sizePolicy().hasHeightForWidth());
        label_13->setSizePolicy(sizePolicy3);

        horizontalLayout_12->addWidget(label_13);

        TargetChlineEdit = new QLineEdit(groupBox_7);
        TargetChlineEdit->setObjectName("TargetChlineEdit");
        sizePolicy2.setHeightForWidth(TargetChlineEdit->sizePolicy().hasHeightForWidth());
        TargetChlineEdit->setSizePolicy(sizePolicy2);
        TargetChlineEdit->setMaximumSize(QSize(40, 16777215));
        TargetChlineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_12->addWidget(TargetChlineEdit);

        FastSwitchpushButton = new QPushButton(groupBox_7);
        FastSwitchpushButton->setObjectName("FastSwitchpushButton");
        sizePolicy2.setHeightForWidth(FastSwitchpushButton->sizePolicy().hasHeightForWidth());
        FastSwitchpushButton->setSizePolicy(sizePolicy2);

        horizontalLayout_12->addWidget(FastSwitchpushButton);

        CounterclockwisepushButton = new QPushButton(groupBox_7);
        CounterclockwisepushButton->setObjectName("CounterclockwisepushButton");
        sizePolicy2.setHeightForWidth(CounterclockwisepushButton->sizePolicy().hasHeightForWidth());
        CounterclockwisepushButton->setSizePolicy(sizePolicy2);

        horizontalLayout_12->addWidget(CounterclockwisepushButton);

        ClockwisepushButton = new QPushButton(groupBox_7);
        ClockwisepushButton->setObjectName("ClockwisepushButton");
        sizePolicy2.setHeightForWidth(ClockwisepushButton->sizePolicy().hasHeightForWidth());
        ClockwisepushButton->setSizePolicy(sizePolicy2);

        horizontalLayout_12->addWidget(ClockwisepushButton);

        InitpushButton = new QPushButton(groupBox_7);
        InitpushButton->setObjectName("InitpushButton");
        sizePolicy2.setHeightForWidth(InitpushButton->sizePolicy().hasHeightForWidth());
        InitpushButton->setSizePolicy(sizePolicy2);

        horizontalLayout_12->addWidget(InitpushButton);

        StoppushButton = new QPushButton(groupBox_7);
        StoppushButton->setObjectName("StoppushButton");
        sizePolicy2.setHeightForWidth(StoppushButton->sizePolicy().hasHeightForWidth());
        StoppushButton->setSizePolicy(sizePolicy2);

        horizontalLayout_12->addWidget(StoppushButton);

        ClearErrorpushButton = new QPushButton(groupBox_7);
        ClearErrorpushButton->setObjectName("ClearErrorpushButton");
        sizePolicy2.setHeightForWidth(ClearErrorpushButton->sizePolicy().hasHeightForWidth());
        ClearErrorpushButton->setSizePolicy(sizePolicy2);

        horizontalLayout_12->addWidget(ClearErrorpushButton);


        verticalLayout_4->addWidget(groupBox_7);

        groupBox_8 = new QGroupBox(groupBox_6);
        groupBox_8->setObjectName("groupBox_8");
        gridLayout_4 = new QGridLayout(groupBox_8);
        gridLayout_4->setObjectName("gridLayout_4");
        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName("horizontalLayout_13");
        label_14 = new QLabel(groupBox_8);
        label_14->setObjectName("label_14");
        sizePolicy.setHeightForWidth(label_14->sizePolicy().hasHeightForWidth());
        label_14->setSizePolicy(sizePolicy);

        horizontalLayout_13->addWidget(label_14);

        MaxSpeedlineEdit = new QLineEdit(groupBox_8);
        MaxSpeedlineEdit->setObjectName("MaxSpeedlineEdit");
        sizePolicy2.setHeightForWidth(MaxSpeedlineEdit->sizePolicy().hasHeightForWidth());
        MaxSpeedlineEdit->setSizePolicy(sizePolicy2);
        MaxSpeedlineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_13->addWidget(MaxSpeedlineEdit);

        label_15 = new QLabel(groupBox_8);
        label_15->setObjectName("label_15");
        sizePolicy.setHeightForWidth(label_15->sizePolicy().hasHeightForWidth());
        label_15->setSizePolicy(sizePolicy);

        horizontalLayout_13->addWidget(label_15);

        MinSpeedlineEdit = new QLineEdit(groupBox_8);
        MinSpeedlineEdit->setObjectName("MinSpeedlineEdit");
        sizePolicy2.setHeightForWidth(MinSpeedlineEdit->sizePolicy().hasHeightForWidth());
        MinSpeedlineEdit->setSizePolicy(sizePolicy2);
        MinSpeedlineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_13->addWidget(MinSpeedlineEdit);

        label_16 = new QLabel(groupBox_8);
        label_16->setObjectName("label_16");

        horizontalLayout_13->addWidget(label_16);

        AcclineEdit = new QLineEdit(groupBox_8);
        AcclineEdit->setObjectName("AcclineEdit");
        sizePolicy2.setHeightForWidth(AcclineEdit->sizePolicy().hasHeightForWidth());
        AcclineEdit->setSizePolicy(sizePolicy2);
        AcclineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_13->addWidget(AcclineEdit);

        label_17 = new QLabel(groupBox_8);
        label_17->setObjectName("label_17");

        horizontalLayout_13->addWidget(label_17);

        DeclineEdit = new QLineEdit(groupBox_8);
        DeclineEdit->setObjectName("DeclineEdit");
        sizePolicy2.setHeightForWidth(DeclineEdit->sizePolicy().hasHeightForWidth());
        DeclineEdit->setSizePolicy(sizePolicy2);
        DeclineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_13->addWidget(DeclineEdit);

        horizontalLayout_13->setStretch(0, 1);
        horizontalLayout_13->setStretch(1, 2);
        horizontalLayout_13->setStretch(2, 1);
        horizontalLayout_13->setStretch(3, 2);
        horizontalLayout_13->setStretch(4, 1);
        horizontalLayout_13->setStretch(5, 2);
        horizontalLayout_13->setStretch(6, 1);
        horizontalLayout_13->setStretch(7, 2);

        gridLayout_4->addLayout(horizontalLayout_13, 0, 0, 1, 1);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        ReadPara_pushButton = new QPushButton(groupBox_8);
        ReadPara_pushButton->setObjectName("ReadPara_pushButton");
        sizePolicy2.setHeightForWidth(ReadPara_pushButton->sizePolicy().hasHeightForWidth());
        ReadPara_pushButton->setSizePolicy(sizePolicy2);

        verticalLayout_3->addWidget(ReadPara_pushButton);

        SetParapushButton = new QPushButton(groupBox_8);
        SetParapushButton->setObjectName("SetParapushButton");
        sizePolicy2.setHeightForWidth(SetParapushButton->sizePolicy().hasHeightForWidth());
        SetParapushButton->setSizePolicy(sizePolicy2);

        verticalLayout_3->addWidget(SetParapushButton);

        SavepushButton = new QPushButton(groupBox_8);
        SavepushButton->setObjectName("SavepushButton");
        sizePolicy2.setHeightForWidth(SavepushButton->sizePolicy().hasHeightForWidth());
        SavepushButton->setSizePolicy(sizePolicy2);

        verticalLayout_3->addWidget(SavepushButton);

        ResetpushButton = new QPushButton(groupBox_8);
        ResetpushButton->setObjectName("ResetpushButton");
        sizePolicy2.setHeightForWidth(ResetpushButton->sizePolicy().hasHeightForWidth());
        ResetpushButton->setSizePolicy(sizePolicy2);

        verticalLayout_3->addWidget(ResetpushButton);


        gridLayout_4->addLayout(verticalLayout_3, 0, 1, 2, 1);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName("horizontalLayout_14");
        label_18 = new QLabel(groupBox_8);
        label_18->setObjectName("label_18");

        horizontalLayout_14->addWidget(label_18);

        CurrentlineEdit = new QLineEdit(groupBox_8);
        CurrentlineEdit->setObjectName("CurrentlineEdit");
        sizePolicy2.setHeightForWidth(CurrentlineEdit->sizePolicy().hasHeightForWidth());
        CurrentlineEdit->setSizePolicy(sizePolicy2);
        CurrentlineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_14->addWidget(CurrentlineEdit);

        label_19 = new QLabel(groupBox_8);
        label_19->setObjectName("label_19");
        sizePolicy.setHeightForWidth(label_19->sizePolicy().hasHeightForWidth());
        label_19->setSizePolicy(sizePolicy);

        horizontalLayout_14->addWidget(label_19);

        MaxChlineEdit = new QLineEdit(groupBox_8);
        MaxChlineEdit->setObjectName("MaxChlineEdit");
        sizePolicy2.setHeightForWidth(MaxChlineEdit->sizePolicy().hasHeightForWidth());
        MaxChlineEdit->setSizePolicy(sizePolicy2);
        MaxChlineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_14->addWidget(MaxChlineEdit);

        label_20 = new QLabel(groupBox_8);
        label_20->setObjectName("label_20");

        horizontalLayout_14->addWidget(label_20);

        CANBaudRatecomboBox = new QComboBox(groupBox_8);
        CANBaudRatecomboBox->addItem(QString());
        CANBaudRatecomboBox->addItem(QString());
        CANBaudRatecomboBox->addItem(QString());
        CANBaudRatecomboBox->addItem(QString());
        CANBaudRatecomboBox->addItem(QString());
        CANBaudRatecomboBox->addItem(QString());
        CANBaudRatecomboBox->setObjectName("CANBaudRatecomboBox");
        sizePolicy2.setHeightForWidth(CANBaudRatecomboBox->sizePolicy().hasHeightForWidth());
        CANBaudRatecomboBox->setSizePolicy(sizePolicy2);
        CANBaudRatecomboBox->setInsertPolicy(QComboBox::InsertAtBottom);
        CANBaudRatecomboBox->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);

        horizontalLayout_14->addWidget(CANBaudRatecomboBox);

        label_21 = new QLabel(groupBox_8);
        label_21->setObjectName("label_21");
        sizePolicy.setHeightForWidth(label_21->sizePolicy().hasHeightForWidth());
        label_21->setSizePolicy(sizePolicy);

        horizontalLayout_14->addWidget(label_21);

        SerBaudRatecomboBox = new QComboBox(groupBox_8);
        SerBaudRatecomboBox->addItem(QString());
        SerBaudRatecomboBox->addItem(QString());
        SerBaudRatecomboBox->addItem(QString());
        SerBaudRatecomboBox->addItem(QString());
        SerBaudRatecomboBox->addItem(QString());
        SerBaudRatecomboBox->setObjectName("SerBaudRatecomboBox");
        sizePolicy2.setHeightForWidth(SerBaudRatecomboBox->sizePolicy().hasHeightForWidth());
        SerBaudRatecomboBox->setSizePolicy(sizePolicy2);

        horizontalLayout_14->addWidget(SerBaudRatecomboBox);


        gridLayout_4->addLayout(horizontalLayout_14, 1, 0, 1, 1);


        verticalLayout_4->addWidget(groupBox_8);

        verticalLayout_4->setStretch(0, 1);
        verticalLayout_4->setStretch(1, 4);

        gridLayout_5->addWidget(groupBox_6, 0, 0, 1, 1);


        horizontalLayout_8->addWidget(Fncwidget);


        verticalLayout_2->addLayout(horizontalLayout_8);

        gridLayout_7 = new QGridLayout();
        gridLayout_7->setObjectName("gridLayout_7");
        gridLayout_7->setHorizontalSpacing(0);
        gridLayout_7->setVerticalSpacing(6);
        horizontalSpacer_8 = new QSpacerItem(858, 18, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_7->addItem(horizontalSpacer_8, 0, 1, 1, 1);

        LogplainTextEdit = new QPlainTextEdit(centralwidget);
        LogplainTextEdit->setObjectName("LogplainTextEdit");
        QSizePolicy sizePolicy8(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy8.setHorizontalStretch(0);
        sizePolicy8.setVerticalStretch(0);
        sizePolicy8.setHeightForWidth(LogplainTextEdit->sizePolicy().hasHeightForWidth());
        LogplainTextEdit->setSizePolicy(sizePolicy8);

        gridLayout_7->addWidget(LogplainTextEdit, 2, 0, 1, 3);

        ClearLogpushButton = new QPushButton(centralwidget);
        ClearLogpushButton->setObjectName("ClearLogpushButton");

        gridLayout_7->addWidget(ClearLogpushButton, 0, 2, 1, 1);

        label_9 = new QLabel(centralwidget);
        label_9->setObjectName("label_9");

        gridLayout_7->addWidget(label_9, 0, 0, 1, 1);


        verticalLayout_2->addLayout(gridLayout_7);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 969, 23));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "\344\270\262\345\217\243\351\205\215\347\275\256", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\344\270\262\345\217\243\345\217\267", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\346\263\242\347\211\271\347\216\207", nullptr));
        BaudratecomboBox->setItemText(0, QCoreApplication::translate("MainWindow", "9600", nullptr));
        BaudratecomboBox->setItemText(1, QCoreApplication::translate("MainWindow", "19200", nullptr));
        BaudratecomboBox->setItemText(2, QCoreApplication::translate("MainWindow", "38400", nullptr));
        BaudratecomboBox->setItemText(3, QCoreApplication::translate("MainWindow", "57600", nullptr));
        BaudratecomboBox->setItemText(4, QCoreApplication::translate("MainWindow", "115200", nullptr));

        pushButton->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200\344\270\262\345\217\243", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "\345\234\260\345\235\200\351\205\215\347\275\256", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\345\234\260\345\235\200", nullptr));
        CheckAddr->setText(QCoreApplication::translate("MainWindow", "\346\237\245\350\257\242", nullptr));
        ChangeAddr->setText(QCoreApplication::translate("MainWindow", "\344\277\256\346\224\271", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("MainWindow", "\350\275\257\344\273\266\347\211\210\346\234\254", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "\350\275\257\344\273\266\347\211\210\346\234\254", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\345\215\225\346\255\245\346\223\215\344\275\234", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\346\223\215\344\275\234", nullptr));
        ReadWritecomboBox->setItemText(0, QCoreApplication::translate("MainWindow", "\350\257\273\345\257\204\345\255\230\345\231\250", nullptr));
        ReadWritecomboBox->setItemText(1, QCoreApplication::translate("MainWindow", "\345\206\231\345\215\225\344\270\252\345\257\204\345\255\230\345\231\250", nullptr));
        ReadWritecomboBox->setItemText(2, QCoreApplication::translate("MainWindow", "\345\206\231\345\244\232\344\270\252\345\257\204\345\255\230\345\231\250", nullptr));

        label_6->setText(QCoreApplication::translate("MainWindow", "\345\257\204\345\255\230\345\231\250\345\234\260\345\235\200", nullptr));
#if QT_CONFIG(whatsthis)
        RegAddrlineEdit->setWhatsThis(QCoreApplication::translate("MainWindow", "<html><head/><body><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        RegAddrlineEdit->setText(QString());
        RegAddrlineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "16\350\277\233\345\210\266", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "\345\257\204\345\255\230\345\231\250\344\270\252\346\225\260", nullptr));
#if QT_CONFIG(whatsthis)
        WriteNumlineEdit->setWhatsThis(QCoreApplication::translate("MainWindow", "<html><head/><body><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(whatsthis)
        WriteNumlineEdit->setText(QString());
        WriteNumlineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "10\350\277\233\345\210\266", nullptr));
        CmdSendpushButton->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256", nullptr));
        DatalineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "10\350\277\233\345\210\266", nullptr));
        HexcheckBox->setText(QCoreApplication::translate("MainWindow", "16\350\277\233\345\210\266", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("MainWindow", "\345\257\204\345\255\230\345\231\250\346\237\245\350\257\242", nullptr));
        ClearLinespushButton->setText(QCoreApplication::translate("MainWindow", "\346\270\205\347\251\272", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "\351\227\264\351\232\224\346\227\266\351\227\264", nullptr));
        GapTimelineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "ms", nullptr));
        QTableWidgetItem *___qtablewidgetitem = ReadMultableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "\345\257\204\345\255\230\345\231\250(0x)", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = ReadMultableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256(int)", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "\350\257\273\345\257\204\345\255\230\345\231\250\345\274\200\345\247\213\345\234\260\345\235\200", nullptr));
        RegStartlineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "16\350\277\233\345\210\266", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "\350\257\273\345\217\226\344\270\252\346\225\260", nullptr));
        ReadNumlineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "10\350\277\233\345\210\266", nullptr));
        AddRowpushButton->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240", nullptr));
        LoopReadcheckBox->setText(QCoreApplication::translate("MainWindow", "\345\276\252\347\216\257\350\257\273\345\217\226", nullptr));
        StartReadpushButton->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\350\257\273\345\217\226", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("MainWindow", "\351\230\200\346\216\247\345\210\266", nullptr));
        groupBox_7->setTitle(QCoreApplication::translate("MainWindow", "\350\277\220\345\212\250\346\216\247\345\210\266", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "\346\214\207\345\256\232\351\200\232\351\201\223", nullptr));
        FastSwitchpushButton->setText(QCoreApplication::translate("MainWindow", "\346\234\200\344\274\230\350\267\257\345\276\204", nullptr));
        CounterclockwisepushButton->setText(QCoreApplication::translate("MainWindow", "\351\200\206\346\227\266\351\222\210\345\210\207\346\215\242", nullptr));
        ClockwisepushButton->setText(QCoreApplication::translate("MainWindow", "\351\241\272\346\227\266\351\222\210\345\210\207\346\215\242", nullptr));
        InitpushButton->setText(QCoreApplication::translate("MainWindow", "\347\275\256\351\233\266", nullptr));
        StoppushButton->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242", nullptr));
        ClearErrorpushButton->setText(QCoreApplication::translate("MainWindow", "\346\270\205\351\231\244\351\224\231\350\257\257", nullptr));
        groupBox_8->setTitle(QCoreApplication::translate("MainWindow", "\345\217\202\346\225\260\351\205\215\347\275\256", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "\346\234\200\345\244\247\351\200\237\345\272\246", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "\346\234\200\345\260\217\351\200\237\345\272\246", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "\345\212\240\351\200\237\345\272\246", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "\345\207\217\351\200\237\345\272\246", nullptr));
        ReadPara_pushButton->setText(QCoreApplication::translate("MainWindow", "\350\257\273\345\217\226\345\217\202\346\225\260", nullptr));
        SetParapushButton->setText(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256\345\217\202\346\225\260", nullptr));
        SavepushButton->setText(QCoreApplication::translate("MainWindow", "\346\216\211\347\224\265\344\277\235\345\255\230", nullptr));
        ResetpushButton->setText(QCoreApplication::translate("MainWindow", "\346\201\242\345\244\215\345\207\272\345\216\202", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "\351\242\235\345\256\232\347\224\265\346\265\201", nullptr));
        CurrentlineEdit->setPlaceholderText(QString());
        label_19->setText(QCoreApplication::translate("MainWindow", "\346\234\200\345\244\247\351\200\232\351\201\223\346\225\260", nullptr));
        label_20->setText(QCoreApplication::translate("MainWindow", "CAN\346\263\242\347\211\271\347\216\207", nullptr));
        CANBaudRatecomboBox->setItemText(0, QCoreApplication::translate("MainWindow", "100", nullptr));
        CANBaudRatecomboBox->setItemText(1, QCoreApplication::translate("MainWindow", "125", nullptr));
        CANBaudRatecomboBox->setItemText(2, QCoreApplication::translate("MainWindow", "250", nullptr));
        CANBaudRatecomboBox->setItemText(3, QCoreApplication::translate("MainWindow", "500", nullptr));
        CANBaudRatecomboBox->setItemText(4, QCoreApplication::translate("MainWindow", "800", nullptr));
        CANBaudRatecomboBox->setItemText(5, QCoreApplication::translate("MainWindow", "1000", nullptr));

        label_21->setText(QCoreApplication::translate("MainWindow", "\344\270\262\345\217\243\346\263\242\347\211\271\347\216\207", nullptr));
        SerBaudRatecomboBox->setItemText(0, QCoreApplication::translate("MainWindow", "9600", nullptr));
        SerBaudRatecomboBox->setItemText(1, QCoreApplication::translate("MainWindow", "19200", nullptr));
        SerBaudRatecomboBox->setItemText(2, QCoreApplication::translate("MainWindow", "38400", nullptr));
        SerBaudRatecomboBox->setItemText(3, QCoreApplication::translate("MainWindow", "57600", nullptr));
        SerBaudRatecomboBox->setItemText(4, QCoreApplication::translate("MainWindow", "115200", nullptr));

        ClearLogpushButton->setText(QCoreApplication::translate("MainWindow", "\346\270\205\347\251\272\346\227\245\345\277\227", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "\346\227\245\345\277\227\346\230\276\347\244\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
