#ifndef MARKDOWNBRIDGE_H
#define MARKDOWNBRIDGE_H

#include <QObject>
#include <QString>
#include <QDebug>

class MarkdownBridge : public QObject
{
    Q_OBJECT
public:
    explicit MarkdownBridge(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    void sendToQt(const QString &markdown);

    void updateEditor(const QString &markdown);

signals:
    void markdownChanged(const QString &markdown);
    void updateJS(const QString &markdown);
};


#endif // MARKDOWNBRIDGE_H
