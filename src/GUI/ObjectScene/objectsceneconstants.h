#ifndef OBJECTSCENECONSTANTS_H
#define OBJECTSCENECONSTANTS_H

namespace ObjectSceneConstants
{

using objectId_t = unsigned;

/**
 * @brief The ObjectField enum Определяет, какие данные могут быть в объекте
 */
enum ObjectField : int
{
    OBJECTFIELD_PARENTITEM_ID = 0,  //! Для комплексных объектов
    OBJECTFIELD_ID,                 //! Короткое имя объекта для отображения
    OBJECTFIELD_OBJECTTYPE,         //! Специальное поле для идентификации типа объекта (желательно к использованию при сложной логике)

    OBJECTFIELD_NAME_SHORT,         //! Короткое имя объекта
    OBJECTFIELD_NAME,               //! Полное имя объекта
    OBJECTFIELD_DESCRIPTION,        //! Описание объекта

    OBJECTFIELD_PROPERTY_JSON,      //! JSON с кастомными (не универсальными) свойствами объекта
};

/**
 * @brief The ObjectType enum Определяет, какого типа этот объект. Используется в регулировании взаимодействий
 */
enum ObjectType : int
{
    OBJECTTYPE_NONE = 0,
    OBJECTTYPE_ARROWLINE,
    OBJECTTYPE_VERTEX,
    OBJECTTYPE_VERTEX_CONNECTION,
};

}

#endif // OBJECTSCENECONSTANTS_H
