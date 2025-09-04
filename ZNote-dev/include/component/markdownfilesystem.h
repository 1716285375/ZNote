#ifndef MARKDOWFILESYSTEM_H
#define MARKDOWFILESYSTEM_H


#include <QObject>
#include <QFileSystemModel>
#include <QString>
#include <QStringList>
#include <QVector>


class MarkdownFileSystem
{
public:
	explicit MarkdownFileSystem(const QString& rootPath);

	QVector<QString> scanMarkdownFiles() const;

	QString loadFile(const QString& filePath) const;

private:
	QString m_rootPath;
};

#endif MARKDOWFILESYSTEM_H
