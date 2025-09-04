#ifndef DOWNLOADQUEUE_H
#define DOWNLOADQUEUE_H

#include <QObject>
#include <QQueue>
#include <QDebug>

class DownloadQueue : public QQueue
{
    Q_OBJECT;
public:
    explicit DownloadQueue(QObject *parent = nullptr);

    void addTask(const QString &url);

    bool hasNext() const;

    QString takeNext();

    void clear();

signals:
    void allFinished();

private slots:
    handleFinished();

private:

    QQueue<QString> tasks;
};

#endif // DOWNLOADQUEUE_H
