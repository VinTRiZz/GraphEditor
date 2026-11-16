#pragma once

#include "ovcanvaslayer.hpp"
#include "ovcontextmenulayer.hpp"

#include <QLabel>

namespace ObjectItems {
class TextLabel;
}

namespace OVLayers {

// Для алиаса
class OVInformationLayer;
using ObjectView = OVInformationLayer;

class OVInformationLayer :
        public OVCanvasLayer,
        public OVContextMenuLayer<OVInformationLayer>
{
    Q_OBJECT
public:
    explicit OVInformationLayer(QWidget* parent = nullptr);

    ObjectItems::TextLabel* getCursorLabel() const;
    QLabel* getInformationLabel() const;

    void setCurrentToolname(const QString& toolName);

    void setInfoLabelPresenter(const std::function<QString()>& pres);

    void setCursorValuesPresenter(const std::function<QString(const QPointF&)>& pres);

public slots:
    void setCursorLabelEnabled(bool isEn);
    void setInformationLabelEnabled(bool isEn);
    void setHighlightEnabled(bool isEn);

    void updateCursorLabel();
    void updateInformationLabel();
    void updateHighlight();

private:

    bool m_isCursorLabelEnabled {true};
    ObjectItems::TextLabel* m_pCursorLabel{
        nullptr};  //! Объект, который показывает информацию у курсора

    bool m_isHighlightEnabled {false};
    QGraphicsPathItem* m_highlightItem {nullptr};

    bool m_isInfoLabelEnabled {true};
    QLabel* m_pInformationLabel{nullptr};
    QString m_currentToolName;
    std::function<QString()> m_infoLabelPresenter;

    std::function<QString(const QPointF&)> m_cursorValuesPresenter;

protected:
    void mouseMoveEvent(QMouseEvent* e) override;

    void enterEvent(QEvent* e) override;
    void leaveEvent(QEvent* e) override;

    void resizeEvent(QResizeEvent* e) override;

    void contextMenuEvent(QContextMenuEvent* e) override;

    ObjectItems::TextLabel* getCursorLabel();
};

} // namespace OVLayers

