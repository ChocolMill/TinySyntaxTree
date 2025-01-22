/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QWidget *widget;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QTreeWidget *treeWidget;
    QPlainTextEdit *plainTextEdit;
    QPushButton *pushButton_8;
    QPlainTextEdit *plainTextEdit_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(932, 832);
        widget = new QWidget(Widget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(40, 10, 851, 821));
        pushButton_6 = new QPushButton(widget);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        pushButton_6->setGeometry(QRect(0, 740, 161, 40));
        QFont font;
        font.setPointSize(15);
        pushButton_6->setFont(font);
        pushButton_7 = new QPushButton(widget);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));
        pushButton_7->setGeometry(QRect(230, 740, 161, 41));
        pushButton_7->setFont(font);
        treeWidget = new QTreeWidget(widget);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setGeometry(QRect(420, 80, 431, 471));
        treeWidget->setHeaderHidden(true);
        plainTextEdit = new QPlainTextEdit(widget);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(0, 80, 391, 650));
        pushButton_8 = new QPushButton(widget);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        pushButton_8->setGeometry(QRect(570, 30, 161, 41));
        pushButton_8->setFont(font);
        plainTextEdit_2 = new QPlainTextEdit(widget);
        plainTextEdit_2->setObjectName(QString::fromUtf8("plainTextEdit_2"));
        plainTextEdit_2->setGeometry(QRect(420, 590, 431, 191));
        label_3 = new QLabel(widget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(420, 560, 111, 31));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font1.setPointSize(12);
        label_3->setFont(font1);
        label_4 = new QLabel(widget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(420, 40, 111, 31));
        label_4->setFont(font1);
        label = new QLabel(widget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 50, 131, 31));
        label->setFont(font1);

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "\345\256\236\351\252\214\344\270\211\357\274\232TINY\346\211\251\345\205\205\350\257\255\350\250\200\347\232\204\350\257\255\346\263\225\346\240\221\347\224\237\346\210\220_\350\202\226\347\277\224", nullptr));
        pushButton_6->setText(QApplication::translate("Widget", "\346\211\223\345\274\200\346\272\220\347\250\213\345\272\217", nullptr));
        pushButton_7->setText(QApplication::translate("Widget", "\344\277\235\345\255\230\346\272\220\347\250\213\345\272\217", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("Widget", "\346\240\221\345\275\242\347\273\223\346\236\204", nullptr));
        pushButton_8->setText(QApplication::translate("Widget", "\347\224\237\346\210\220\350\257\255\346\263\225\346\240\221", nullptr));
        label_3->setText(QApplication::translate("Widget", "\350\257\215\346\263\225\345\210\206\346\236\220", nullptr));
        label_4->setText(QApplication::translate("Widget", "\350\257\255\346\263\225\345\210\206\346\236\220", nullptr));
        label->setText(QApplication::translate("Widget", "TINY\346\272\220\344\273\243\347\240\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
