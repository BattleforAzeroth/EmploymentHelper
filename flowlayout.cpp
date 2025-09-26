#include "flowlayout.h"
#include <QtMath>
#include <QWidget>

FlowLayout::FlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing)
{
    setContentsMargins(margin >= 0 ? margin : 6,
                       margin >= 0 ? margin : 6,
                       margin >= 0 ? margin : 6,
                       margin >= 0 ? margin : 6);
}

FlowLayout::~FlowLayout() {
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}

void FlowLayout::addItem(QLayoutItem *item) {
    itemList.append(item);
}

void FlowLayout::addLineBreak() {
    // 在当前最后一个 item 的 index 加入换行标记
    if (!itemList.isEmpty()) {
        breakIndices.append(itemList.size() - 1);
    }
}

int FlowLayout::horizontalSpacing() const {
    if (m_hSpace >= 0) return m_hSpace;
    return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

int FlowLayout::verticalSpacing() const {
    if (m_vSpace >= 0) return m_vSpace;
    return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}

int FlowLayout::count() const {
    return itemList.size();
}

QLayoutItem *FlowLayout::itemAt(int index) const {
    return itemList.value(index);
}

QLayoutItem *FlowLayout::takeAt(int index) {
    if (index >= 0 && index < itemList.size())
        return itemList.takeAt(index);
    return nullptr;
}

Qt::Orientations FlowLayout::expandingDirections() const {
    return {};
}

bool FlowLayout::hasHeightForWidth() const {
    return true;
}

int FlowLayout::heightForWidth(int width) const {
    return doLayout(QRect(0, 0, width, 0), true);
}

QSize FlowLayout::minimumSize() const {
    QSize size;
    for (QLayoutItem *item : itemList)
        size = size.expandedTo(item->minimumSize());

    QMargins m = contentsMargins();
    size += QSize(m.left() + m.right(), m.top() + m.bottom());
    return size;
}

void FlowLayout::setGeometry(const QRect &rect) {
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

int FlowLayout::doLayout(const QRect &rect, bool testOnly) const {
    int x = rect.x();
    int y = rect.y();
    int lineHeight = 0;

    int spaceX = horizontalSpacing();
    if (spaceX == -1)
        spaceX = 6;
    int spaceY = verticalSpacing();
    if (spaceY == -1)
        spaceY = 6;

    QMargins m = contentsMargins();
    x += m.left();
    y += m.top();
    int effectiveRight = rect.right() - m.right();

    for (int i = 0; i < itemList.size(); ++i) {
        QLayoutItem *item = itemList.at(i);
        QWidget *wid = item->widget();
        int space = wid ? wid->style()->layoutSpacing(
                              QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal)
                        : spaceX;
        QSize itemSize = item->sizeHint();
        int nextX = x + itemSize.width() + (lineHeight == 0 ? 0 : spaceX);

        bool forcedBreak = breakIndices.contains(i);

        if (nextX - (lineHeight == 0 ? 0 : spaceX) > effectiveRight && lineHeight > 0) {
            // 换行
            x = rect.x() + m.left();
            y += lineHeight + spaceY;
            nextX = x + itemSize.width();
            lineHeight = 0;
        }

        if (forcedBreak && lineHeight > 0 && x != rect.x() + m.left()) {
            // 本行放完再换行
            x = rect.x() + m.left();
            y += lineHeight + spaceY;
            lineHeight = 0;
        }

        if (!testOnly) {
            QRect geom(QPoint(x, y), itemSize);
            item->setGeometry(geom);
        }

        x = nextX + spaceX;
        lineHeight = qMax(lineHeight, itemSize.height());

        if (forcedBreak) {
            x = rect.x() + m.left();
            y += lineHeight + spaceY;
            lineHeight = 0;
        }
    }

    int totalHeight = y + lineHeight + contentsMargins().bottom() - rect.y();
    return totalHeight;
}

int FlowLayout::smartSpacing(QStyle::PixelMetric pm) const {
    QObject *parentObj = parent();
    if (!parentObj) {
        return -1;
    } else if (parentObj->isWidgetType()) {
        QWidget *pw = static_cast<QWidget *>(parentObj);
        return pw->style()->pixelMetric(pm, nullptr, pw);
    } else {
        return -1;
    }
}