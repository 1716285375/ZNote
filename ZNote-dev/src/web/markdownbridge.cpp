#include "web/markdownbridge.h"






void MarkdownBridge::sendToQt(const QString &markdown)
{
    qDebug() << "JS发送到Qt:" << markdown;
    emit markdownChanged(markdown);
}

void MarkdownBridge::updateEditor(const QString &markdown)
{
    // 调用JS更新编辑器内容
    emit updateJS(markdown);
}
