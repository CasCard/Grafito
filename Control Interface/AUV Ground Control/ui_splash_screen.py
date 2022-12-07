# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'splash_screendOVowB.ui'
##
## Created by: Qt User Interface Compiler version 5.15.2
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide2.QtCore import *
from PySide2.QtGui import *
from PySide2.QtWidgets import *


class Ui_SplashScreen(object):
    def setupUi(self, SplashScreen):
        if not SplashScreen.objectName():
            SplashScreen.setObjectName(u"SplashScreen")
        SplashScreen.resize(340, 380)
        self.centralwidget = QWidget(SplashScreen)
        self.centralwidget.setObjectName(u"centralwidget")
        self.circularProgressBarBase = QFrame(self.centralwidget)
        self.circularProgressBarBase.setObjectName(u"circularProgressBarBase")
        self.circularProgressBarBase.setGeometry(QRect(10, 10, 320, 320))
        self.circularProgressBarBase.setFrameShape(QFrame.NoFrame)
        self.circularProgressBarBase.setFrameShadow(QFrame.Raised)
        self.circularProgress = QFrame(self.circularProgressBarBase)
        self.circularProgress.setObjectName(u"circularProgress")
        self.circularProgress.setGeometry(QRect(10, 10, 300, 300))
        self.circularProgress.setStyleSheet(u"QFrame{\n"
" 	border-radius: 150px;\n"
"	background-color:qconicalgradient(cx:0.5, cy:0.5, angle:90, stop:0.749 rgba(255, 0, 127, 0), stop:0.750 rgba(85, 170, 255, 255));\n"
"}")
        self.circularProgress.setFrameShape(QFrame.NoFrame)
        self.circularProgress.setFrameShadow(QFrame.Raised)
        self.circularBg = QFrame(self.circularProgressBarBase)
        self.circularBg.setObjectName(u"circularBg")
        self.circularBg.setGeometry(QRect(10, 10, 300, 300))
        self.circularBg.setStyleSheet(u"QFrame{\n"
"border-radius:150px;\n"
"background-color:rgba(77,77,127,120);\n"
"}")
        self.circularBg.setFrameShape(QFrame.NoFrame)
        self.circularBg.setFrameShadow(QFrame.Raised)
        self.container = QFrame(self.circularProgressBarBase)
        self.container.setObjectName(u"container")
        self.container.setGeometry(QRect(25, 25, 270, 270))
        font = QFont()
        font.setFamily(u"Segoe UI Variable Display")
        font.setPointSize(12)
        self.container.setFont(font)
        self.container.setStyleSheet(u"QFrame{\n"
"	border-radius:135px;\n"
"	background-color:rgb(77,77,127);\n"
"}")
        self.container.setFrameShape(QFrame.StyledPanel)
        self.container.setFrameShadow(QFrame.Raised)
        self.labelTitle = QLabel(self.container)
        self.labelTitle.setObjectName(u"labelTitle")
        self.labelTitle.setGeometry(QRect(20, 30, 231, 71))
        font1 = QFont()
        font1.setFamily(u"Segoe UI")
        font1.setPointSize(14)
        self.labelTitle.setFont(font1)
        self.labelTitle.setStyleSheet(u"background-color:none;\n"
"color:#FFFFFF")
        self.labelTitle.setFrameShape(QFrame.NoFrame)
        self.labelTitle.setAlignment(Qt.AlignCenter)
        self.labelPercentage = QLabel(self.container)
        self.labelPercentage.setObjectName(u"labelPercentage")
        self.labelPercentage.setGeometry(QRect(20, 100, 231, 81))
        font2 = QFont()
        font2.setFamily(u"Segoe UI")
        font2.setPointSize(68)
        self.labelPercentage.setFont(font2)
        self.labelPercentage.setStyleSheet(u"background-color:none;\n"
"color:#FFFFFF")
        self.labelPercentage.setFrameShape(QFrame.NoFrame)
        self.labelPercentage.setMidLineWidth(0)
        self.labelPercentage.setTextFormat(Qt.PlainText)
        self.labelPercentage.setAlignment(Qt.AlignCenter)
        self.labelTitle_2 = QLabel(self.container)
        self.labelTitle_2.setObjectName(u"labelTitle_2")
        self.labelTitle_2.setGeometry(QRect(20, 180, 231, 71))
        self.labelTitle_2.setFont(font1)
        self.labelTitle_2.setLayoutDirection(Qt.LeftToRight)
        self.labelTitle_2.setStyleSheet(u"background-color:none;\n"
"color:#FFFFFF")
        self.labelTitle_2.setFrameShape(QFrame.NoFrame)
        self.labelTitle_2.setAlignment(Qt.AlignCenter)
        self.circularBg.raise_()
        self.circularProgress.raise_()
        self.container.raise_()
        SplashScreen.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(SplashScreen)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 340, 29))
        SplashScreen.setMenuBar(self.menubar)

        self.retranslateUi(SplashScreen)

        QMetaObject.connectSlotsByName(SplashScreen)
    # setupUi

    def retranslateUi(self, SplashScreen):
        SplashScreen.setWindowTitle(QCoreApplication.translate("SplashScreen", u"MainWindow", None))
        self.labelTitle.setText(QCoreApplication.translate("SplashScreen", u"<html><head/><body><p><span style=\" font-weight:600;\">AUV Varuna 2.0</span></p><p>NIT Calicut</p></body></html>", None))
        self.labelPercentage.setText(QCoreApplication.translate("SplashScreen", u"0%", None))
        self.labelTitle_2.setText(QCoreApplication.translate("SplashScreen", u"Loading ....", None))
    # retranslateUi

