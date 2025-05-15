import sys
import numpy as np
import cv2
import sys
from PyQt5.QtWidgets import  QWidget, QLabel, QApplication
from PyQt5.QtCore import QThread, Qt, pyqtSignal, pyqtSlot
from PyQt5.QtGui import QImage, QPixmap

class Thread(QThread):
    changePixmap = pyqtSignal(QImage)

    def run(self):
        cap = cv2.VideoCapture(r'C:\Users\abel_\OneDrive\Desktop\RIG\AUV Ground Control\redrod2.mp4')
        while cap.isOpened():
            ret, frame = cap.read()
            if ret:
                # https://stackoverflow.com/a/55468544/6622587
                rgbImage = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                h, w, ch = rgbImage.shape
                bytesPerLine = ch * w
                convertToQtFormat = QImage(rgbImage.data, w, h, bytesPerLine, QImage.Format_RGB888)
                p = convertToQtFormat.scaled(640, 480, Qt.KeepAspectRatio)
                self.changePixmap.emit(p)
                self.msleep(1000 // 30)


class App(QWidget):
    def __init__(self):
        super().__init__()
        [...]
        self.initUI()

    @pyqtSlot(QImage)
    def setImage(self, image):
        self.label.setPixmap(QPixmap.fromImage(image))

    def initUI(self):
        self.setWindowTitle('Abel')
        self.setGeometry(0, 0,500, 500)
        self.resize(500, 500)
        # create a label
        self.label = QLabel(self)
        self.label.move(280, 120)
        self.label.resize(640, 480)
        th = Thread(self)
        th.changePixmap.connect(self.setImage)
        th.start()
        self.show()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = App()
    ex.show()
    sys.exit(app.exec_())

# import cv2
# from PyQt5 import QtGui, QtWidgets
#
# import cv2
# import sys
# from PyQt5.QtWidgets import  QWidget, QLabel, QApplication
# from PyQt5.QtCore import QThread, Qt, pyqtSignal, pyqtSlot
# from PyQt5.QtGui import QImage, QPixmap
#
# if __name__ == '__main__':
#     import sys
#     src = cv2.imread('test.png')
#     src = cv2.cvtColor(src, cv2.COLOR_BGR2RGB)
#     h, w, ch = src.shape
#     bytesPerLine = ch * w
#     qImg = QtGui.QImage(src.data, w, h, bytesPerLine, QtGui.QImage.Format_RGB888)
#     app = QtWidgets.QApplication(sys.argv)
#     w = QtWidgets.QLabel()
#     w.setPixmap(QtGui.QPixmap.fromImage(qImg))
#     w.show()
#     sys.exit(app.exec_())