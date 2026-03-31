/********************************************************************************
** Form generated from reading UI file 'uartwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UARTWIDGET_H
#define UI_UARTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_uartWidget
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QComboBox *bandRateBox;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QComboBox *dataBitBox;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QComboBox *checkBitBox;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QComboBox *stopBitBox;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *uartWidget)
    {
        if (uartWidget->objectName().isEmpty())
            uartWidget->setObjectName(QString::fromUtf8("uartWidget"));
        uartWidget->resize(220, 270);
        verticalLayout_2 = new QVBoxLayout(uartWidget);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(5, 5, 5, 5);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(20, -1, 20, -1);
        label_2 = new QLabel(uartWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(label_2);

        bandRateBox = new QComboBox(uartWidget);
        bandRateBox->addItem(QString());
        bandRateBox->addItem(QString());
        bandRateBox->setObjectName(QString::fromUtf8("bandRateBox"));
        bandRateBox->setMinimumSize(QSize(0, 0));
        bandRateBox->setMaximumSize(QSize(100, 32));
        bandRateBox->setStyleSheet(QString::fromUtf8("/* \347\273\237\344\270\200\346\211\200\346\234\211\350\276\223\345\205\245\346\241\206\357\274\210QLineEdit + QComboBox\357\274\211\347\232\204\346\240\267\345\274\217 */\n"
"QLineEdit, QComboBox {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e0e0e0;\n"
"    border-radius: 6px;\n"
"    padding: 6px 8px;\n"
"    font-size: 16px;\n"
"    color: #333333;\n"
"}\n"
"\n"
"/* QComboBox \344\270\213\346\213\211\347\256\255\345\244\264\345\214\272\345\237\237\346\240\267\345\274\217\357\274\210\351\201\277\345\205\215\351\242\235\345\244\226\345\207\270\350\265\267 + \346\227\240\350\255\246\345\221\212\357\274\211 */\n"
"QComboBox::drop-down {\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    width: 20px;\n"
"}\n"
"QComboBox::down-arrow {\n"
"    image: none; /* \346\270\205\347\251\272\347\256\255\345\244\264\357\274\214\346\227\240\350\265\204\346\272\220\345\212\240\350\275\275\350\255\246\345\221\212 */\n"
"    width: 12px;\n"
"    height: 12px;\n"
"}\n"
"\n"
"/* \350\201\232\347"
                        "\204\246\347\212\266\346\200\201\344\277\235\346\214\201\344\270\200\350\207\264 */\n"
"QLineEdit:focus, QComboBox:focus {\n"
"    border: 1px solid #40a9ff;\n"
"    outline: none;\n"
"}"));

        horizontalLayout_2->addWidget(bandRateBox);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(20, -1, 20, -1);
        label_3 = new QLabel(uartWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy);

        horizontalLayout_3->addWidget(label_3);

        dataBitBox = new QComboBox(uartWidget);
        dataBitBox->addItem(QString());
        dataBitBox->addItem(QString());
        dataBitBox->addItem(QString());
        dataBitBox->addItem(QString());
        dataBitBox->setObjectName(QString::fromUtf8("dataBitBox"));
        dataBitBox->setMinimumSize(QSize(95, 0));
        dataBitBox->setMaximumSize(QSize(100, 32));
        dataBitBox->setStyleSheet(QString::fromUtf8("/* \347\273\237\344\270\200\346\211\200\346\234\211\350\276\223\345\205\245\346\241\206\357\274\210QLineEdit + QComboBox\357\274\211\347\232\204\346\240\267\345\274\217 */\n"
"QLineEdit, QComboBox {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e0e0e0;\n"
"    border-radius: 6px;\n"
"    padding: 6px 8px;\n"
"    font-size: 16px;\n"
"    color: #333333;\n"
"}\n"
"\n"
"/* QComboBox \344\270\213\346\213\211\347\256\255\345\244\264\345\214\272\345\237\237\346\240\267\345\274\217\357\274\210\351\201\277\345\205\215\351\242\235\345\244\226\345\207\270\350\265\267 + \346\227\240\350\255\246\345\221\212\357\274\211 */\n"
"QComboBox::drop-down {\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    width: 20px;\n"
"}\n"
"QComboBox::down-arrow {\n"
"    image: none; /* \346\270\205\347\251\272\347\256\255\345\244\264\357\274\214\346\227\240\350\265\204\346\272\220\345\212\240\350\275\275\350\255\246\345\221\212 */\n"
"    width: 12px;\n"
"    height: 12px;\n"
"}\n"
"\n"
"/* \350\201\232\347"
                        "\204\246\347\212\266\346\200\201\344\277\235\346\214\201\344\270\200\350\207\264 */\n"
"QLineEdit:focus, QComboBox:focus {\n"
"    border: 1px solid #40a9ff;\n"
"    outline: none;\n"
"}"));

        horizontalLayout_3->addWidget(dataBitBox);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(20, -1, 20, -1);
        label_4 = new QLabel(uartWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        sizePolicy.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy);

        horizontalLayout_4->addWidget(label_4);

        checkBitBox = new QComboBox(uartWidget);
        checkBitBox->addItem(QString());
        checkBitBox->addItem(QString());
        checkBitBox->addItem(QString());
        checkBitBox->addItem(QString());
        checkBitBox->addItem(QString());
        checkBitBox->setObjectName(QString::fromUtf8("checkBitBox"));
        checkBitBox->setMinimumSize(QSize(95, 0));
        checkBitBox->setMaximumSize(QSize(100, 32));
        checkBitBox->setStyleSheet(QString::fromUtf8("/* \347\273\237\344\270\200\346\211\200\346\234\211\350\276\223\345\205\245\346\241\206\357\274\210QLineEdit + QComboBox\357\274\211\347\232\204\346\240\267\345\274\217 */\n"
"QLineEdit, QComboBox {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e0e0e0;\n"
"    border-radius: 6px;\n"
"    padding: 6px 8px;\n"
"    font-size: 16px;\n"
"    color: #333333;\n"
"}\n"
"\n"
"/* QComboBox \344\270\213\346\213\211\347\256\255\345\244\264\345\214\272\345\237\237\346\240\267\345\274\217\357\274\210\351\201\277\345\205\215\351\242\235\345\244\226\345\207\270\350\265\267 + \346\227\240\350\255\246\345\221\212\357\274\211 */\n"
"QComboBox::drop-down {\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    width: 20px;\n"
"}\n"
"QComboBox::down-arrow {\n"
"    image: none; /* \346\270\205\347\251\272\347\256\255\345\244\264\357\274\214\346\227\240\350\265\204\346\272\220\345\212\240\350\275\275\350\255\246\345\221\212 */\n"
"    width: 12px;\n"
"    height: 12px;\n"
"}\n"
"\n"
"/* \350\201\232\347"
                        "\204\246\347\212\266\346\200\201\344\277\235\346\214\201\344\270\200\350\207\264 */\n"
"QLineEdit:focus, QComboBox:focus {\n"
"    border: 1px solid #40a9ff;\n"
"    outline: none;\n"
"}"));

        horizontalLayout_4->addWidget(checkBitBox);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(7);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(20, -1, 20, -1);
        label_5 = new QLabel(uartWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        sizePolicy.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy);

        horizontalLayout_5->addWidget(label_5);

        stopBitBox = new QComboBox(uartWidget);
        stopBitBox->addItem(QString());
        stopBitBox->addItem(QString());
        stopBitBox->addItem(QString());
        stopBitBox->addItem(QString());
        stopBitBox->setObjectName(QString::fromUtf8("stopBitBox"));
        stopBitBox->setMinimumSize(QSize(95, 0));
        stopBitBox->setMaximumSize(QSize(100, 32));
        stopBitBox->setStyleSheet(QString::fromUtf8("/* \347\273\237\344\270\200\346\211\200\346\234\211\350\276\223\345\205\245\346\241\206\357\274\210QLineEdit + QComboBox\357\274\211\347\232\204\346\240\267\345\274\217 */\n"
"QLineEdit, QComboBox {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e0e0e0;\n"
"    border-radius: 6px;\n"
"    padding: 6px 8px;\n"
"    font-size: 16px;\n"
"    color: #333333;\n"
"}\n"
"\n"
"/* QComboBox \344\270\213\346\213\211\347\256\255\345\244\264\345\214\272\345\237\237\346\240\267\345\274\217\357\274\210\351\201\277\345\205\215\351\242\235\345\244\226\345\207\270\350\265\267 + \346\227\240\350\255\246\345\221\212\357\274\211 */\n"
"QComboBox::drop-down {\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    width: 20px;\n"
"}\n"
"QComboBox::down-arrow {\n"
"    image: none; /* \346\270\205\347\251\272\347\256\255\345\244\264\357\274\214\346\227\240\350\265\204\346\272\220\345\212\240\350\275\275\350\255\246\345\221\212 */\n"
"    width: 12px;\n"
"    height: 12px;\n"
"}\n"
"\n"
"/* \350\201\232\347"
                        "\204\246\347\212\266\346\200\201\344\277\235\346\214\201\344\270\200\350\207\264 */\n"
"QLineEdit:focus, QComboBox:focus {\n"
"    border: 1px solid #40a9ff;\n"
"    outline: none;\n"
"}"));

        horizontalLayout_5->addWidget(stopBitBox);


        verticalLayout_2->addLayout(horizontalLayout_5);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(-1, 10, -1, -1);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer);

        pushButton = new QPushButton(uartWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setStyleSheet(QString::fromUtf8("/* QPushButton \345\237\272\347\241\200\346\240\267\345\274\217 - \346\240\270\345\277\203\346\232\227\351\273\221\351\243\216\346\240\274 */\n"
"QPushButton {\n"
"    background-color: #1a1a1a;  /* \346\267\261\351\273\221\350\211\262\350\203\214\346\231\257 */\n"
"    border-radius: 12px;         /* \345\234\206\350\247\222 */\n"
"    border: 2px solid #2d2d2d;  /* \347\273\237\344\270\200\350\276\271\346\241\206\345\256\275\345\272\246\344\270\2722px */\n"
"\n"
"    font-family: \"Microsoft YaHei\", sans-serif;\n"
"    font-size: 16px;            \n"
"    color: #ffffff;              \n"
"\n"
"    padding: 5px 10px;          \n"
"    min-width: 80px;            \n"
"    min-height: 28px;           \n"
"}\n"
"\n"
"/* \351\274\240\346\240\207\346\202\254\345\201\234 - \350\275\273\345\276\256\346\217\220\344\272\256 */\n"
"QPushButton:hover {\n"
"    background-color: #2a2a2a;\n"
"    border-color: #3d3d3d;\n"
"}\n"
"\n"
"/* \346\214\211\344\270\213\346\225\210\346\236\234 - \345\212\240\346\267\261\350\203\214"
                        "\346\231\257 */\n"
"QPushButton:pressed {\n"
"    background-color: #0a0a0a;\n"
"}\n"
"\n"
"/* \347\246\201\347\224\250\347\212\266\346\200\201 - \347\201\260\350\211\262\350\260\203 */\n"
"QPushButton:disabled {\n"
"    background-color: #2c2c2c;\n"
"    color: #666666;\n"
"}\n"
"\n"
"/* \350\277\236\346\216\245\346\210\220\345\212\237\357\274\210\346\226\255\345\274\200\347\212\266\346\200\201\357\274\211- \347\272\242\350\211\262\351\253\230\344\272\256\346\240\267\345\274\217 */\n"
"QPushButton[connected=\"true\"] {\n"
"    background-color: #ff0000;    /* \347\272\257\347\272\242\350\211\262\350\203\214\346\231\257 */\n"
"    border: 2px solid #ffffff;    /* \344\277\235\346\214\201\350\276\271\346\241\206\345\256\275\345\272\2462px\357\274\214\344\273\205\346\224\271\351\242\234\350\211\262 */\n"
"    color: #ffffff;               /* \347\231\275\350\211\262\346\226\207\345\255\227 */\n"
"}\n"
"QPushButton[connected=\"true\"]:hover {\n"
"    background-color: #ff3333;\n"
"}\n"
"QPushButton[connected=\"tr"
                        "ue\"]:pressed {\n"
"    background-color: #cc0000;\n"
"}"));

        horizontalLayout_6->addWidget(pushButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_6);

        verticalLayout->setStretch(0, 3);

        verticalLayout_2->addLayout(verticalLayout);

        verticalLayout_2->setStretch(0, 2);
        verticalLayout_2->setStretch(1, 2);
        verticalLayout_2->setStretch(2, 2);
        verticalLayout_2->setStretch(3, 2);
        verticalLayout_2->setStretch(4, 3);

        retranslateUi(uartWidget);

        QMetaObject::connectSlotsByName(uartWidget);
    } // setupUi

    void retranslateUi(QWidget *uartWidget)
    {
        uartWidget->setWindowTitle(QCoreApplication::translate("uartWidget", "Form", nullptr));
        label_2->setText(QCoreApplication::translate("uartWidget", "\346\263\242\347\211\271\347\216\207", nullptr));
        bandRateBox->setItemText(0, QCoreApplication::translate("uartWidget", "115200", nullptr));
        bandRateBox->setItemText(1, QCoreApplication::translate("uartWidget", "9600", nullptr));

        label_3->setText(QCoreApplication::translate("uartWidget", "\346\225\260\346\215\256\344\275\215", nullptr));
        dataBitBox->setItemText(0, QCoreApplication::translate("uartWidget", "8", nullptr));
        dataBitBox->setItemText(1, QCoreApplication::translate("uartWidget", "7", nullptr));
        dataBitBox->setItemText(2, QCoreApplication::translate("uartWidget", "6", nullptr));
        dataBitBox->setItemText(3, QCoreApplication::translate("uartWidget", "5", nullptr));

        label_4->setText(QCoreApplication::translate("uartWidget", "\346\240\241\351\252\214\344\275\215", nullptr));
        checkBitBox->setItemText(0, QCoreApplication::translate("uartWidget", "None", nullptr));
        checkBitBox->setItemText(1, QCoreApplication::translate("uartWidget", "Even", nullptr));
        checkBitBox->setItemText(2, QCoreApplication::translate("uartWidget", "Odd", nullptr));
        checkBitBox->setItemText(3, QCoreApplication::translate("uartWidget", "Mark", nullptr));
        checkBitBox->setItemText(4, QCoreApplication::translate("uartWidget", "Space", nullptr));

        label_5->setText(QCoreApplication::translate("uartWidget", "\345\201\234\346\255\242\344\275\215", nullptr));
        stopBitBox->setItemText(0, QCoreApplication::translate("uartWidget", "1", nullptr));
        stopBitBox->setItemText(1, QCoreApplication::translate("uartWidget", "1.5", nullptr));
        stopBitBox->setItemText(2, QCoreApplication::translate("uartWidget", "2", nullptr));
        stopBitBox->setItemText(3, QCoreApplication::translate("uartWidget", "0", nullptr));

        pushButton->setText(QCoreApplication::translate("uartWidget", "\351\200\211\346\213\251\344\270\262\345\217\243", nullptr));
    } // retranslateUi

};

namespace Ui {
    class uartWidget: public Ui_uartWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UARTWIDGET_H
