/********************************************************************************
** Form generated from reading UI file 'tcpwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPWIDGET_H
#define UI_TCPWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_tcpWidget
{
public:
    QVBoxLayout *verticalLayout_6;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_7;
    QComboBox *tcpModeBox;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *ipEdit1;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_4;
    QLineEdit *portEdit1;
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_5;
    QLineEdit *ipEdit2;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_6;
    QLineEdit *portEdit2;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *connectBtn;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *tcpWidget)
    {
        if (tcpWidget->objectName().isEmpty())
            tcpWidget->setObjectName(QString::fromUtf8("tcpWidget"));
        tcpWidget->resize(220, 270);
        verticalLayout_6 = new QVBoxLayout(tcpWidget);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, -1, 0, -1);
        label_7 = new QLabel(tcpWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy);

        horizontalLayout_5->addWidget(label_7);

        tcpModeBox = new QComboBox(tcpWidget);
        tcpModeBox->addItem(QString());
        tcpModeBox->addItem(QString());
        tcpModeBox->addItem(QString());
        tcpModeBox->setObjectName(QString::fromUtf8("tcpModeBox"));
        tcpModeBox->setMaximumSize(QSize(100, 32));
        tcpModeBox->setStyleSheet(QString::fromUtf8("/* \347\273\237\344\270\200\346\211\200\346\234\211\350\276\223\345\205\245\346\241\206\357\274\210QLineEdit + QComboBox\357\274\211\347\232\204\346\240\267\345\274\217 */\n"
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

        horizontalLayout_5->addWidget(tcpModeBox);

        horizontalLayout_5->setStretch(0, 1);
        horizontalLayout_5->setStretch(1, 1);

        verticalLayout_5->addLayout(horizontalLayout_5);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(tcpWidget);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(label);

        ipEdit1 = new QLineEdit(tcpWidget);
        ipEdit1->setObjectName(QString::fromUtf8("ipEdit1"));

        verticalLayout->addWidget(ipEdit1);


        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label_4 = new QLabel(tcpWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        sizePolicy1.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy1);

        verticalLayout_2->addWidget(label_4);

        portEdit1 = new QLineEdit(tcpWidget);
        portEdit1->setObjectName(QString::fromUtf8("portEdit1"));

        verticalLayout_2->addWidget(portEdit1);


        horizontalLayout->addLayout(verticalLayout_2);

        horizontalLayout->setStretch(0, 4);
        horizontalLayout->setStretch(1, 1);

        verticalLayout_5->addLayout(horizontalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, -1, -1, -1);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label_5 = new QLabel(tcpWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        sizePolicy1.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy1);

        verticalLayout_3->addWidget(label_5);

        ipEdit2 = new QLineEdit(tcpWidget);
        ipEdit2->setObjectName(QString::fromUtf8("ipEdit2"));

        verticalLayout_3->addWidget(ipEdit2);


        horizontalLayout_4->addLayout(verticalLayout_3);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        label_6 = new QLabel(tcpWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        sizePolicy1.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy1);

        verticalLayout_4->addWidget(label_6);

        portEdit2 = new QLineEdit(tcpWidget);
        portEdit2->setObjectName(QString::fromUtf8("portEdit2"));

        verticalLayout_4->addWidget(portEdit2);


        horizontalLayout_4->addLayout(verticalLayout_4);

        horizontalLayout_4->setStretch(0, 4);
        horizontalLayout_4->setStretch(1, 1);

        verticalLayout_5->addLayout(horizontalLayout_4);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 20, -1, -1);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        connectBtn = new QPushButton(tcpWidget);
        connectBtn->setObjectName(QString::fromUtf8("connectBtn"));
        connectBtn->setStyleSheet(QString::fromUtf8("/* QPushButton \345\237\272\347\241\200\346\240\267\345\274\217 - \346\240\270\345\277\203\346\232\227\351\273\221\351\243\216\346\240\274 */\n"
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

        horizontalLayout_2->addWidget(connectBtn);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_5->addLayout(horizontalLayout_2);

        verticalLayout_5->setStretch(0, 1);
        verticalLayout_5->setStretch(1, 1);
        verticalLayout_5->setStretch(2, 1);
        verticalLayout_5->setStretch(3, 2);

        verticalLayout_6->addLayout(verticalLayout_5);


        retranslateUi(tcpWidget);

        QMetaObject::connectSlotsByName(tcpWidget);
    } // setupUi

    void retranslateUi(QWidget *tcpWidget)
    {
        tcpWidget->setWindowTitle(QCoreApplication::translate("tcpWidget", "Form", nullptr));
        label_7->setText(QCoreApplication::translate("tcpWidget", "\351\200\211\346\213\251\346\250\241\345\274\217", nullptr));
        tcpModeBox->setItemText(0, QCoreApplication::translate("tcpWidget", "TCPClient", nullptr));
        tcpModeBox->setItemText(1, QCoreApplication::translate("tcpWidget", "TCPServer", nullptr));
        tcpModeBox->setItemText(2, QCoreApplication::translate("tcpWidget", "UDP", nullptr));

        label->setText(QCoreApplication::translate("tcpWidget", "\350\277\234\347\250\213\345\234\260\345\235\200", nullptr));
        ipEdit1->setText(QString());
        label_4->setText(QCoreApplication::translate("tcpWidget", "\347\253\257\345\217\243", nullptr));
        portEdit1->setText(QString());
        label_5->setText(QCoreApplication::translate("tcpWidget", "\346\234\254\345\234\260\345\234\260\345\235\200", nullptr));
        label_6->setText(QCoreApplication::translate("tcpWidget", "\347\253\257\345\217\243", nullptr));
        portEdit2->setText(QString());
        connectBtn->setText(QCoreApplication::translate("tcpWidget", "\350\277\236\346\216\245", nullptr));
    } // retranslateUi

};

namespace Ui {
    class tcpWidget: public Ui_tcpWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPWIDGET_H
