/********************************************************************************
** Form generated from reading UI file 'sdrgui.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SDRGUI_H
#define UI_SDRGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_elektorSDR
{
public:
    QLCDNumber *lcd_Frequency;
    QLabel *systemindicator;
    QLCDNumber *lcd_fmRate;
    QComboBox *streamOutSelector;
    QLabel *timeDisplay;
    QPushButton *quitButton;
    QLCDNumber *lcd_OutputRate;
    QPushButton *fc_plus;
    QPushButton *fc_minus;
    QLabel *incrementingFlag;
    QPushButton *f_minus;
    QPushButton *f_plus;
    QSpinBox *fm_increment;
    QSpinBox *minimumSelect;
    QSpinBox *maximumSelect;
    QLCDNumber *lcd_inputRate;
    QSlider *squelchSlider;
    QPushButton *squelchButton;
    QPushButton *startButton;
    QComboBox *fmDecoder;
    QLabel *decoderDisplay;
    QLabel *radioTextBox;
    QLabel *stationLabelTextBox;
    QComboBox *fmMode;
    QComboBox *audioChannelSelect;
    QComboBox *fmDeemphasisSelector;
    QComboBox *fmRdsSelector;
    QLabel *rdsSyncLabel;
    QLabel *speechLabel;
    QLabel *pll_isLocked;
    QLabel *label_6;
    QLCDNumber *rdsAFDisplay;
    QLabel *label_11;
    QLCDNumber *rdsPiDisplay;
    QLCDNumber *dc_component;
    QPushButton *freqButton;
    QComboBox *fmLFcutoff;
    QPushButton *freqSave;

    void setupUi(QDialog *elektorSDR)
    {
        if (elektorSDR->objectName().isEmpty())
            elektorSDR->setObjectName(QStringLiteral("elektorSDR"));
        elektorSDR->resize(715, 206);
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Active, QPalette::Window, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush);
        elektorSDR->setPalette(palette);
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        elektorSDR->setFont(font);
        lcd_Frequency = new QLCDNumber(elektorSDR);
        lcd_Frequency->setObjectName(QStringLiteral("lcd_Frequency"));
        lcd_Frequency->setGeometry(QRect(400, 160, 191, 41));
        QPalette palette1;
        QBrush brush1(QColor(255, 0, 0, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(255, 255, 0, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Base, brush2);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        lcd_Frequency->setPalette(palette1);
        QFont font1;
        font1.setPointSize(17);
        font1.setBold(true);
        font1.setItalic(true);
        font1.setWeight(75);
        lcd_Frequency->setFont(font1);
        lcd_Frequency->setAutoFillBackground(false);
        lcd_Frequency->setFrameShape(QFrame::NoFrame);
        lcd_Frequency->setFrameShadow(QFrame::Raised);
        lcd_Frequency->setLineWidth(2);
        lcd_Frequency->setDigitCount(10);
        lcd_Frequency->setSegmentStyle(QLCDNumber::Flat);
        systemindicator = new QLabel(elektorSDR);
        systemindicator->setObjectName(QStringLiteral("systemindicator"));
        systemindicator->setGeometry(QRect(180, 70, 161, 31));
        QFont font2;
        font2.setPointSize(10);
        systemindicator->setFont(font2);
        systemindicator->setFrameShape(QFrame::Panel);
        systemindicator->setFrameShadow(QFrame::Raised);
        systemindicator->setLineWidth(2);
        lcd_fmRate = new QLCDNumber(elektorSDR);
        lcd_fmRate->setObjectName(QStringLiteral("lcd_fmRate"));
        lcd_fmRate->setGeometry(QRect(120, 160, 131, 31));
        QPalette palette2;
        palette2.setBrush(QPalette::Active, QPalette::Base, brush);
        palette2.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette2.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        palette2.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette2.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        lcd_fmRate->setPalette(palette2);
        lcd_fmRate->setAutoFillBackground(false);
        lcd_fmRate->setFrameShape(QFrame::NoFrame);
        lcd_fmRate->setFrameShadow(QFrame::Raised);
        lcd_fmRate->setLineWidth(2);
        lcd_fmRate->setDigitCount(6);
        lcd_fmRate->setSegmentStyle(QLCDNumber::Flat);
        streamOutSelector = new QComboBox(elektorSDR);
        streamOutSelector->setObjectName(QStringLiteral("streamOutSelector"));
        streamOutSelector->setGeometry(QRect(200, 0, 211, 31));
        QFont font3;
        font3.setPointSize(8);
        font3.setBold(false);
        font3.setItalic(true);
        font3.setWeight(50);
        streamOutSelector->setFont(font3);
        timeDisplay = new QLabel(elektorSDR);
        timeDisplay->setObjectName(QStringLiteral("timeDisplay"));
        timeDisplay->setGeometry(QRect(0, 70, 181, 31));
        timeDisplay->setFrameShape(QFrame::Panel);
        timeDisplay->setFrameShadow(QFrame::Raised);
        timeDisplay->setLineWidth(2);
        quitButton = new QPushButton(elektorSDR);
        quitButton->setObjectName(QStringLiteral("quitButton"));
        quitButton->setGeometry(QRect(120, 0, 81, 31));
        QPalette palette3;
        palette3.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette3.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette3.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        quitButton->setPalette(palette3);
        QFont font4;
        font4.setBold(true);
        font4.setWeight(75);
        quitButton->setFont(font4);
        quitButton->setAutoFillBackground(true);
        lcd_OutputRate = new QLCDNumber(elektorSDR);
        lcd_OutputRate->setObjectName(QStringLiteral("lcd_OutputRate"));
        lcd_OutputRate->setGeometry(QRect(0, 160, 101, 31));
        QPalette palette4;
        palette4.setBrush(QPalette::Active, QPalette::Base, brush);
        palette4.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette4.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        palette4.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette4.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        lcd_OutputRate->setPalette(palette4);
        lcd_OutputRate->setAutoFillBackground(false);
        lcd_OutputRate->setFrameShape(QFrame::NoFrame);
        lcd_OutputRate->setLineWidth(2);
        lcd_OutputRate->setDigitCount(6);
        lcd_OutputRate->setSegmentStyle(QLCDNumber::Flat);
        fc_plus = new QPushButton(elektorSDR);
        fc_plus->setObjectName(QStringLiteral("fc_plus"));
        fc_plus->setGeometry(QRect(570, 30, 71, 41));
        fc_minus = new QPushButton(elektorSDR);
        fc_minus->setObjectName(QStringLiteral("fc_minus"));
        fc_minus->setGeometry(QRect(450, 30, 71, 41));
        incrementingFlag = new QLabel(elektorSDR);
        incrementingFlag->setObjectName(QStringLiteral("incrementingFlag"));
        incrementingFlag->setGeometry(QRect(520, 30, 51, 41));
        incrementingFlag->setFrameShape(QFrame::Box);
        incrementingFlag->setLineWidth(2);
        f_minus = new QPushButton(elektorSDR);
        f_minus->setObjectName(QStringLiteral("f_minus"));
        f_minus->setGeometry(QRect(380, 30, 71, 41));
        f_plus = new QPushButton(elektorSDR);
        f_plus->setObjectName(QStringLiteral("f_plus"));
        f_plus->setGeometry(QRect(640, 30, 71, 41));
        fm_increment = new QSpinBox(elektorSDR);
        fm_increment->setObjectName(QStringLiteral("fm_increment"));
        fm_increment->setGeometry(QRect(240, 30, 71, 41));
        fm_increment->setMaximum(1000);
        fm_increment->setSingleStep(100);
        fm_increment->setValue(100);
        minimumSelect = new QSpinBox(elektorSDR);
        minimumSelect->setObjectName(QStringLiteral("minimumSelect"));
        minimumSelect->setGeometry(QRect(170, 30, 71, 41));
        QFont font5;
        font5.setPointSize(9);
        minimumSelect->setFont(font5);
        minimumSelect->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
        minimumSelect->setMaximum(1700);
        minimumSelect->setValue(86);
        maximumSelect = new QSpinBox(elektorSDR);
        maximumSelect->setObjectName(QStringLiteral("maximumSelect"));
        maximumSelect->setGeometry(QRect(310, 30, 71, 41));
        maximumSelect->setFont(font5);
        maximumSelect->setMaximum(1700);
        maximumSelect->setValue(110);
        lcd_inputRate = new QLCDNumber(elektorSDR);
        lcd_inputRate->setObjectName(QStringLiteral("lcd_inputRate"));
        lcd_inputRate->setGeometry(QRect(250, 160, 131, 31));
        QPalette palette5;
        palette5.setBrush(QPalette::Active, QPalette::Base, brush);
        palette5.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette5.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette5.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        palette5.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette5.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        lcd_inputRate->setPalette(palette5);
        lcd_inputRate->setAutoFillBackground(false);
        lcd_inputRate->setFrameShape(QFrame::NoFrame);
        lcd_inputRate->setLineWidth(2);
        lcd_inputRate->setDigitCount(7);
        lcd_inputRate->setSegmentStyle(QLCDNumber::Flat);
        squelchSlider = new QSlider(elektorSDR);
        squelchSlider->setObjectName(QStringLiteral("squelchSlider"));
        squelchSlider->setGeometry(QRect(340, 80, 281, 20));
        squelchSlider->setSliderPosition(50);
        squelchSlider->setOrientation(Qt::Horizontal);
        squelchButton = new QPushButton(elektorSDR);
        squelchButton->setObjectName(QStringLiteral("squelchButton"));
        squelchButton->setGeometry(QRect(620, 70, 91, 31));
        startButton = new QPushButton(elektorSDR);
        startButton->setObjectName(QStringLiteral("startButton"));
        startButton->setGeometry(QRect(0, 0, 121, 31));
        startButton->setFont(font4);
        startButton->setAutoFillBackground(true);
        fmDecoder = new QComboBox(elektorSDR);
        fmDecoder->setObjectName(QStringLiteral("fmDecoder"));
        fmDecoder->setGeometry(QRect(470, 0, 141, 31));
        decoderDisplay = new QLabel(elektorSDR);
        decoderDisplay->setObjectName(QStringLiteral("decoderDisplay"));
        decoderDisplay->setGeometry(QRect(610, 0, 101, 31));
        decoderDisplay->setFont(font5);
        decoderDisplay->setFrameShape(QFrame::Panel);
        decoderDisplay->setFrameShadow(QFrame::Raised);
        decoderDisplay->setLineWidth(2);
        radioTextBox = new QLabel(elektorSDR);
        radioTextBox->setObjectName(QStringLiteral("radioTextBox"));
        radioTextBox->setGeometry(QRect(0, 130, 521, 31));
        radioTextBox->setFrameShape(QFrame::Panel);
        radioTextBox->setFrameShadow(QFrame::Raised);
        stationLabelTextBox = new QLabel(elektorSDR);
        stationLabelTextBox->setObjectName(QStringLiteral("stationLabelTextBox"));
        stationLabelTextBox->setGeometry(QRect(0, 100, 171, 31));
        stationLabelTextBox->setFrameShape(QFrame::Panel);
        stationLabelTextBox->setFrameShadow(QFrame::Raised);
        stationLabelTextBox->setLineWidth(2);
        fmMode = new QComboBox(elektorSDR);
        fmMode->setObjectName(QStringLiteral("fmMode"));
        fmMode->setGeometry(QRect(390, 100, 91, 31));
        audioChannelSelect = new QComboBox(elektorSDR);
        audioChannelSelect->setObjectName(QStringLiteral("audioChannelSelect"));
        audioChannelSelect->setGeometry(QRect(480, 100, 91, 31));
        fmDeemphasisSelector = new QComboBox(elektorSDR);
        fmDeemphasisSelector->setObjectName(QStringLiteral("fmDeemphasisSelector"));
        fmDeemphasisSelector->setGeometry(QRect(570, 100, 61, 31));
        fmRdsSelector = new QComboBox(elektorSDR);
        fmRdsSelector->setObjectName(QStringLiteral("fmRdsSelector"));
        fmRdsSelector->setGeometry(QRect(170, 100, 71, 31));
        rdsSyncLabel = new QLabel(elektorSDR);
        rdsSyncLabel->setObjectName(QStringLiteral("rdsSyncLabel"));
        rdsSyncLabel->setGeometry(QRect(240, 100, 41, 31));
        rdsSyncLabel->setFrameShape(QFrame::Panel);
        rdsSyncLabel->setFrameShadow(QFrame::Raised);
        rdsSyncLabel->setLineWidth(2);
        speechLabel = new QLabel(elektorSDR);
        speechLabel->setObjectName(QStringLiteral("speechLabel"));
        speechLabel->setGeometry(QRect(280, 100, 61, 31));
        speechLabel->setFrameShape(QFrame::Panel);
        speechLabel->setFrameShadow(QFrame::Raised);
        speechLabel->setLineWidth(2);
        pll_isLocked = new QLabel(elektorSDR);
        pll_isLocked->setObjectName(QStringLiteral("pll_isLocked"));
        pll_isLocked->setGeometry(QRect(340, 100, 51, 31));
        pll_isLocked->setFrameShape(QFrame::Panel);
        pll_isLocked->setFrameShadow(QFrame::Raised);
        pll_isLocked->setLineWidth(2);
        label_6 = new QLabel(elektorSDR);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(610, 140, 51, 21));
        rdsAFDisplay = new QLCDNumber(elektorSDR);
        rdsAFDisplay->setObjectName(QStringLiteral("rdsAFDisplay"));
        rdsAFDisplay->setGeometry(QRect(640, 170, 64, 23));
        QPalette palette6;
        palette6.setBrush(QPalette::Active, QPalette::Base, brush);
        palette6.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette6.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette6.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        palette6.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette6.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        rdsAFDisplay->setPalette(palette6);
        rdsAFDisplay->setAutoFillBackground(false);
        rdsAFDisplay->setFrameShape(QFrame::NoFrame);
        rdsAFDisplay->setSegmentStyle(QLCDNumber::Flat);
        label_11 = new QLabel(elektorSDR);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(610, 170, 41, 21));
        rdsPiDisplay = new QLCDNumber(elektorSDR);
        rdsPiDisplay->setObjectName(QStringLiteral("rdsPiDisplay"));
        rdsPiDisplay->setGeometry(QRect(650, 140, 51, 21));
        QPalette palette7;
        palette7.setBrush(QPalette::Active, QPalette::Base, brush);
        palette7.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette7.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette7.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        palette7.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette7.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        rdsPiDisplay->setPalette(palette7);
        rdsPiDisplay->setAutoFillBackground(false);
        rdsPiDisplay->setFrameShape(QFrame::NoFrame);
        rdsPiDisplay->setMode(QLCDNumber::Hex);
        rdsPiDisplay->setSegmentStyle(QLCDNumber::Flat);
        dc_component = new QLCDNumber(elektorSDR);
        dc_component->setObjectName(QStringLiteral("dc_component"));
        dc_component->setGeometry(QRect(410, 0, 61, 31));
        QPalette palette8;
        palette8.setBrush(QPalette::Active, QPalette::Base, brush);
        palette8.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette8.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette8.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        palette8.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette8.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        dc_component->setPalette(palette8);
        dc_component->setAutoFillBackground(false);
        dc_component->setFrameShape(QFrame::NoFrame);
        dc_component->setSegmentStyle(QLCDNumber::Flat);
        freqButton = new QPushButton(elektorSDR);
        freqButton->setObjectName(QStringLiteral("freqButton"));
        freqButton->setGeometry(QRect(0, 30, 121, 41));
        fmLFcutoff = new QComboBox(elektorSDR);
        fmLFcutoff->setObjectName(QStringLiteral("fmLFcutoff"));
        fmLFcutoff->setGeometry(QRect(630, 100, 81, 31));
        freqSave = new QPushButton(elektorSDR);
        freqSave->setObjectName(QStringLiteral("freqSave"));
        freqSave->setGeometry(QRect(120, 30, 51, 41));

        retranslateUi(elektorSDR);

        QMetaObject::connectSlotsByName(elektorSDR);
    } // setupUi

    void retranslateUi(QDialog *elektorSDR)
    {
        elektorSDR->setWindowTitle(QApplication::translate("elektorSDR", "fm-receiver", 0));
        elektorSDR->setWindowIconText(QApplication::translate("elektorSDR", "QUIT", 0));
        systemindicator->setText(QApplication::translate("elektorSDR", "JFF-ESDR V2.0 Portaudio", 0));
        streamOutSelector->clear();
        streamOutSelector->insertItems(0, QStringList()
         << QApplication::translate("elektorSDR", "select output", 0)
        );
        timeDisplay->setText(QApplication::translate("elektorSDR", "TextLabel", 0));
        quitButton->setText(QApplication::translate("elektorSDR", "QUIT", 0));
        fc_plus->setText(QApplication::translate("elektorSDR", "fc+", 0));
        fc_minus->setText(QApplication::translate("elektorSDR", "fc-", 0));
        incrementingFlag->setText(QString());
        f_minus->setText(QApplication::translate("elektorSDR", "f-", 0));
        f_plus->setText(QApplication::translate("elektorSDR", "f+", 0));
        squelchButton->setText(QApplication::translate("elektorSDR", "squelchOff", 0));
        startButton->setText(QApplication::translate("elektorSDR", "START", 0));
        fmDecoder->clear();
        fmDecoder->insertItems(0, QStringList()
         << QApplication::translate("elektorSDR", "fm4decoder", 0)
         << QApplication::translate("elektorSDR", "fm1decoder", 0)
         << QApplication::translate("elektorSDR", "fm2decoder", 0)
         << QApplication::translate("elektorSDR", "fm3decoder", 0)
         << QApplication::translate("elektorSDR", "fm5decoder", 0)
        );
        decoderDisplay->setText(QString());
        radioTextBox->setText(QApplication::translate("elektorSDR", "TextLabel", 0));
        stationLabelTextBox->setText(QApplication::translate("elektorSDR", "TextLabel", 0));
        fmMode->clear();
        fmMode->insertItems(0, QStringList()
         << QApplication::translate("elektorSDR", "stereo", 0)
         << QApplication::translate("elektorSDR", "mono", 0)
        );
        audioChannelSelect->clear();
        audioChannelSelect->insertItems(0, QStringList()
         << QApplication::translate("elektorSDR", "stereo", 0)
         << QApplication::translate("elektorSDR", "Left", 0)
         << QApplication::translate("elektorSDR", "Right", 0)
         << QApplication::translate("elektorSDR", "Left+Right", 0)
         << QApplication::translate("elektorSDR", "Left-Right", 0)
        );
        fmDeemphasisSelector->clear();
        fmDeemphasisSelector->insertItems(0, QStringList()
         << QApplication::translate("elektorSDR", "50", 0)
         << QApplication::translate("elektorSDR", "none", 0)
         << QApplication::translate("elektorSDR", "75", 0)
        );
        fmRdsSelector->clear();
        fmRdsSelector->insertItems(0, QStringList()
         << QApplication::translate("elektorSDR", "no rds", 0)
         << QApplication::translate("elektorSDR", "rds on", 0)
        );
        rdsSyncLabel->setText(QString());
        speechLabel->setText(QString());
        pll_isLocked->setText(QApplication::translate("elektorSDR", "pilot", 0));
        label_6->setText(QApplication::translate("elektorSDR", "PiCode", 0));
        label_11->setText(QApplication::translate("elektorSDR", "AF", 0));
        freqButton->setText(QApplication::translate("elektorSDR", "Freq", 0));
        fmLFcutoff->clear();
        fmLFcutoff->insertItems(0, QStringList()
         << QApplication::translate("elektorSDR", "12000", 0)
         << QApplication::translate("elektorSDR", "9000", 0)
         << QApplication::translate("elektorSDR", "15000", 0)
        );
        freqSave->setText(QApplication::translate("elektorSDR", "FS", 0));
    } // retranslateUi

};

namespace Ui {
    class elektorSDR: public Ui_elektorSDR {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SDRGUI_H
