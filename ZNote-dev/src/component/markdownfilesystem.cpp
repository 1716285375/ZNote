#include "component/markdownfilesystem.h"

#include <QDirIterator>
#include <QFile>
#include <QTextStream>

MarkdownFileSystem::MarkdownFileSystem(const QString& rootPath)
	: m_rootPath(rootPath)
{
}

QVector<QString> MarkdownFileSystem::scanMarkdownFiles() const
{
	QVector<QString> files;
	QDirIterator it(m_rootPath,
		QStringList() << "*.md",
		QDir::Files,
		QDirIterator::Subdirectories);

	while (it.hasNext()) {
		files.append(it.next());
	}
	return files;
}

QString MarkdownFileSystem::loadFile(const QString& filePath) const
{
	QFile f(filePath);
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
		return QString();

	QTextStream in(&f);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	return in.readAll(); 
#else
	return in.readAll();
#endif
}

