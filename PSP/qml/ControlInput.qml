import QtQuick 2.11
import QtQuick.Controls 2.4

TextField {
    id: control
    implicitWidth: 200
    implicitHeight: 30
    color: "black"
    selectionColor: "blue"
    selectedTextColor: color
    horizontalAlignment :TextInput.AlignRight    
    property bool modified : false
    selectByMouse: true
    validator: DoubleValidator{id : validator;}
    verticalAlignment: TextInput.AlignBottom
    bottomPadding: 7
    font.pixelSize: 13
    inputMethodHints: Qt.ImhDigitsOnly
    
    onTextEdited: {
        modified = true
    }

    background: Rectangle {
        y: control.height
        height: control.focus ? 2 : 1
        color:{
            control.focus ? "blue" : control.enabled ? "black" : "grey"
        }
        border.width: 0
    }
}
