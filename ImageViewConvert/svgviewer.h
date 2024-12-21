#ifndef SVGVIEWER_H
#define SVGVIEWER_H

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>
#include <QSvgRenderer>
#include <QPaintEvent>
class SvgViewer : public QGraphicsView
{
    Q_OBJECT
public:
    explicit SvgViewer(QWidget *parent = nullptr);
    bool openFile(const QString &fileName);
    void zoomIn(qreal factor);
    void zoomOut(qreal factor);
    void rotateView(int angle);

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

signals:
    void zoomChanged();

private:
    QGraphicsSvgItem *m_svgItem;
    QImage m_image;
    int currentAngle;
};

#endif // SVGVIEWER_H
