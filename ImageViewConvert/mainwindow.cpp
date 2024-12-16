#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QWheelEvent>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("ImageViewConvert");
    setGeometry(300, 300, 800, 600);
    updateActions(false);
    imageLabel = new QLabel();
    imageLabel->setAlignment(Qt::AlignCenter);//图片显示小于label时候居中显示
    imageLabel->resize(0, 0);
    imageLabel->setMouseTracking(true);//允许 imageLabel 在鼠标移动时接收鼠标事件
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);//允许其根据内容自适应大小
    imageLabel->setScaledContents(true);
    imageLabel->installEventFilter(this);
    setCentralWidget(imageLabel);//设置为主窗口的中央小部件

    //在图片放大之后，当看不到其他地方时，采用scrollarea可以移动得到其他位置
    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    setCentralWidget(scrollArea);

}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == imageLabel) {
        if (event->type() == QEvent::Wheel) {
            QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
            if (wheelEvent->angleDelta().y() > 0) {
                scaleImage(1.25);
            } else {
                scaleImage(0.8);
            }
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image File", "", "Images (*.png *.jpg *.jpeg *.bmp *.gif *.svg)");
    if (!fileName.isEmpty()) {
        imageSave = QImage(fileName);
        if (imageSave.isNull()) {
            QMessageBox::warning(this, "Open Image", "Could not open the image file.");
            return;
        }
        currentAngle = 0;
        scaleFactor = 1.0;
        imageLabel->setPixmap(QPixmap::fromImage(imageSave));
        imageLabel->adjustSize();
        updateActions(true);
    }
}

void MainWindow::on_actionSave_triggered()
{
    QString folderPath = QFileDialog::getExistingDirectory(this,tr("选择保存的文件夹"),QDir::homePath());
    if(!folderPath.isEmpty()){
        QString imagePath = QFileDialog::getSaveFileName(this,
                                                         tr("Save File"),
                                                         folderPath + "/image",
                                                         tr("JPEG (*.jpg *.jpeg);;PNG (*.png);;SVG (*.svg)"));
        if(!imagePath.isEmpty()){
            if(imagePath.endsWith(".svg",Qt::CaseInsensitive)){
                imageSave.save(imagePath,"SVG");
            }else{
                imageSave.save(imagePath);
            }
        }
    }
}

void MainWindow::updateActions(bool actionState)
{
    this->ui->actionSave->setEnabled(actionState);
    this->ui->actionZoom_in->setEnabled(actionState);
    this->ui->actionZoom_out->setEnabled(actionState);
    this->ui->actionRotate_left->setEnabled(actionState);
    this->ui->actionRotate_right->setEnabled(actionState);

}

void MainWindow::scaleImage(double factor)
{
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());
    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

}

void MainWindow::on_actionZoom_in_triggered()
{
    scaleImage(1.25);
}

void MainWindow::on_actionZoom_out_triggered()
{
    scaleImage(0.75);
}

void MainWindow::rotateImage(int angle)
{
    currentAngle += angle;
    QTransform transform;
    transform.rotate(currentAngle);
    QImage rotateImage = imageSave.transformed(transform,Qt::SmoothTransformation);
    imageLabel->setPixmap(QPixmap::fromImage(rotateImage));
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    int newValue = factor * scrollBar->value() + (factor - 1) * scrollBar->pageStep() / 2;
    scrollBar->setValue(newValue);
}

void MainWindow::on_actionRotate_left_triggered()
{
    rotateImage(-90);
}

void MainWindow::on_actionRotate_right_triggered()
{
    rotateImage(90);
}
