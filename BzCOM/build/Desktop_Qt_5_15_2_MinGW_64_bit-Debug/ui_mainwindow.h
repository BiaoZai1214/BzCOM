/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QTextBrowser *textBrowser;
    QTabWidget *tabWidget;
    QHBoxLayout *horizontalLayout_6;
    QTextEdit *textEdit;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *clearLog;
    QPushButton *saveLog;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *hexBtn;
    QComboBox *checkBox;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *selectFile;
    QComboBox *ProtocolBox;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *fileBtn;
    QPushButton *sendBtn;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 480);
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        MainWindow->setFont(font);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_3 = new QVBoxLayout(centralwidget);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        textBrowser = new QTextBrowser(centralwidget);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));

        horizontalLayout->addWidget(textBrowser);

        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));

        horizontalLayout->addWidget(tabWidget);

        horizontalLayout->setStretch(0, 5);
        horizontalLayout->setStretch(1, 2);

        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        textEdit = new QTextEdit(centralwidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));

        horizontalLayout_6->addWidget(textEdit);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        clearLog = new QPushButton(centralwidget);
        clearLog->setObjectName(QString::fromUtf8("clearLog"));
        clearLog->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: rgba(255, 255, 255, 0.8);\n"
"    color: #1d1d1f;\n"
"    border: 1px solid rgba(0, 0, 0, 0.1);\n"
"    border-radius: 8px;\n"
"    font-size: 13px;\n"
"    padding: 8px 16px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgba(255, 255, 255, 0.9);\n"
"    border-color: rgba(0, 0, 0, 0.15);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: rgba(245, 245, 247, 0.9);\n"
"}"));

        horizontalLayout_2->addWidget(clearLog);

        saveLog = new QPushButton(centralwidget);
        saveLog->setObjectName(QString::fromUtf8("saveLog"));
        saveLog->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: rgba(255, 255, 255, 0.8);\n"
"    color: #1d1d1f;\n"
"    border: 1px solid rgba(0, 0, 0, 0.1);\n"
"    border-radius: 8px;\n"
"    font-size: 13px;\n"
"    padding: 8px 16px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgba(255, 255, 255, 0.9);\n"
"    border-color: rgba(0, 0, 0, 0.15);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: rgba(245, 245, 247, 0.9);\n"
"}"));

        horizontalLayout_2->addWidget(saveLog);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        hexBtn = new QPushButton(centralwidget);
        hexBtn->setObjectName(QString::fromUtf8("hexBtn"));
        hexBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: rgba(255, 255, 255, 0.8);\n"
"    color: #1d1d1f;\n"
"    border: 1px solid rgba(0, 0, 0, 0.1);\n"
"    border-radius: 8px;\n"
"    font-size: 13px;\n"
"    padding: 8px 16px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgba(255, 255, 255, 0.9);\n"
"    border-color: rgba(0, 0, 0, 0.15);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: rgba(245, 245, 247, 0.9);\n"
"}"));

        horizontalLayout_3->addWidget(hexBtn);

        checkBox = new QComboBox(centralwidget);
        checkBox->addItem(QString());
        checkBox->addItem(QString());
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setFont(font);
        checkBox->setStyleSheet(QString::fromUtf8("/* \347\273\237\344\270\200\346\211\200\346\234\211\350\276\223\345\205\245\346\241\206\357\274\210QLineEdit + QComboBox\357\274\211\347\232\204\346\240\267\345\274\217 */\n"
" QComboBox {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e0e0e0;\n"
"    border-radius: 6px;\n"
"    padding: 6px 8px;\n"
"    font-size: 14px;\n"
"    color: #333333;\n"
"}\n"
"\n"
"/* \345\275\273\345\272\225\351\232\220\350\227\217QComboBox\344\270\213\346\213\211\345\214\272\345\237\237\357\274\232\345\256\275\345\272\246\350\256\276\344\270\2720\357\274\214\346\227\240\344\273\273\344\275\225\344\272\244\344\272\222 */\n"
"QComboBox::drop-down {\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    width: 0px; /* \345\205\263\351\224\256\357\274\232\345\260\206\344\270\213\346\213\211\345\214\272\345\237\237\345\256\275\345\272\246\350\256\276\344\270\2720\357\274\214\350\247\206\350\247\211\344\270\212\345\256\214\345\205\250\346\266\210\345\244\261 */\n"
"}\n"
"QComboBox::down-arrow {\n"
"    image"
                        ": none; /* \346\270\205\347\251\272\347\256\255\345\244\264\345\233\276\346\240\207\357\274\214\351\201\277\345\205\215\350\265\204\346\272\220\345\212\240\350\275\275\350\255\246\345\221\212 */\n"
"    width: 0px;\n"
"    height: 0px;\n"
"}\n"
"\n"
"/* \345\217\257\351\200\211\357\274\232QComboBox \347\274\226\350\276\221\345\214\272\345\237\237\346\226\207\345\255\227\345\261\205\344\270\255\357\274\210\345\205\274\345\256\271\345\217\257\347\274\226\350\276\221\345\234\272\346\231\257\357\274\211 */\n"
"QComboBox QAbstractItemView {\n"
"    text-align: center;\n"
"    alignment: AlignCenter;\n"
"}"));

        horizontalLayout_3->addWidget(checkBox);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(9);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        selectFile = new QPushButton(centralwidget);
        selectFile->setObjectName(QString::fromUtf8("selectFile"));
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(selectFile->sizePolicy().hasHeightForWidth());
        selectFile->setSizePolicy(sizePolicy);
        selectFile->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: rgba(255, 255, 255, 0.8);\n"
"    color: #1d1d1f;\n"
"    border: 1px solid rgba(0, 0, 0, 0.1);\n"
"    border-radius: 8px;\n"
"    font-size: 13px;\n"
"    padding: 8px 16px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgba(255, 255, 255, 0.9);\n"
"    border-color: rgba(0, 0, 0, 0.15);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: rgba(245, 245, 247, 0.9);\n"
"}"));

        horizontalLayout_4->addWidget(selectFile);

        ProtocolBox = new QComboBox(centralwidget);
        ProtocolBox->addItem(QString());
        ProtocolBox->addItem(QString());
        ProtocolBox->setObjectName(QString::fromUtf8("ProtocolBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(ProtocolBox->sizePolicy().hasHeightForWidth());
        ProtocolBox->setSizePolicy(sizePolicy1);
        ProtocolBox->setStyleSheet(QString::fromUtf8("/* \347\273\237\344\270\200\346\211\200\346\234\211\350\276\223\345\205\245\346\241\206\357\274\210QLineEdit + QComboBox\357\274\211\347\232\204\346\240\267\345\274\217 */\n"
" QComboBox {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e0e0e0;\n"
"    border-radius: 6px;\n"
"    padding: 6px 8px;\n"
"    font-size: 14px;\n"
"    color: #333333;\n"
"}\n"
"\n"
"/* \345\275\273\345\272\225\351\232\220\350\227\217QComboBox\344\270\213\346\213\211\345\214\272\345\237\237\357\274\232\345\256\275\345\272\246\350\256\276\344\270\2720\357\274\214\346\227\240\344\273\273\344\275\225\344\272\244\344\272\222 */\n"
"QComboBox::drop-down {\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    width: 0px; /* \345\205\263\351\224\256\357\274\232\345\260\206\344\270\213\346\213\211\345\214\272\345\237\237\345\256\275\345\272\246\350\256\276\344\270\2720\357\274\214\350\247\206\350\247\211\344\270\212\345\256\214\345\205\250\346\266\210\345\244\261 */\n"
"}\n"
"QComboBox::down-arrow {\n"
"    image"
                        ": none; /* \346\270\205\347\251\272\347\256\255\345\244\264\345\233\276\346\240\207\357\274\214\351\201\277\345\205\215\350\265\204\346\272\220\345\212\240\350\275\275\350\255\246\345\221\212 */\n"
"    width: 0px;\n"
"    height: 0px;\n"
"}\n"
"\n"
"/* \345\217\257\351\200\211\357\274\232QComboBox \347\274\226\350\276\221\345\214\272\345\237\237\346\226\207\345\255\227\345\261\205\344\270\255\357\274\210\345\205\274\345\256\271\345\217\257\347\274\226\350\276\221\345\234\272\346\231\257\357\274\211 */\n"
"QComboBox QAbstractItemView {\n"
"    text-align: center;\n"
"    alignment: AlignCenter;\n"
"}"));

        horizontalLayout_4->addWidget(ProtocolBox);

        horizontalLayout_4->setStretch(0, 1);
        horizontalLayout_4->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(10);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        fileBtn = new QPushButton(centralwidget);
        fileBtn->setObjectName(QString::fromUtf8("fileBtn"));
        fileBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: rgba(255, 255, 255, 0.8);\n"
"    color: #1d1d1f;\n"
"    border: 1px solid rgba(0, 0, 0, 0.1);\n"
"    border-radius: 8px;\n"
"    font-size: 13px;\n"
"    padding: 8px 16px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgba(255, 255, 255, 0.9);\n"
"    border-color: rgba(0, 0, 0, 0.15);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: rgba(245, 245, 247, 0.9);\n"
"}"));

        horizontalLayout_5->addWidget(fileBtn);

        sendBtn = new QPushButton(centralwidget);
        sendBtn->setObjectName(QString::fromUtf8("sendBtn"));
        sendBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: rgba(255, 255, 255, 0.8);\n"
"    color: #1d1d1f;\n"
"    border: 1px solid rgba(0, 0, 0, 0.1);\n"
"    border-radius: 8px;\n"
"    font-size: 13px;\n"
"    padding: 8px 16px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgba(255, 255, 255, 0.9);\n"
"    border-color: rgba(0, 0, 0, 0.15);\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: rgba(245, 245, 247, 0.9);\n"
"}"));

        horizontalLayout_5->addWidget(sendBtn);

        horizontalLayout_5->setStretch(0, 1);
        horizontalLayout_5->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout_5);


        horizontalLayout_6->addLayout(verticalLayout);

        horizontalLayout_6->setStretch(0, 5);
        horizontalLayout_6->setStretch(1, 2);

        verticalLayout_2->addLayout(horizontalLayout_6);

        verticalLayout_2->setStretch(0, 3);
        verticalLayout_2->setStretch(1, 2);

        verticalLayout_3->addLayout(verticalLayout_2);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        clearLog->setText(QCoreApplication::translate("MainWindow", "\346\270\205\347\251\272\346\227\245\345\277\227", nullptr));
        saveLog->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\346\227\245\345\277\227", nullptr));
        hexBtn->setText(QCoreApplication::translate("MainWindow", "ASCII", nullptr));
        checkBox->setItemText(0, QCoreApplication::translate("MainWindow", "     \346\227\240\346\240\241\351\252\214", nullptr));
        checkBox->setItemText(1, QCoreApplication::translate("MainWindow", "\346\240\241\351\252\214\345\222\214", nullptr));

        selectFile->setText(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\346\226\207\344\273\266", nullptr));
        ProtocolBox->setItemText(0, QCoreApplication::translate("MainWindow", "     None", nullptr));
        ProtocolBox->setItemText(1, QCoreApplication::translate("MainWindow", "CRC16-2-2", nullptr));

        fileBtn->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\346\226\207\344\273\266", nullptr));
        sendBtn->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\346\226\207\346\234\254", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
