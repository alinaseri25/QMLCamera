#ifndef FASTIMAGEITEM_H
#define FASTIMAGEITEM_H

#include <QObject>
#include <QQuickItem>
#include <QImage>

class FastImageItem : public QQuickItem
{
    Q_OBJECT
public:
    explicit FastImageItem(QQuickItem *parent = nullptr);

    Q_INVOKABLE void setImage(const QImage &image);

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
    QImage m_image;
    bool m_imageChanged = false;


signals:
};

#endif // FASTIMAGEITEM_H
