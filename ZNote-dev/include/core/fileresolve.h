#ifndef FILERESOLVE_H
#define FILERESOLVE_H

#include <QWidget>
#include <QWebEngineView>
#include <QFile>
#include <QTextStream>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
extern "C" {
#include <cmark.h>
}


class FileResolve
{
public:
	FileResolve();
	~FileResolve();

	static QString markdownToHtml(const QString& markdownContent);
};

#endif // FILERESOLVE_H
