#pragma once

#include <QGraphicsItem>

#include <QPen>
#include <QBrush>

namespace ObjectItems {

// СОГЛАШЕНИЕ:
// ID НЕ ДОЛЖЕН БЫТЬ ОТРИЦАТЕЛЬНЫМ ДЛЯ ОБЫЧНЫХ ЭЛЕМЕНТОВ
// ДЛЯ СИСТЕМНЫХ ID ОТРИЦАТЕЛЬНЫЙ
// ID НЕ ДОЛЖЕН БЫТЬ РАВЕН 0
using objectId_t = long long;
const objectId_t NULL_OBJECT_ID { 0 };

/**
 * @brief The ObjectDataRole enum Определяет, какие данные могут быть в объекте
 */
enum ObjectDataRole : int {
    OBJECTDATAROLE_ID = Qt::UserRole + 1000,    //! Короткое имя объекта для отображения
    OBJECTDATAROLE_PARENTITEM_POINTER,          //! Для комплексных объектов
    OBJECTDATAROLE_COMPLEX_PARENTITEM_POINTER,  //! Для объектов из комплексных объектов

    // СОГЛАШЕНИЕ: Все кастомные типы должны быть после USERTYPE
    OBJECTDATAROLE_USERTYPE,   //! Для пользовательских типов
};

/**
 * @brief The ObjectType enum Тип объекта, чтобы не заниматься динамик кастами
 */
enum ObjectType : int {
    OIT_Undefined = -1,
    OIT_BasicItem,

    OIT_AnchorItem,

    OIT_AbstractConnectionLine,
    OIT_ArrowedConnectionLine,
    OIT_ElegantConnectionLine,

    OIT_AbstractText,
    OIT_TextLabel,

    OIT_GroupItem,
    OIT_CommentItem,
    OIT_MarkerItem,

    OIT_UserItemType = 100, // Для пользовательских типов объекта
};

class BasicItemInterface
{
private:
    ObjectItems::objectId_t m_id { NULL_OBJECT_ID };

    int m_itemType {ObjectType::OIT_Undefined};
    QString m_systemName {"Unknown item"};

    QString m_displayName;
    QString m_description;

    QPen m_linePen                      {Qt::black};
    QPen m_lineHoverPen                 {Qt::black};
    QPen m_selectionPen                 {QColor(180, 180, 130)};
    QBrush m_backgroundBrush            {Qt::transparent};
    QBrush m_backgroundHoverBrush       {Qt::transparent};
    QBrush m_backgroundSelectionBrush   {Qt::transparent};

    static objectId_t createSystemId();

public:
    bool isSystemObject() const;

    void setItemId(ObjectItems::objectId_t id);
    void setSystemId();
    ObjectItems::objectId_t getItemId() const;

    void setObjectType(int typ);
    int getObjectType() const;

    QString getSystemName() const;

    void setDisplayName(const QString& text);
    QString getDisplayName() const;

    void setDescription(const QString& text);
    QString getDescription() const;

    void setLinePen(const QPen& penC);
    QPen getLinePen() const;

    void setLineHoverPen(const QPen& penC);
    QPen getLineHoverPen() const;

    void setLineSelectionPen(const QPen& penC);
    QPen getLineSelectionPen() const;

    void setBackgroundBrush(const QBrush& brushC);
    QBrush getBackgroundBrush() const;

    void setBackgroundSelectionBrush(const QBrush& brushC);
    QBrush getBackgroundSelectionBrush() const;

    void setBackgroundHoverBrush(const QBrush& brushC);
    QBrush getBackgroundHoverBrush() const;

protected:
    virtual void processIdChange() = 0;
    virtual void processDisplayNameChange() = 0;
    virtual void processInternalDataChange() = 0;
    virtual void processColorChange() = 0;

    void setSystemName(const QString& iText);
};

}
