#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>


class ConfigManager
{
public:
    ConfigManager() {}

    QString getResolution() const { return m_resolution; }
    QString getAudioFormat() const { return m_audioFormat; }
    bool isSubtitlesEnabled() const { return m_subtitles; }

    void setResolution(const QString &res) { m_resolution = res; }
    void setAudioFormat(const QString &fmt) { m_audioFormat = fmt; }
    void setSubtitlesEnabled(bool enabled) { m_subtitles = enabled; }
    void setFileName(const QString &fileName);

private:
    QString m_resolution;
    QString m_audioFormat;
    QString m_fileName;
    bool m_subtitles;

};

#endif // CONFIGMANAGER_H
