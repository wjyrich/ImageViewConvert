#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "svgviewer.h"

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
enum ImageType{
    imageGif,
    imageSVG,
    imagePNGJPG
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    bool eventFilter(QObject* obj, QEvent* event);

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionZoom_in_triggered();

    void on_actionZoom_out_triggered();

    void on_actionRotate_left_triggered();

    void on_actionRotate_right_triggered();

private:
    Ui::MainWindow *ui;
    QLabel *imageLabel;
    QImage imageSave;//保存图片的参数 方便导出及后期复原
    double scaleFactor;//图片的缩小放大因子
    QScrollArea *scrollArea;//放大图片后采用scrollarea可以移动得到其他位置
    int currentAngle;
    ImageType imageType;

    SvgViewer *svgviewer;

    void scaleImage(double factor);
    void updateActions(bool actionState);
    void rotateImage(int angle);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
};
#endif // MAINWINDOW_H
