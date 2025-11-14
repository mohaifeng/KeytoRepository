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
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "QtCharts"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_5;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *DevTypecomboBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QComboBox *DevIdexcomboBox;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QComboBox *CANIdexcomboBox;
    QPushButton *OpenCANpushButton;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_2;
    QChartView *chartwidget;
    QPlainTextEdit *LogplainTextEdit;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(892, 670);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        horizontalLayout_4 = new QHBoxLayout(groupBox);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
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


        horizontalLayout_4->addLayout(horizontalLayout);

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


        horizontalLayout_4->addLayout(horizontalLayout_2);

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


        horizontalLayout_4->addLayout(horizontalLayout_3);

        OpenCANpushButton = new QPushButton(groupBox);
        OpenCANpushButton->setObjectName(QString::fromUtf8("OpenCANpushButton"));

        horizontalLayout_4->addWidget(OpenCANpushButton);


        horizontalLayout_5->addWidget(groupBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalSpacer_2 = new QSpacerItem(438, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_2);

        chartwidget = new QChartView(centralwidget);
        chartwidget->setObjectName(QString::fromUtf8("chartwidget"));
        chartwidget->setMouseTracking(false);

        horizontalLayout_6->addWidget(chartwidget);

        horizontalLayout_6->setStretch(0, 1);
        horizontalLayout_6->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout_6);

        LogplainTextEdit = new QPlainTextEdit(centralwidget);
        LogplainTextEdit->setObjectName(QString::fromUtf8("LogplainTextEdit"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(LogplainTextEdit->sizePolicy().hasHeightForWidth());
        LogplainTextEdit->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(LogplainTextEdit);

        verticalLayout->setStretch(0, 1);
        verticalLayout->setStretch(1, 5);
        verticalLayout->setStretch(2, 2);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 892, 23));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

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

        OpenCANpushButton->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
