#ifndef GRAPHMAINTANER_H
#define GRAPHMAINTANER_H

#include "graphobject.h"
#include "graphextendedobject.h"

namespace Graph
{

class GraphMaintainer;
using PMaintainer = std::shared_ptr<Graph::GraphMaintainer>;

/**
 * @brief The GraphMaintainer class Мастер данных графа
 */
class GraphMaintainer
{
public:
    GraphMaintainer();
    ~GraphMaintainer();

    bool operator ==(const GraphMaintainer &gObj_) const;
    bool operator ==(const PMaintainer &gObj_) const;

    bool operator !=(const GraphMaintainer &gObj_) const;
    bool operator !=(const PMaintainer &gObj_) const;

    static PMaintainer createInstance();

    /**
     * @brief getObject Получение инстанции объекта графа
     * @return          Ссылка на поле класса
     */
    GraphObject& getObject();

    /**
     * @brief getExtendedObject Получение инстанции расширенного объекта графа
     * @return                  Указатель на поле класса, являющееся Q_OBJECT наследником
     */
    GraphExtendedObject *getExtendedObject();

    /**
     * @brief resetMaintainer Полностью очищает все данные мейнтейнера
     */
    void resetMaintainer();

    // ============================================================== //
    // ================= Стандартные свойства графа ================= //
    // ============================================================== //
    /**
     * @brief setName   Изменить название графа
     * @param iName     Новое название
     */
    void setName(const QString& iName);

    /**
     * @brief getName   Получить название графа
     * @return          Строка с названием графа
     */
    QString getName() const;

    /**
     * @brief setDescription    Установить описание графа
     * @param iDescr            Описание графа
     */
    void setDescription(const QString& iDescr);

    /**
     * @brief getDescription    Получить описание графа
     * @return                  Описание графа
     */
    QString getDescription() const;

    /**
     * @brief setCreateTime     Установить время создания графа
     * @param iDateTime         Время создания графа
     */
    void setCreateTime(const QDateTime& iDateTime);

    /**
     * @brief getCreateTime     Получить время создания графа
     * @return                  Время создания графа
     */
    QDateTime getCreateTime() const;

    /**
     * @brief setEditTime   Установить время изменения графа
     * @param iDateTime     Время изменения графа
     */
    void setEditTime(const QDateTime& iDateTime);

    /**
     * @brief getEditTime   Получить время изменения графа
     * @return              Время изменения графа
     */
    QDateTime getEditTime() const;

    /**
     * @brief setCustomValue    Задание значения пользовательским данным
     * @param key               Ключ (уникальный или уже имеющийся)
     * @param value             Значение (любое, желательно без указателей)
     */
    void setCustomValue(const QString& key, const QVariant& value);

    /**
     * @brief removeCustomValue     Удалить пользовательские данные по ключу
     * @param key                   Ключ (уникальный)
     */
    void removeCustomValue(const QString& key);

    /**
     * @brief getCustomValue    Получение пользовательского значения
     * @param key               Ключ (уже имеющийся)
     * @return                  Значение или NULL QVariant при отсутствии такого ключа
     */
    QVariant getCustomValue(const QString& key) const;

    /**
     * @brief getCustomValueMap     Получение всех пользовательских данных
     * @return                      Словарь ключ-значение пользовательских данных
     */
    std::map<QString, QVariant> getCustomValueMap() const;

private:
    GraphExtendedObject m_graph;

    // Основные параметры графа
    QString     m_name;
    QString     m_description;
    QDateTime   m_createTime;
    QDateTime   m_editTime;

    std::map<QString, QVariant> m_customDataValues; //! Пользовательские данные
};

}

#endif // GRAPHMAINTANER_H
