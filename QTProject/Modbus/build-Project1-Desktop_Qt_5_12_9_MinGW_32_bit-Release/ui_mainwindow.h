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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QComboBox *SerialcomboBox;
    QPushButton *pushButton;
    QComboBox *BaudratecomboBox;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *AddrlineEdit;
    QPushButton *CheckAddr;
    QPushButton *ChangeAddr;
    QLabel *label_4;
    QComboBox *ReadWritecomboBox;
    QLabel *label_5;
    QLineEdit *DatalineEdit;
    QLabel *label_6;
    QLineEdit *RegAddrlineEdit;
    QPushButton *CmdSendpushButton;
    QCheckBox *HexcheckBox;
    QTableWidget *ReadMultableWidget;
    QLabel *label_7;
    QLineEdit *RegStartlineEdit;
    QLabel *label_8;
    QLineEdit *ReadNumlineEdit;
    QPushButton *StartReadpushButton;
    QPushButton *AddRowpushButton;
    QPushButton *ClearLinespushButton;
    QPlainTextEdit *LogplainTextEdit;
    QLabel *label_9;
    QPushButton *ClearLogpushButton;
    QLabel *label_10;
    QLineEdit *DatalineEdit_2;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(665, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        SerialcomboBox = new QComboBox(centralwidget);
        SerialcomboBox->setObjectName(QString::fromUtf8("SerialcomboBox"));
        SerialcomboBox->setEnabled(true);
        SerialcomboBox->setGeometry(QRect(50, 0, 69, 22));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(120, 0, 71, 51));
        pushButton->setCheckable(true);
        pushButton->setChecked(true);
        BaudratecomboBox = new QComboBox(centralwidget);
        BaudratecomboBox->addItem(QString());
        BaudratecomboBox->addItem(QString());
        BaudratecomboBox->addItem(QString());
        BaudratecomboBox->addItem(QString());
        BaudratecomboBox->addItem(QString());
        BaudratecomboBox->addItem(QString());
        BaudratecomboBox->setObjectName(QString::fromUtf8("BaudratecomboBox"));
        BaudratecomboBox->setEnabled(true);
        BaudratecomboBox->setGeometry(QRect(50, 30, 69, 22));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 0, 41, 21));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 30, 41, 20));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(240, 10, 31, 21));
        AddrlineEdit = new QLineEdit(centralwidget);
        AddrlineEdit->setObjectName(QString::fromUtf8("AddrlineEdit"));
        AddrlineEdit->setGeometry(QRect(270, 10, 41, 21));
        CheckAddr = new QPushButton(centralwidget);
        CheckAddr->setObjectName(QString::fromUtf8("CheckAddr"));
        CheckAddr->setGeometry(QRect(320, 10, 31, 21));
        ChangeAddr = new QPushButton(centralwidget);
        ChangeAddr->setObjectName(QString::fromUtf8("ChangeAddr"));
        ChangeAddr->setGeometry(QRect(360, 10, 31, 21));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 100, 31, 21));
        ReadWritecomboBox = new QComboBox(centralwidget);
        ReadWritecomboBox->addItem(QString());
        ReadWritecomboBox->addItem(QString());
        ReadWritecomboBox->addItem(QString());
        ReadWritecomboBox->setObjectName(QString::fromUtf8("ReadWritecomboBox"));
        ReadWritecomboBox->setGeometry(QRect(40, 100, 69, 22));
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(270, 100, 31, 21));
        DatalineEdit = new QLineEdit(centralwidget);
        DatalineEdit->setObjectName(QString::fromUtf8("DatalineEdit"));
        DatalineEdit->setGeometry(QRect(300, 100, 71, 20));
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(120, 100, 71, 21));
        RegAddrlineEdit = new QLineEdit(centralwidget);
        RegAddrlineEdit->setObjectName(QString::fromUtf8("RegAddrlineEdit"));
        RegAddrlineEdit->setGeometry(QRect(190, 100, 61, 20));
        CmdSendpushButton = new QPushButton(centralwidget);
        CmdSendpushButton->setObjectName(QString::fromUtf8("CmdSendpushButton"));
        CmdSendpushButton->setGeometry(QRect(440, 100, 75, 23));
        HexcheckBox = new QCheckBox(centralwidget);
        HexcheckBox->setObjectName(QString::fromUtf8("HexcheckBox"));
        HexcheckBox->setGeometry(QRect(380, 100, 71, 21));
        ReadMultableWidget = new QTableWidget(centralwidget);
        if (ReadMultableWidget->columnCount() < 2)
            ReadMultableWidget->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        ReadMultableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        ReadMultableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        ReadMultableWidget->setObjectName(QString::fromUtf8("ReadMultableWidget"));
        ReadMultableWidget->setGeometry(QRect(10, 130, 211, 192));
        label_7 = new QLabel(centralwidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(230, 130, 101, 21));
        RegStartlineEdit = new QLineEdit(centralwidget);
        RegStartlineEdit->setObjectName(QString::fromUtf8("RegStartlineEdit"));
        RegStartlineEdit->setGeometry(QRect(230, 150, 101, 20));
        label_8 = new QLabel(centralwidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(230, 170, 51, 21));
        ReadNumlineEdit = new QLineEdit(centralwidget);
        ReadNumlineEdit->setObjectName(QString::fromUtf8("ReadNumlineEdit"));
        ReadNumlineEdit->setGeometry(QRect(230, 190, 51, 20));
        StartReadpushButton = new QPushButton(centralwidget);
        StartReadpushButton->setObjectName(QString::fromUtf8("StartReadpushButton"));
        StartReadpushButton->setGeometry(QRect(230, 220, 75, 23));
        AddRowpushButton = new QPushButton(centralwidget);
        AddRowpushButton->setObjectName(QString::fromUtf8("AddRowpushButton"));
        AddRowpushButton->setGeometry(QRect(10, 330, 75, 23));
        ClearLinespushButton = new QPushButton(centralwidget);
        ClearLinespushButton->setObjectName(QString::fromUtf8("ClearLinespushButton"));
        ClearLinespushButton->setGeometry(QRect(140, 330, 75, 23));
        LogplainTextEdit = new QPlainTextEdit(centralwidget);
        LogplainTextEdit->setObjectName(QString::fromUtf8("LogplainTextEdit"));
        LogplainTextEdit->setGeometry(QRect(10, 400, 631, 161));
        label_9 = new QLabel(centralwidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(10, 380, 54, 20));
        ClearLogpushButton = new QPushButton(centralwidget);
        ClearLogpushButton->setObjectName(QString::fromUtf8("ClearLogpushButton"));
        ClearLogpushButton->setGeometry(QRect(550, 370, 75, 23));
        label_10 = new QLabel(centralwidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(450, 10, 51, 21));
        DatalineEdit_2 = new QLineEdit(centralwidget);
        DatalineEdit_2->setObjectName(QString::fromUtf8("DatalineEdit_2"));
        DatalineEdit_2->setGeometry(QRect(510, 10, 111, 20));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 665, 23));
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
        pushButton->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\344\270\262\345\217\243", nullptr));
        BaudratecomboBox->setItemText(0, QApplication::translate("MainWindow", "9600", nullptr));
        BaudratecomboBox->setItemText(1, QApplication::translate("MainWindow", "38400", nullptr));
        BaudratecomboBox->setItemText(2, QApplication::translate("MainWindow", "19200", nullptr));
        BaudratecomboBox->setItemText(3, QApplication::translate("MainWindow", "38400", nullptr));
        BaudratecomboBox->setItemText(4, QApplication::translate("MainWindow", "57600", nullptr));
        BaudratecomboBox->setItemText(5, QApplication::translate("MainWindow", "115200", nullptr));

        label->setText(QApplication::translate("MainWindow", "\344\270\262\345\217\243\345\217\267", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "\346\263\242\347\211\271\347\216\207", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "\345\234\260\345\235\200", nullptr));
        CheckAddr->setText(QApplication::translate("MainWindow", "\346\237\245\350\257\242", nullptr));
        ChangeAddr->setText(QApplication::translate("MainWindow", "\344\277\256\346\224\271", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "\346\223\215\344\275\234", nullptr));
        ReadWritecomboBox->setItemText(0, QApplication::translate("MainWindow", "\350\257\273\345\257\204\345\255\230\345\231\250", nullptr));
        ReadWritecomboBox->setItemText(1, QApplication::translate("MainWindow", "\345\206\231\345\215\225\344\270\252\345\257\204\345\255\230\345\231\250", nullptr));
        ReadWritecomboBox->setItemText(2, QApplication::translate("MainWindow", "\345\206\231\345\244\232\344\270\252\345\257\204\345\255\230\345\231\250", nullptr));

        label_5->setText(QApplication::translate("MainWindow", "\346\225\260\346\215\256", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "\345\257\204\345\255\230\345\231\250\345\234\260\345\235\200", nullptr));
        CmdSendpushButton->setText(QApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        HexcheckBox->setText(QApplication::translate("MainWindow", "16\350\277\233\345\210\266", nullptr));
        QTableWidgetItem *___qtablewidgetitem = ReadMultableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MainWindow", "\345\257\204\345\255\230\345\231\250\345\234\260\345\235\200", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = ReadMultableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("MainWindow", "\346\225\260\346\215\256", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "\350\257\273\345\257\204\345\255\230\345\231\250\345\274\200\345\247\213\345\234\260\345\235\200", nullptr));
        label_8->setText(QApplication::translate("MainWindow", "\350\257\273\345\217\226\344\270\252\346\225\260", nullptr));
        StartReadpushButton->setText(QApplication::translate("MainWindow", "\345\274\200\345\247\213\350\257\273\345\217\226", nullptr));
        AddRowpushButton->setText(QApplication::translate("MainWindow", "\346\267\273\345\212\240\344\270\200\350\241\214", nullptr));
        ClearLinespushButton->setText(QApplication::translate("MainWindow", "\346\270\205\347\251\272", nullptr));
        label_9->setText(QApplication::translate("MainWindow", "\346\227\245\345\277\227\346\230\276\347\244\272", nullptr));
        ClearLogpushButton->setText(QApplication::translate("MainWindow", "\346\270\205\347\251\272\346\227\245\345\277\227", nullptr));
        label_10->setText(QApplication::translate("MainWindow", "\350\275\257\344\273\266\347\211\210\346\234\254", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
