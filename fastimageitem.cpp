#include "fastimageitem.h"
#include <QSGSimpleTextureNode>
#include <QQuickWindow>

FastImageItem::FastImageItem(QQuickItem *parent)
    : QQuickItem{parent}
{
    setFlag(ItemHasContents, true);
}

void FastImageItem::setImage(const QImage &image)
{
    m_image = image;
    m_imageChanged = true;
    update(); // schedule repaint
}

QSGNode *FastImageItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    auto *node = static_cast<QSGSimpleTextureNode *>(oldNode);

    if (!node) {
        node = new QSGSimpleTextureNode();
    }

    if (!window()) {
        return node;
    }

    if (m_imageChanged && !m_image.isNull()) {
        QSGTexture *texture = window()->createTextureFromImage(
            m_image,
            QQuickWindow::TextureCanUseAtlas
            );

        node->setTexture(texture);
        node->setOwnsTexture(true);
        node->setRect(boundingRect());
        m_imageChanged = false;
    }

    return node;
}


