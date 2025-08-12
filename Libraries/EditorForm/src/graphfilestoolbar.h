#ifndef GRAPHFILESTOOLBAR_H
#define GRAPHFILESTOOLBAR_H

#include <CustomWidgets/ButtonToolbar.h>

/**
 * @brief The GraphFilesToolbar class   Тулбар с кнопками для работы с файлами
 */
class GraphFilesToolbar : public ButtonToolbar::HeadWidget {
    Q_OBJECT
public:
    explicit GraphFilesToolbar(QWidget* parent = nullptr);

    /**
     * @brief setShowPropertiesEnabled  Переключить кнопку открытия свойств
     * @param isSaveEnabled
     */
    void setShowPropertiesEnabled(bool isPropOpenEnabled);

    /**
     * @brief setSaveEnabled    Переключить кнопку сохранения
     * @param isSaveEnabled
     */
    void setSaveEnabled(bool isSaveEnabled);

    /**
     * @brief setSaveAsEnabled  Переключить кнопку "сохранить как..."
     * @param isSaveAsEnabled
     */
    void setSaveAsEnabled(bool isSaveAsEnabled);

    /**
     * @brief setLoadEnabled    Переключить кнопку загрузки
     * @param isLoadEnabled
     */
    void setLoadEnabled(bool isLoadEnabled);

signals:
    // Сигналы для скрытия/показа (прокидка сигнала кнопки)
    void showProperties();
    void hideProperties();

    // Сигналы работы с сохранением графа
    void createGraph();
    void saveGraph(const QString& sPath);
    void loadGraph(const QString& lPath);

private:
    unsigned m_showPropertiesButtonIndex{0};
    unsigned m_saveButtonIndex{0};
    unsigned m_saveAsButtonIndex{0};
    unsigned m_loadButtonIndex{0};
};

#endif  // GRAPHFILESTOOLBAR_H
