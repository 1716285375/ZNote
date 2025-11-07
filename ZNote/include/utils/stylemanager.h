#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QObject>
#include <QString>

/**
 * @brief 样式管理器，负责加载和切换应用主题
 */
class StyleManager : public QObject
{
    Q_OBJECT

public:
    enum Theme {
        Light = 0,
        Dark = 1
    };
    Q_ENUM(Theme)

    explicit StyleManager(QObject *parent = nullptr);
    ~StyleManager() override = default;

    /**
     * @brief 获取单例实例
     */
    static StyleManager* instance();

    /**
     * @brief 设置主题
     * @param theme 主题类型（Light或Dark）
     */
    void setTheme(Theme theme);

    /**
     * @brief 获取当前主题
     * @return 当前主题
     */
    Theme currentTheme() const { return m_currentTheme; }

    /**
     * @brief 加载样式文件
     * @param theme 主题类型
     * @return 成功返回true，失败返回false
     */
    bool loadStyle(Theme theme);

    /**
     * @brief 从资源文件加载样式
     * @param resourcePath 资源路径（如":/qss/light.qss"）
     * @return 成功返回true，失败返回false
     */
    bool loadStyleFromResource(const QString &resourcePath);

    /**
     * @brief 从文件系统加载样式
     * @param filePath 文件路径
     * @return 成功返回true，失败返回false
     */
    bool loadStyleFromFile(const QString &filePath);

signals:
    /**
     * @brief 主题改变信号
     * @param theme 新主题
     */
    void themeChanged(Theme theme);

private:
    Theme m_currentTheme;
    QString m_styleSheet;

    static StyleManager* s_instance;
};

#endif // STYLEMANAGER_H

