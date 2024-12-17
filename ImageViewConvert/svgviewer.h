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
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QGraphicsSvgItem *m_svgItem;
    QImage m_image;
};

#endif // SVGVIEWER_H
