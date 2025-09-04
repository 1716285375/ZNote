#include "core/fileresolve.h"


#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QUrl>
#include <QMessageBox>

FileResolve::FileResolve()
{
}

FileResolve::~FileResolve()
{
}

QString FileResolve::markdownToHtml(const QString& markdownContent)
{
	QByteArray mdUtf8 = markdownContent.toUtf8();

	// 返回 const char* 的 HTML
	char* html = cmark_markdown_to_html(mdUtf8.constData(), mdUtf8.size(), CMARK_OPT_DEFAULT);

	QString htmlStr = QString::fromUtf8(html);
	// 不 free html，让 cmark 自己释放内存（不安全，但在某些预编译库里是必要的）
	// 或者改用 cmark_markdown_to_html_with_mem 或 cmark_markdown_to_html_alloc

	QString fullHtml = QString(
		"<!DOCTYPE html><html><head>"
		"<meta charset='utf-8'>"
		"<style>"
		"body { font-family: 'Segoe UI', sans-serif; padding: 1rem; color: #F3F4F6; background-color: #111827; }"
		"h1,h2,h3,h4,h5,h6 { color: #3B82F6; }"
		"a { color: #2563EB; text-decoration: none; }"
		"pre { background-color: #1F2937; padding: 0.5rem; overflow-x: auto; }"
		"code { background-color: #374151; padding: 2px 4px; border-radius: 4px; }"
		"</style>"
		"</head><body>%1</body></html>").arg(htmlStr);

	return fullHtml;
}

