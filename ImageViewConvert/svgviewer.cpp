#include "svgviewer.h"

#include <QDebug>

SvgViewer::SvgViewer(QWidget *parent)
    : QGraphicsView(parent)
    , m_svgItem(nullptr)
{
    setScene(new QGraphicsScene(this));
}

bool SvgViewer::openFile(const QString &fileName)
{
    QGraphicsScene *s = scene();
    QScopedPointer<QGraphicsSvgItem> svgItem(new QGraphicsSvgItem(fileName));
    if(!svgItem->renderer()->isValid())
        return false;

    s->clear();
    resetTransform();
    m_svgItem = svgItem.take();
    m_svgItem->setFlags(QGraphicsItem::ItemClipsToShape);
    m_svgItem->setCacheMode(QGraphicsItem::NoCache);
    m_svgItem->setZValue(0);
    s->addItem(m_svgItem);
    return true;
}

void SvgViewer::paintEvent(QPaintEvent *event)
{
    if (m_image.size() != viewport()->size()) {
        m_image = QImage(viewport()->size(), QImage::Format_ARGB32_Premultiplied);
    }
    QPainter imagePainter(&m_image);
    QGraphicsView::render(&imagePainter);
    imagePainter.end();
    QPainter p(viewport());
    p.drawImage(0, 0, m_image);
    QGraphicsView::paintEvent(event); //让SVG能够动起来
}
