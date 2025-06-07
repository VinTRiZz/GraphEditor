#ifndef OBJECTSCENECONSTANTS_H
#define OBJECTSCENECONSTANTS_H

namespace ObjectSceneConstants
{

/**
 * @brief The ObjectField enum Определяет, какие данные могут быть в объекте
 */
enum ObjectField
{
    OBJECTFIELD_NAME_SHORT,     //! Короткое имя объекта для отображения
    OBJECTFIELD_NAME,           //! Полное имя объекта
    OBJECTFIELD_DESCRIPTION,    //! Описание объекта
    OBJECTFIELD_PROPERTY_JSON,  //! JSON с кастомными (не универсальными) свойствами объекта
};

}

#endif // OBJECTSCENECONSTANTS_H
