#include "urlparser.h"
#include "misc.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include <QDebug>

UrlParser::UrlParser(QObject* parent)
	: QObject(parent),
	process(new QProcess(this)),
	buffer(QByteArray())
{
	connect(process, &QProcess::readyReadStandardOutput, this, [this]() {
		// 将标准输出追加到缓冲区
		buffer.append(process->readAllStandardOutput());  // 累积所有的输出
		});

	connect(process, &QProcess::finished, this, [this]() {
		// 进程完成后，解析整个缓冲区中的内容
		parseOutput(buffer);
		buffer.clear();  // 解析后清空缓冲区
		process->deleteLater();  // 清理进程
		});
}

void UrlParser::parseOutput(const QByteArray& output)
{
	// 将缓冲区数据转换为 QString 来打印调试
	//qDebug() << "Process output as string:" << QString::fromUtf8(output);

	// 解析 JSON 数据
	QJsonParseError err;
	QJsonDocument doc = QJsonDocument::fromJson(output, &err);

	if (err.error != QJsonParseError::NoError)
	{
		emit errorOccurred("JSON parse error: " + err.errorString());
		return;
	}

	if (!doc.isObject())
	{
		emit errorOccurred("Invalid JSON from yt-dlp");
		return;
	}

	QJsonObject root = doc.object();
	QString playlistTitle = root.value("title").toString("UnknownTitle" + znote::utils::generateRandomString(6));
	QString id = root.value("id").toString();
	int playlistCount = root.value("playlist_count").toInt();

	QList<ParsedEntry> results;
	QJsonArray entries = root.value("entries").toArray();
	int index = 1;

	// 定义正则表达式，匹配 "?p=" 后面的数字
	QRegularExpression re("\\?p=(\\d+)");


	// 解析 entries
	for (const QJsonValue& v : entries)
	{
		QJsonObject obj = v.toObject();
		ParsedEntry entry;
		entry.url = obj.value("url").toString();
		entry.playlistTitle = playlistTitle;
		entry.id = id;

		QRegularExpressionMatch match = re.match(entry.url);

		if (match.hasMatch()) {
			// 提取数字部分
			entry.index = match.captured(1).toInt(); // captured(1) 获取第一个捕获组，即数字部分
			qDebug() << "Extracted number:" << entry.index;
		}
		else {
			entry.index;
		}
		entry.playlistCount = playlistCount;
		entry.type = UrlType::Lists;
		index += 1;
		results.append(entry);
	}

	// 处理没有 entries 的情况，单个视频
	if (results.isEmpty())
	{
		ParsedEntry single;
		single.id = root.value("id").toString();
		single.playlistTitle = playlistTitle;
		single.url = root.value("webpage_url").toString();
		single.playlistCount = 1;
		single.type = UrlType::Single;
		results.append(single);
	}

	znote::utils::printParsedEntries(results);

	// 发射结果信号
	emit urlParsed(results);
}

void UrlParser::parse(const QString& url)
{
	currentUrl = url;
	QStringList args = znote::utils::buildCommand(url, CommandType::ExtractPlaylist);

	// 调试输出命令
	znote::utils::printCommand(args);

	// 启动进程执行命令
	process->start("yt-dlp.exe", args);

	emit logMessage("▶ Running: yt-dlp.exe " + args.join(" "));
}
