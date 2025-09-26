#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "types.h"

class QScrollArea;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    bool loadJson(const QString &path);

private:
    void buildUi();
    void createGroupWidget(const Group &group, QWidget *parentLayoutHost);

    QVector<Group> m_groups;
    QScrollArea *m_scrollArea = nullptr;
    QWidget *m_container = nullptr;
};

#endif // MAINWINDOW_H