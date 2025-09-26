#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QVector>

struct Snippet {
    QString title;
    QString content;
    bool wrap = false; // true 表示此按钮后换行
};

struct Group {
    QString name;
    QVector<Snippet> snippets;
};

#endif // TYPES_H