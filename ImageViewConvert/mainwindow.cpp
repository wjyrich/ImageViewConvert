#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QWheelEvent>
#include <QDebug>
#include <QMovie>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , svgviewer(new SvgViewer)
    , isDragging(false)
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
    zoomMsg = new QLabel();
    ui->statusBar->addPermanentWidget(zoomMsg);
    //在图片放大之后，当看不到其他地方时，采用scrollarea可以移动得到其他位置
    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    setCentralWidget(scrollArea);

    updateZoomLabel();
    connect(svgviewer, &SvgViewer::zoomChanged, this, &MainWindow::updateZoomLabel);

}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == imageLabel) {
        switch(event->type()){
            case QEvent::Wheel:{
                if(imageType == imagePNGJPG){
                    QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
                    if (wheelEvent->angleDelta().y() > 0) {
                        scaleImage(1.25);
                    } else {
                        scaleImage(0.8);
                    }
                    return true;
                }
                else if (imageType == imageGif){
                    QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
                    if(wheelEvent->angleDelta().y()>0)
                        scaleGif(1.25);
                    else
                        scaleGif(0.75);
                }
                break;
            }
            case QEvent::MouseMove:{
                const QMouseEvent* const me = static_cast<const QMouseEvent*>(event);
                const QPoint position = me->pos();
                ui->statusBar->showMessage(QString("(x,y) coordinates: (%1,%2)").arg(position.x()).arg(position.y()));
                if(isDragging){
                    QPoint delta = me->pos() - lastMousePosition;
                    imageLabel->move(imageLabel->pos() + delta); // 移动 QLabel
                    lastMousePosition = me->pos();
                }
                break;
            }
            case QEvent::MouseButtonPress:{
                QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
                if(mouseEvent->button()==Qt::LeftButton){
                    lastMousePosition  = mouseEvent->pos();
                    isDragging = true;
                }
                break;
            }
            case QEvent::MouseButtonRelease:{
                QMouseEvent *mouseEvent  = static_cast<QMouseEvent *>(event);
                if (mouseEvent->button() == Qt::LeftButton) {
                    isDragging = false;
                }
                break;
            }
            default:
                break;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image File", "",
                                                    "Images (*.png *.jpg *.jpeg *.bmp *.gif *.svg)");

    if (!fileName.isEmpty()) {
        QFileInfo fileInfo(fileName);
        QString fileExtension = fileInfo.suffix().toLower();
        if (fileExtension == "gif") {
            imageType = imageGif;
            movie = new QMovie(fileName);
            imageLabel->setMovie(movie);
            movie->start();
            imageLabel->adjustSize();
            updateActions(true);
        }
        else if(fileExtension == "svg"){
            imageType = imageSVG;
            if (svgviewer->openFile(fileName)) {
                setCentralWidget(svgviewer);
                updateActions(true);
            } else {
                QMessageBox::warning(this, "Open SVG", "Could not open the SVG file.");
            }
        }
        else{
            if(imageType == imageSVG){
                svgviewer->scene()->clear();
                svgviewer->close();
            }
            imageType = imagePNGJPG;
            imageSave = QImage(fileName);
            if (imageSave.isNull()) {
                QMessageBox::warning(this, "Open Image", "Could not open the image file.");
            }
            imageLabel->setPixmap(QPixmap::fromImage(imageSave));
            imageLabel->adjustSize();
            updateActions(true);
        }
        currentAngle = 0;
        scaleFactor = 1.0;
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
    this->ui->actionrestore->setEnabled(actionState);
}

void MainWindow::scaleImage(double factor)
{
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());
    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

}

void MainWindow::scaleGif(double factor)
{
    scaleFactor *= factor;
    QSize originalSize = imageLabel->movie()->currentPixmap().size(); // 获取原始图像大小
    imageLabel->resize(originalSize.width() * scaleFactor, originalSize.height() * scaleFactor);
}
void MainWindow::on_actionZoom_in_triggered()
{
    if(imageType == imagePNGJPG){
        scaleImage(1.25);
    }else if(imageType == imageSVG){
        svgviewer->zoomIn(2);
    }
    else if(imageType == imageGif){
        scaleGif(1.25);
    }
}

void MainWindow::on_actionZoom_out_triggered()
{
    if(imageType == imagePNGJPG){
        scaleImage(0.75);
    }else if(imageType == imageSVG){
        svgviewer->zoomIn(0.5);
    }else if(imageType == imageGif){
        scaleGif(0.75);
    }
}

void MainWindow::rotateImage(int angle)
{
    currentAngle += angle;
    QTransform transform;
    transform.rotate(currentAngle);
    if(imageType == imagePNGJPG){
        QImage rotateImage = imageSave.transformed(transform,Qt::SmoothTransformation);
        imageLabel->setPixmap(QPixmap::fromImage(rotateImage));
    }
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    int newValue = factor * scrollBar->value() + (factor - 1) * scrollBar->pageStep() / 2;
    scrollBar->setValue(newValue);
}


void MainWindow::on_actionRotate_left_triggered()
{
    if(imageType == imagePNGJPG){
        rotateImage(-90);
    }else if(imageType == imageSVG){
        svgviewer->rotateView(-90);
    }
}

void MainWindow::on_actionRotate_right_triggered()
{
    if(imageType == imagePNGJPG  ){
        rotateImage(90);
    }else if(imageType == imageSVG){
        svgviewer->rotateView(90);
    }
}

void MainWindow::on_actionrestore_triggered()
{
    QSize windowSize = scrollArea->size();
    QSize labelSize;
    if(imageType == imagePNGJPG)
        labelSize = imageLabel->pixmap()->size();
    else
        labelSize = imageLabel->movie()->currentPixmap().size();
    double imageRatio = double(labelSize.height()) / labelSize.width();
    double scaleTo;

    if (windowSize.width() * imageRatio > windowSize.height()){
        scaleTo = double(windowSize.height()) / labelSize.height();
    }
    else{
        scaleTo = double(windowSize.width()) / labelSize.width();
    }

    double scaleBy = scaleTo / scaleFactor;
    if(imageType == imagePNGJPG)
        scaleImage(scaleBy);
    else
        scaleGif(scaleBy);
}
void MainWindow::updateZoomLabel(){
    const int percent = qRound(svgviewer->transform().m11() * qreal(100));
    zoomMsg->setText(QString::number(percent) + QLatin1Char('%'));
}
