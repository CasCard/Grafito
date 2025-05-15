from __future__ import annotations
import sys
import platform
import os
import random
import time

from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import (QCoreApplication, QPropertyAnimation, QDate, QDateTime,
                          QMetaObject, QObject, QPoint, QRect, QSize, QTime, QUrl, Qt, QEvent)
from PyQt5.QtGui import (QBrush, QColor, QConicalGradient, QCursor, QFont, QFontDatabase,
                         QIcon, QKeySequence, QLinearGradient, QPalette, QPainter, QPixmap, QRadialGradient)
from PyQt5.QtWidgets import *
from PyQt5 import uic
import psutil
from pyqtgraph import PlotWidget
import pyqtgraph as pg
from pathlib import Path
import numpy as np
from collections import deque

from typing import *
import sys
import os
from matplotlib.backends.qt_compat import QtCore, QtWidgets
# from PyQt5 import QtWidgets, QtCore
from matplotlib.backends.backend_qt5agg import FigureCanvas
# from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
import matplotlib as mpl
import matplotlib.figure as mpl_fig
import matplotlib.animation as anim
import numpy as np
import pandas as pd

from PyQt5 import QtGui
from PyQt5.QtWidgets import QWidget, QApplication, QLabel, QVBoxLayout
from PyQt5.QtGui import QPixmap
import sys
import cv2
from PyQt5.QtCore import pyqtSignal, pyqtSlot, Qt, QThread

from PyQt5.QtWidgets import  QWidget, QLabel, QApplication
from PyQt5.QtCore import QThread, Qt, pyqtSignal, pyqtSlot
from PyQt5.QtGui import QImage, QPixmap



# GLOBALS
counter = 0
jumper = 10
timer = 0

# uncomment to use the webcam
cap = cv2.VideoCapture(0)

# read the data / load the data stream

class Thread(QThread):

    changePixmap1 = pyqtSignal(QImage)


    def run(self):
        # for raw footage , comment when using camera
        # cap = cv2.VideoCapture(r'C:\Users\abel_\OneDrive\Desktop\RIG\AUV Ground Control\redrod2.mp4')

        while cap.isOpened():
            # ret2, frame2 = cap2.read()
            ret, frame = cap.read()
            if ret:
                # https://stackoverflow.com/a/55468544/6622587
                rgbImage = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                # create a gray scale
                # gray = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                # h_g , w_g , d_g = gray.shape
                h, w, ch = rgbImage.shape
                bytesPerLine = ch * w
                # bytesPerLine_g = d_g * w_g
                # qt formqt for gray
                # qt_image = QImage(gray.data, w_g, h_g, bytesPerLine_g, QImage.Format_Grayscale8)
                convertToQtFormat = QImage(rgbImage.data, w, h, bytesPerLine, QImage.Format_RGB888)
                p = convertToQtFormat.scaled(640, 480, Qt.KeepAspectRatio)
                # p2 = qt_image.scaled(640, 480, Qt.KeepAspectRatio)
                self.changePixmap1.emit(p)
                self.msleep(1000 // 30)



class Thread2(QThread):

    changePixmap2 = pyqtSignal(QImage)

    def run(self):
        # add video processing here comment when using the camera
        # cap = cv2.VideoCapture(r'C:\Users\abel_\OneDrive\Desktop\RIG\AUV Ground Control\redrod2.mp4')

        while cap.isOpened():
            # ret2, frame2 = cap2.read()
            ret, frame = cap.read()
            if ret:
                # https://stackoverflow.com/a/55468544/6622587
                gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
                # appy a gray scale


                # gray = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                h_g , w_g  = gray.shape
                # h, w= gray.shape
                # bytesPerLine = 1 * w
                bytesPerLine_g = 1 * w_g
                # qt formqt for gray
                qt_image = QImage(gray.data, w_g, h_g, bytesPerLine_g, QImage.Format_Grayscale8)
                # convertToQtFormat = QImage(rgbImage.data, w, h, bytesPerLine, QImage.Format_RGB888)
                # p = convertToQtFormat.scaled(640, 480, Qt.KeepAspectRatio)
                p2 = qt_image.scaled(640, 480, Qt.KeepAspectRatio)
                self.changePixmap2.emit(p2)
                self.msleep(1000 // 30)




class MainWindow(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        self.ui = uic.loadUi("ground_control.ui", self)
        # 2. Place the matplotlib figure
        self.setWindowTitle("AUV Varuna Ground Control")
        # self.frm = QtWidgets.QFrame(self)
        # self.frm.setStyleSheet("QWidget { background-color: #eeeeec; }")
        # self.lyt = QtWidgets.QVBoxLayout()
        # self.frm.setLayout(self.lyt)
        # self.setCentralWidget(self.frm)
        image = cv2.imread('test.png')

        # 2. Place the matplotlib figure
        self.myFig = MyFigureCanvas(x_len=200, y_range=[0, 100], interval=20)
        # add the figue to the layout
        self.ui.verticalLayout.addWidget(self.myFig)
        # display image using opencv in box layout called raw_image






        # self.data = pd.read_csv('data.csv', header=None)

        # self.setWindowFlags(QtCore.Qt.FramelessWindowHint)  # Remove title bar
        # Set background to transparent
        self.setAttribute(QtCore.Qt.WA_TranslucentBackground)

        # params
        self.batteryLevel = 0
        self.depthLevel = 0
        self.cameraStatus = "OFF"
        self.cpuTemp = 0

        self.velocity_v = 0
        self.acceleration_v = 0
        self.distance_v = 0

        self.prop1_v = 0
        self.prop2_v = 0
        self.prop3_v = 0
        self.prop4_v = 0
        self.prop5_v = 0
        self.prop6_v = 0

        self.roll_v = 0
        self.pitch_v = 0
        self.yaw_v = 0

        self.flow1_v = 0
        self.flow2_v = 0

        self.dragValue = 0
        self.timeValue = 0

        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update_battery)
        self.timer.timeout.connect(self.update_depth)
        self.timer.timeout.connect(self.update_cpu)
        self.timer.timeout.connect(self.update_velocity)
        self.timer.timeout.connect(self.update_acceleration)
        self.timer.timeout.connect(self.update_distance)

        self.timer.timeout.connect(self.update_prop1)
        self.timer.timeout.connect(self.update_prop2)
        self.timer.timeout.connect(self.update_prop3)
        self.timer.timeout.connect(self.update_prop4)
        self.timer.timeout.connect(self.update_prop5)
        self.timer.timeout.connect(self.update_prop6)

        self.timer.timeout.connect(self.update_roll)
        self.timer.timeout.connect(self.update_pitch)
        self.timer.timeout.connect(self.update_yaw)

        self.timer.timeout.connect(self.update_flow1)
        self.timer.timeout.connect(self.update_flow2)
        self.timer.timeout.connect(self.update_camera)

        self.timer.timeout.connect(self.update_drag)

        self.timer.timeout.connect(self.update_time)

        # add image to the layout

        th = Thread(self)
        th.changePixmap1.connect(self.setImage)
        th.start()
        th2 = Thread2(self)
        th2.changePixmap2.connect(self.setImage2)
        th2.start()


        self.timer.start(500)

    @pyqtSlot(QImage)
    def setImage(self, image):
        self.ui.rawvideo.setPixmap(QPixmap.fromImage(image))
        # self.ui.provideo.setPixmap(QPixmap.fromImage(image))

    @pyqtSlot(QImage)
    def setImage2(self, image):
        self.ui.provideo.setPixmap(QPixmap.fromImage(image))

    # @pyqtSlot(QImage)
    # def setImage(self, image):


    def update_battery(self):
        self.batteryLevel = random.randint(0, 100)
        print(self.batteryLevel)

        self.ui.battery_level.setText(
            f'<p><span style=" font-size:18pt;">Battery : {self.batteryLevel} V</span></p>'
        )

    def update_depth(self):
        self.depthLevel = random.randint(0, 100)

        self.ui.depth.setText(
            f'<p><span style=" font-size:18pt;">Depth : {self.depthLevel} m</span></p>'
        )

    def update_cpu(self):
        self.cpuTemp = random.randint(0, 100)

        self.ui.cpu_temp.setText(
            f'<p><span style=" font-size:18pt;">CPU Temp : {self.cpuTemp} C</span></p>'
        )

    def update_camera(self):
        self.cameraStatus = random.choice(["ON", "OFF"])
        self.ui.camera.setText(
            f'<p><span style=" font-size:18pt;">Camera : {self.cameraStatus}</span></p>'
        )

    def update_velocity(self):
        self.velocity_v = random.randint(0, 100)
        self.ui.velocity.setText(
            f'<p><span style=" font-size:18pt;">Velocity : {self.velocity_v} m/s</span></p>'
        )

    def update_acceleration(self):
        self.acceleration_v = random.randint(0, 100)
        self.ui.acceleration.setText(
            f'<p><span style=" font-size:18pt;">Acceleration : {self.acceleration_v} m/s2</span></p>'
        )

    def update_distance(self):
        self.distance_v = random.randint(0, 100)
        self.ui.distance.setText(
            f'<p><span style=" font-size:18pt;">Distance : {self.distance_v} m</span></p>'
        )

    def update_prop1(self):
        self.prop1_v = random.randint(0, 100)
        self.ui.prop1.setText(
            f'<p><span style=" font-size:18pt;">Prop 1 : {self.prop1_v} </span></p>'
        )

    def update_prop2(self):
        self.prop2_v = random.randint(0, 100)
        self.ui.prop2.setText(
            f'<p><span style=" font-size:18pt;">Prop 2 : {self.prop2_v} </span></p>'
        )

    def update_prop3(self):
        self.prop3_v = random.randint(0, 100)
        self.ui.prop3.setText(
            f'<p><span style=" font-size:18pt;">Prop 3 : {self.prop3_v} </span></p>'
        )

    def update_prop4(self):
        self.prop4_v = random.randint(0, 100)
        self.ui.prop4.setText(
            f'<p><span style=" font-size:18pt;">Prop 4 : {self.prop4_v} </span></p>'
        )

    def update_prop5(self):
        self.prop5_v = random.randint(0, 100)
        self.ui.prop5.setText(
            f'<p><span style=" font-size:18pt;">Prop 5 : {self.prop5_v} </span></p>'
        )

    def update_prop6(self):
        self.prop6_v = random.randint(0, 100)
        self.ui.prop6.setText(
            f'<p><span style=" font-size:18pt;">Prop 6 : {self.prop6_v} </span></p>'
        )

    def update_roll(self):
        self.roll_v = random.randint(0, 100)
        self.ui.roll.setText(
            f'<p><span style=" font-size:18pt;">Roll : {self.roll_v}</span></p>'
        )

    def update_pitch(self):
        self.pitch_v = random.randint(0, 100)
        self.ui.pitch.setText(
            f'<p><span style=" font-size:18pt;">Pitch : {self.pitch_v}</span></p>'
        )

    def update_yaw(self):
        self.yaw_v = random.randint(0, 100)
        self.ui.yaw.setText(
            f'<p><span style=" font-size:18pt;">Yaw : {self.yaw_v}</span></p>'
        )

    def update_flow1(self):
        self.flow1_v = random.randint(0, 100)
        self.ui.flow1.setText(
            f'<p><span style=" font-size:18pt;">Flow 1 : {self.flow1_v}</span></p>'
        )

    def update_flow2(self):
        self.flow2_v = random.randint(0, 100)
        self.ui.flow2.setText(
            f'<p><span style=" font-size:18pt;">Flow 2 : {self.flow2_v}</span></p>'
        )

    def update_drag(self):
        self.dragValue = random.randint(0, 100)
        self.ui.drag.setText(
            f'<p><span style=" font-size:18pt;">Drag : {self.dragValue} N</span></p>'
        )

    def update_time(self):
        self.timeValue = time.strftime("%H:%M:%S", time.gmtime(random.uniform(0, 86400)))
        self.ui.time.setText(
            f'<p><span style=" font-size:18pt;">Time : {self.timeValue} s</span></p>'
        )


    def get(self, which):
        return self.data[which][random.randint(0, 998)]


class MyFigureCanvas(FigureCanvas, anim.FuncAnimation):
    '''
    This is the FigureCanvas in which the live plot is drawn.

    '''

    def __init__(self, x_len: int, y_range: List, interval: int) -> None:
        '''
        :param x_len:       The nr of data points shown in one plot.
        :param y_range:     Range on y-axis.
        :param interval:    Get a new datapoint every .. milliseconds.

        '''
        FigureCanvas.__init__(self, mpl_fig.Figure())
        # Range settings
        self._x_len_ = x_len
        self._y_range_ = y_range

        # Store two lists _x_ and _y_
        x1 = list(range(0, x_len))
        y1 = [0] * x_len

        x2 = list(range(0, x_len))
        y2 = [0] * x_len

        x3 = list(range(0, x_len))
        y3 = [0] * x_len

        # add x axis label as time

        # Store a figure and ax
        self._ax_ = self.figure.subplots()
        self._ax_.set_xlabel('Time')
        self._ax_.set_ylabel('RPY')
        self._ax_.set_ylim(ymin=self._y_range_[0], ymax=self._y_range_[1])
        self._line_1, = self._ax_.plot(x1, y1)
        self._line_2, = self._ax_.plot(x2, y2)
        self._line_3, = self._ax_.plot(x3, y3)

        # Call superclass constructors
        anim.FuncAnimation.__init__(self, self.figure, self._update_canvas_, fargs=(y1, y2, y3,), interval=interval,
                                    blit=True)
        return

    def _update_canvas_(self, i, y1, y2, y3) -> None:
        '''
        This function gets called regularly by the timer.

        '''
        y1.append(round(get_next_datapoint()[0], 2))  # Add new datapoint
        y1 = y1[-self._x_len_:]  # Truncate list _y_
        self._line_1.set_ydata(y1)

        y2.append(round(get_next_datapoint()[1], 2))  # Add new datapoint
        y2 = y2[-self._x_len_:]  # Truncate list _y_
        self._line_2.set_ydata(y2)

        y3.append(round(get_next_datapoint()[2], 2))  # Add new datapoint
        y3 = y3[-self._x_len_:]  # Truncate list _y_
        self._line_3.set_ydata(y3)

        return self._line_1, self._line_2, self._line_3


# Data source
# ------------
n = np.linspace(0, 499, 500)
d = 50 + 25 * (np.sin(n / 8.3)) + 10 * (np.sin(n / 7.5)) - 5 * (np.sin(n / 1.5))
imu_data = (d, d * 1.25, d * 1.5)
i = 0


def get_next_datapoint():
    global i
    i += 1
    if i > 499:
        i = 0
    return d[i], d[i] + 10, d[i] - 15


# ==> SPLASHSCREEN WINDOW
class SplashScreen(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        # self.ui = Ui_SplashScreen()
        # self.ui.setupUi(self)
        self.ui = uic.loadUi("splash_screen.ui", self)
        # ==> SET INITIAL PROGRESS BAR TO (0) ZERO
        self.progressBarValue(0)

        # ==> REMOVE STANDARD TITLE BAR
        self.setWindowFlags(QtCore.Qt.FramelessWindowHint)  # Remove title bar
        # Set background to transparent
        self.setAttribute(QtCore.Qt.WA_TranslucentBackground)

        # ==> APPLY DROP SHADOW EFFECT
        self.shadow = QGraphicsDropShadowEffect(self)
        self.shadow.setBlurRadius(20)
        self.shadow.setXOffset(0)
        self.shadow.setYOffset(0)
        self.shadow.setColor(QColor(0, 0, 0, 120))
        self.ui.circularBg.setGraphicsEffect(self.shadow)

        # QTIMER ==> START
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.progress)
        # TIMER IN MILLISECONDS
        self.timer.start(15)

        # SHOW ==> MAIN WINDOW
        ########################################################################
        self.show()
        ## ==> END ##

    # DEF TO LOANDING
    ########################################################################
    def progress(self):
        global counter
        global jumper
        value = counter

        # HTML TEXT PERCENTAGE
        htmlText = """
        <p><span style=" font-size:68pt;">{VALUE}</span>
        <span style=" font-size:58pt; vertical-align:super;">%</span></p>
        """

        # REPLACE VALUE
        newHtml = htmlText.replace("{VALUE}", str(jumper))

        if value > jumper:
            # APPLY NEW PERCENTAGE TEXT
            self.ui.labelPercentage.setText(newHtml)
            jumper += 10

        # SET VALUE TO PROGRESS BAR
        # fix max value error if > than 100
        if value >= 100:
            value = 1.000
        self.progressBarValue(value)
        if counter == 10:
            self.main = MainWindow()

        # CLOSE SPLASH SCREE AND OPEN APP
        if counter > 100:
            # STOP TIMER
            self.timer.stop()

            # SHOW MAIN WINDOW
            # self.main = MainWindow()
            self.main.show()

            # CLOSE SPLASH SCREEN
            self.close()

        # INCREASE COUNTER
        counter += 0.5

    #
    # # DEF PROGRESS BAR VALUE
    # ########################################################################
    def progressBarValue(self, value):
        # PROGRESSBAR STYLESHEET BASE
        styleSheet = """
        QFrame{
        	border-radius: 150px;
        	background-color: qconicalgradient(cx:0.5, cy:0.5, angle:90, 
        	stop:{STOP_1} rgba(255, 0, 127, 0), 
        	stop:{STOP_2} rgba(85, 170, 255, 255));
        }
        """
        #
        #     # GET PROGRESS BAR VALUE, CONVERT TO FLOAT AND INVERT VALUES
        #     # stop works of 1.000 to 0.000
        progress = (100 - value) / 100.0

        # GET NEW VALUES
        stop_1 = str(progress - 0.001)
        stop_2 = str(progress)
        #
        #     # SET VALUES TO NEW STYLESHEET
        newStylesheet = styleSheet.replace(
            "{STOP_1}", stop_1).replace("{STOP_2}", stop_2)
        #
        #     # APPLY STYLESHEET WITH NEW VALUES
        self.ui.circularProgress.setStyleSheet(newStylesheet)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = SplashScreen()
    sys.exit(app.exec_())
