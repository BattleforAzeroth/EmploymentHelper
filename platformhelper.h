#ifndef PLATFORMHELPER_H
#define PLATFORMHELPER_H

#include <QString>
#include <QWidget>

namespace PlatformHelper {

// 在按钮点击后调用：复制 text -> 最小化 self -> 延时 -> 发送粘贴快捷键
void sendTextToPreviousApp(const QString &text, QWidget *self);

} // namespace PlatformHelper

#endif // PLATFORMHELPER_H