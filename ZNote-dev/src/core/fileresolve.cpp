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

	// 暗色
	//QString fullHtml = QString(
	//	"<!DOCTYPE html><html><head>"
	//	"<meta charset='utf-8'>"
	//	"<style>"
	//	"body { font-family: 'Segoe UI', sans-serif; padding: 1rem; color: #F3F4F6; background-color: #111827; }"
	//	"h1,h2,h3,h4,h5,h6 { color: #3B82F6; }"
	//	"a { color: #2563EB; text-decoration: none; }"
	//	"pre { background-color: #1F2937; padding: 0.5rem; overflow-x: auto; }"
	//	"code { background-color: #374151; padding: 2px 4px; border-radius: 4px; }"
	//	"</style>"
	//	"</head><body>%1</body></html>").arg(htmlStr);


	QString fullHtml = QString(
		"<!DOCTYPE html><html><head>"
		"<meta charset='utf-8'>"
		"<style>"
		"body { font-family: 'Segoe UI', sans-serif; padding: 1rem; color: #111827; background-color: #ffffff; }" // 白底深字
		"h1,h2,h3,h4,h5,h6 { color: #2563EB; }" // 标题蓝色
		"a { color: #1D4ED8; text-decoration: none; }" // 链接深蓝
		"pre { background-color: #F3F4F6; padding: 0.5rem; overflow-x: auto; }" // 代码块浅灰
		"code { background-color: #E5E7EB; padding: 2px 4px; border-radius: 4px; }" // 行内代码浅灰
		"blockquote { border-left: 4px solid #E5E7EB; padding-left: 1rem; color: #374151; }" // 引用
		"table { border-collapse: collapse; width: 100%; }"
		"th, td { border: 1px solid #E5E7EB; padding: 0.5rem; }"
		"</style>"
		"</head><body>%1</body></html>").arg(htmlStr);


	return fullHtml;
}

