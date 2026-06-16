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
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_tcpWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QLabel *modeLabel;
    QSpacerItem *horizontalSpacer_2;
    QComboBox *modeCombo;
    QStackedWidget *stackedWidget;

    void setupUi(QWidget *tcpWidget)
    {
        if (tcpWidget->objectName().isEmpty())
            tcpWidget->setObjectName(QString::fromUtf8("tcpWidget"));
        tcpWidget->resize(220, 270);
        verticalLayout = new QVBoxLayout(tcpWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(5, 5, 5, 5);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, -1, 10, -1);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        modeLabel = new QLabel(tcpWidget);
        modeLabel->setObjectName(QString::fromUtf8("modeLabel"));
        modeLabel->setMinimumSize(QSize(35, 0));

        horizontalLayout->addWidget(modeLabel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        modeCombo = new QComboBox(tcpWidget);
        modeCombo->addItem(QString());
        modeCombo->addItem(QString());
        modeCombo->addItem(QString());
        modeCombo->setObjectName(QString::fromUtf8("modeCombo"));
        modeCombo->setMinimumSize(QSize(100, 0));
        modeCombo->setMaximumSize(QSize(160, 32));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        modeCombo->setFont(font);
        modeCombo->setStyleSheet(QString::fromUtf8("/* \347\273\237\344\270\200\346\211\200\346\234\211\350\276\223\345\205\245\346\241\206\357\274\210QLineEdit + QComboBox\357\274\211\347\232\204\346\240\267\345\274\217 */\n"
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
"    image: none;\n"
"    width: 12px;\n"
"    height: 12px;\n"
"}\n"
"\n"
"/* \350\201\232\347\204\246\347\212\266\346\200\201\344\277\235\346\214\201\344\270\200\350\207\264 */\n"
"QLineEdit:focus, QComboBox:focus {\n"
"    border: 1px solid "
                        "#40a9ff;\n"
"    outline: none;\n"
"}"));

        horizontalLayout->addWidget(modeCombo);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 3);
        horizontalLayout->setStretch(2, 1);
        horizontalLayout->setStretch(3, 10);

        verticalLayout->addLayout(horizontalLayout);

        stackedWidget = new QStackedWidget(tcpWidget);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));

        verticalLayout->addWidget(stackedWidget);


        retranslateUi(tcpWidget);

        QMetaObject::connectSlotsByName(tcpWidget);
    } // setupUi

    void retranslateUi(QWidget *tcpWidget)
    {
        modeLabel->setText(QCoreApplication::translate("tcpWidget", "\346\250\241\345\274\217", nullptr));
        modeCombo->setItemText(0, QCoreApplication::translate("tcpWidget", "TCP\345\256\242\346\210\267\347\253\257", nullptr));
        modeCombo->setItemText(1, QCoreApplication::translate("tcpWidget", "TCP\346\234\215\345\212\241\347\253\257", nullptr));
        modeCombo->setItemText(2, QCoreApplication::translate("tcpWidget", "UDP", nullptr));

        (void)tcpWidget;
    } // retranslateUi

};

namespace Ui {
    class tcpWidget: public Ui_tcpWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPWIDGET_H
