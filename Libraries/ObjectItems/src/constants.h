#ifndef ObjectViewConstants_H
#define ObjectViewConstants_H

#include <stdint.h>

#include <QString>
#include <boost/noncopyable.hpp>

namespace ObjectViewConstants {

/**
 * @brief The GraphConversionConfiguration class Структура информации по уровням
 * объектов на сцене
 */
struct ObjectSceneConfiguration : boost::noncopyable {
    // Уровни расположения объектов на сцене по их типу
    int connectionLineLayer = 10;  //! Уровень линий соединения вершин
    int vertexLayer = 20;          //! Уровень вершин

    int propertyEditorLayer = 100;  //! Уровень редактора свойств

    double vertexWidth = 100;  //! Ширина и высота вершины

    static ObjectSceneConfiguration& getInstance() {
        static ObjectSceneConfiguration inst;
        return inst;
    }
};

// СОГЛАШЕНИЕ:
// ID НЕ ДОЛЖЕН БЫТЬ ОТРИЦАТЕЛЬНЫМ ДЛЯ ОБЫЧНЫХ ЭЛЕМЕНТОВ
// ДЛЯ СИСТЕМНЫХ ID ОТРИЦАТЕЛЬНЫЙ
using objectId_t = long long;

/**
 * @brief The ObjectField enum Определяет, какие данные могут быть в объекте
 */
enum ObjectField : int {
    OBJECTFIELD_PARENTITEM_ID = 0,  //! Для комплексных объектов
    OBJECTFIELD_ID,                 //! Короткое имя объекта для отображения
    OBJECTFIELD_OBJECTTYPE,  //! Специальное поле для идентификации типа объекта
                             //! (желательно к использованию при сложной логике)

    OBJECTFIELD_NAME_SYSTEM,  //! Имя объекта в системе (изменять только в
                              //! конструкторе)

    OBJECTFIELD_NAME_SHORT,   //! Короткое имя объекта
    OBJECTFIELD_NAME,         //! Полное имя объекта
    OBJECTFIELD_DESCRIPTION,  //! Описание объекта

    OBJECTFIELD_COLOR_MAIN,        //! Основной цвет (цвет пера)
    OBJECTFIELD_COLOR_BACKGROUND,  //! Фоновый цвет (заполнения в объекте)
    OBJECTFIELD_COLOR_SELECTED,    //! Цвет индикации выбора

    OBJECTFIELD_PROPERTY_JSON,  //! JSON с кастомными (не универсальными)
                                //! свойствами объекта
};

/**
 * @brief The ObjectType enum Определяет, какого типа этот объект. Используется
 * в регулировании взаимодействий
 */
enum ObjectType : int {
    OBJECTTYPE_NONE = 0,
    OBJECTTYPE_PROPERTY_EDITOR,
    OBJECTTYPE_ARROWLINE,
    OBJECTTYPE_VERTEX,
    OBJECTTYPE_VERTEX_CONNECTION,
};

/**
 *  @brief Свойства, которые могут встречаться в CustomPropertyJson
 */
namespace CustomPropertyName {
const QString PROPERTY_BOUNDINGRECT{"boundingRect"};
}

}  // namespace ObjectViewConstants

#endif  // ObjectViewConstants_H
