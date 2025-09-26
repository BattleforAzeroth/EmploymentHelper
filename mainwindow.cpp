#include "mainwindow.h"
#include "flowlayout.h"
#include "platformhelper.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScreen>
#include <QGuiApplication>
#include <QApplication>
#include <QStyle>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("秋招神器 - EmploymentHelper"));
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_AlwaysShowToolTips, true);

    // 初始最小尺寸
    resize(800, 300);

    // 高 DPI 建议（也可在 main 中设置）
    // QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QString jsonPath = QCoreApplication::applicationDirPath() + "/resume_data.json";
    if (!loadJson(jsonPath)) {
        qWarning() << "Failed to load JSON file:" << jsonPath;
    }

    buildUi();

    // 根据屏幕宽度设定条状宽度
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        int w = int(scr->availableGeometry().width() * 0.70);
        setFixedWidth(w);
        // 高度可根据内容，如果太高则滚动
    }
}

bool MainWindow::loadJson(const QString &path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open JSON file:" << path;
        return false;
    }
    QByteArray data = f.readAll();
    f.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << err.errorString();
        return false;
    }
    if (!doc.isObject()) return false;

    QJsonObject root = doc.object();
    QJsonArray groups = root.value("Groups").toArray();
    m_groups.clear();
    m_groups.reserve(groups.size());

    for (const QJsonValue &gv : groups) {
        if (!gv.isObject()) continue;
        QJsonObject gobj = gv.toObject();
        Group g;
        g.name = gobj.value("Name").toString();
        QJsonArray snippets = gobj.value("Snippets").toArray();
        for (const QJsonValue &sv : snippets) {
            if (!sv.isObject()) continue;
            QJsonObject sobj = sv.toObject();
            Snippet s;
            s.title = sobj.value("Title").toString();
            s.content = sobj.value("Content").toString();
            QString wrapStr = sobj.value("wrap").toString();
            s.wrap = (wrapStr.compare("true", Qt::CaseInsensitive) == 0);
            g.snippets.append(s);
        }
        m_groups.append(g);
    }
    return true;
}

void MainWindow::buildUi() {
    if (m_scrollArea) {
        m_scrollArea->deleteLater();
        m_scrollArea = nullptr;
    }

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    m_container = new QWidget;
    auto *vbox = new QVBoxLayout(m_container);
    vbox->setContentsMargins(8, 8, 8, 8);
    vbox->setSpacing(12);

    for (const auto &g : m_groups) {
        createGroupWidget(g, m_container);
    }
    vbox->addStretch(1);

    m_scrollArea->setWidget(m_container);
    setCentralWidget(m_scrollArea);

    // 样式可适当调整
    setStyleSheet(R"(
        QLabel.groupTitle {
            font-weight: 600;
            color: #333;
            margin: 4px 2px;
        }
        QPushButton.snippetButton {
            background: #f5f6f8;
            border: 1px solid #ccc;
            border-radius: 4px;
            padding: 4px 10px;
        }
        QPushButton.snippetButton:hover {
            background: #e6eefc;
            border-color: #5c8ed8;
        }
        QPushButton.snippetButton:pressed {
            background: #d2e0f7;
        }
    )");
}

void MainWindow::createGroupWidget(const Group &group, QWidget *parentLayoutHost) {
    auto *groupWidget = new QWidget(parentLayoutHost);
    auto *groupLayout = new QVBoxLayout(groupWidget);
    groupLayout->setContentsMargins(0,0,0,0);
    groupLayout->setSpacing(4);

    auto *titleLabel = new QLabel(group.name, groupWidget);
    titleLabel->setObjectName("groupTitle");
    titleLabel->setProperty("class", "groupTitle");
    groupLayout->addWidget(titleLabel);

    auto *flowContainer = new QWidget(groupWidget);
    auto *flow = new FlowLayout(flowContainer, 0, 8, 8);

    for (int i = 0; i < group.snippets.size(); ++i) {
        const Snippet &s = group.snippets[i];
        auto *btn = new QPushButton(s.title, flowContainer);
        btn->setObjectName("snippetButton");
        btn->setProperty("class", "snippetButton");
        btn->setToolTip(s.content);

        // 自适应根据内容加粗/缩小策略（可再优化）
        btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        connect(btn, &QPushButton::clicked, this, [s, this]() {
            PlatformHelper::sendTextToPreviousApp(s.content, this);
        });

        flow->addWidget(btn);

        if (s.wrap) {
            // wrap = true -> 强制换行
            flow->addLineBreak();
        }
    }

    groupLayout->addWidget(flowContainer);
    groupLayout->addSpacing(4);

    // 加入到主容器布局
    if (auto *vbox = qobject_cast<QVBoxLayout*>(m_container->layout())) {
        vbox->addWidget(groupWidget);
    }
}