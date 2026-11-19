// import QtQuick
// import QtQuick.Controls
// import QtQuick.Layouts

// Item {
//     id: root

//     // Public
//     property alias text: lineEdit.text                 // текущий текст
//     property alias placeholder: lineEdit.placeholderText
//     property alias option1Text: option1.text
//     property alias option2Text: option2.text

//     // индекс выбранной опции: 0 — первый, 1 — второй, -1 — ничего
//     readonly property int selectedOption: option1.checked ? 0
//                                          : option2.checked ? 1
//                                          : -1

//     signal accepted(string text, int option)

//     // Sizes
//     implicitWidth:  layout.implicitWidth
//     implicitHeight: layout.implicitHeight

//     ColumnLayout {
//         id: layout
//         anchors.fill: parent
//         spacing: 8

//         TextField {
//             id: lineEdit
//             Layout.fillWidth: true

//             onAccepted: root.accepted(text, root.selectedOption)
//         }

//         // Группа радиокнопок (взаимоисключающие)
//         ButtonGroup {
//             id: group
//             exclusive: true
//         }

//         RadioButton {
//             id: option1
//             text: qsTr("Вариант 1")
//             checked: true
//             ButtonGroup.group: group
//         }

//         RadioButton {
//             id: option2
//             text: qsTr("Вариант 2")
//             ButtonGroup.group: group
//         }

//         // (опционально) кнопка подтверждения
//         Button {
//             Layout.alignment: Qt.AlignRight
//             text: qsTr("OK")
//             enabled: root.selectedOption >= 0
//             onClicked: root.accepted(root.text, root.selectedOption)
//         }
//     }
// }
