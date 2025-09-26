#include "platformhelper.h"
#include <QClipboard>
#include <QGuiApplication>
#include <QApplication>
#include <QTimer>
#include <QDebug>

#ifdef Q_OS_WIN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef Q_OS_MAC
#include <QProcess>
#endif

namespace {

#ifdef Q_OS_WIN
void simulatePasteWindows() {
    // 使用 SendInput 模拟 Ctrl+V
    INPUT inputs[4] = {};
    // Ctrl down
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;
    // V down
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 'V';
    // V up
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 'V';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    // Ctrl up
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

    if (SendInput(4, inputs, sizeof(INPUT)) != 4) {
        qWarning() << "SendInput failed";
    }
}
#endif

#ifdef Q_OS_MAC
void simulatePasteMac() {
    // 用 AppleScript 调用系统事件 Command+V
    QString script = "tell application \"System Events\" to keystroke \"v\" using command down";
    QProcess::execute("/usr/bin/osascript", QStringList() << "-e" << script);
}
#endif

}

namespace PlatformHelper {

void sendTextToPreviousApp(const QString &text, QWidget *self) {
    // 复制到剪贴板
    QClipboard *cb = QGuiApplication::clipboard();
    cb->setText(text, QClipboard::Clipboard);
#if defined(Q_OS_MAC)
    // macOS 通常还同步到 find pasteboard; 可选：延时粘贴
#endif

    // 最小化自身 -> 让之前窗口回到前台
    self->showMinimized();

    // 延时后发送粘贴
    QTimer::singleShot(180, [text]() {
#ifdef Q_OS_WIN
        simulatePasteWindows();
#elif defined(Q_OS_MAC)
        simulatePasteMac();
#else
        // 其他平台（Linux）可扩展: Ctrl+V
        QKeySequence seq(QKeySequence::Paste);
        Q_UNUSED(seq);
        // TODO: Linux 下可使用 XTest 或 Wayland 专用 API（此处留空）
#endif
    });
}

} // namespace PlatformHelper