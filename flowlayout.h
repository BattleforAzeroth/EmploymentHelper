#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QStyle>

class FlowLayout : public QLayout
{
public:
    explicit FlowLayout(QWidget *parent = nullptr, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    ~FlowLayout();

    void addItem(QLayoutItem *item) override;
    int horizontalSpacing() const;
    int verticalSpacing() const;
    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int) const override;
    int count() const override;
    QLayoutItem *itemAt(int index) const override;
    QSize minimumSize() const override;
    void setGeometry(const QRect &rect) override;
    QLayoutItem *takeAt(int index) override;

    // 自定义：强制换行时可插入一个“LineBreak”占位
    void addLineBreak();

private:
    int doLayout(const QRect &rect, bool testOnly) const;
    int smartSpacing(QStyle::PixelMetric pm) const;

    QList<QLayoutItem*> itemList;
    QList<int> breakIndices; // 在这些 index 之后换行
    int m_hSpace;
    int m_vSpace;
};

#endif // FLOWLAYOUT_H