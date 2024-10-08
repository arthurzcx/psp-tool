import QtQuick 2.11
import QtQuick.Window 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

Window{
    id: root

    title: qsTr("Paint calibration data form disk")

    width: 520
    height: 500

    color: "white"

    property color textColor: "yellow"
    property color bkColor: "white"

    Component.onCompleted: {
        var jsonString = dataUtils.getModel();
        if(jsonString.length > 0){
            dataModel.clear();

            var jsonObject = JSON.parse(jsonString);
            for(var i = 0; i < jsonObject.length; i++){
                dataModel.append(jsonObject[i]);
            }

            dataView.forceLayout();
        }
    }

    ListModel{
        id: dataModel
    }

    Rectangle{
        id: leftHalf

        width: 200
        height: parent.height
        anchors.left: parent.left

        color: bkColor

        ColumnLayout{
            anchors.fill: parent

            spacing: 10

            ControlButton{
                id: selectDirectory

                text: qsTr("Select directory")

                onClicked: {
                    dataModel.clear();

                    var jsonObject = JSON.parse(dataUtils.selectDirectory());
                    for(var i = 0; i < jsonObject.length; i++){
                        dataModel.append(jsonObject[i]);
                    }

                    dataView.forceLayout();
                }
            }

            ListView{
                id: dataView

                width: leftHalf.width
                height: 400

                delegate: dataDelegate
                model: dataModel

                clip: true
                ScrollBar.vertical: ScrollBar{}
            }

            Component{
                id: dataDelegate

                Item{
                    id:wrapper

                    width: dataView.width
                    height: dataView.width

                    Rectangle{
                        id: delegateBk
                        anchors.fill: parent
                        color: wrapper.ListView.view.currentIndex === index ? "blue" : "black"
                        border.color: "red"
                    }

                    MouseArea{
                        id: delegateMouseArea

                        anchors.fill: parent

                        onClicked: {
                            wrapper.ListView.view.currentIndex = index;

                            inputName.text = dataModel.get(dataView.currentIndex).BaseName;
                            inputPressure = dataModel.get(dataView.currentIndex).Pressure.toString();
                            inputTemperature = dataModel.get(dataView.currentIndex).Temperature.toString();
                        }
                    }

                    Image{
                        id: delegateImage

                        width: parent.width
                        height: width - 2

                        fillMode: Image.PreserveAspectFit
                        anchors.centerIn: parent

                        source: ImagePath
                    }

                    Colorize
                    {
                        anchors.fill: delegateImage
                        source: delegateImage
                        hue: 0.5
                        saturation: 0.3
                        lightness: 0.2
                    }

                    Text {
                        id: delegateIndex
                        text: index.toString()
                        anchors.left: delegateImage.left
                        anchors.top: delegateImage.top
                        color: textColor
                    }

                    Text{
                        id: delegateBaseName
                        text: BaseName
                        anchors.left: delegateImage.left
                        anchors.bottom: delegateImage.bottom
                        color: textColor
                    }

                    Text {
                        id: delegatePressure
                        text: qsTr("Pressure: ") + Pressure.toString()
                        anchors.left: delegateImage.left
                        anchors.bottom: delegateBaseName.top
                        visible: (isBackground !== true)
                        color: textColor
                    }

                    Text {
                        id: delegateTemperature
                        text: qsTr("Temperature: ") + Temperature.toString()
                        anchors.left: delegateImage.left
                        anchors.bottom: delegatePressure.top
                        visible: (isBackground !== true)
                        color: textColor
                    }

                    Text {
                        id: delegateIsBackground
                        text: qsTr("Background")
                        anchors.centerIn: delegateImage
                        visible: isBackground === true
                        color: textColor
                    }

                    Text {
                        id: delegateIsReference
                        text: qsTr("Reference")
                        anchors.centerIn: delegateImage
                        visible: isReference === true
                        color: textColor
                    }
                }
            }

            Row{
                spacing: 5

                ControlButton{
                    id: btnSetBackground
                    width: 80
                    text: qsTr("Set BK")

                    onClicked: {
                        for(var i = 0 ; i < dataModel.count; i++){
                                dataModel.get(i).isBackground = false;
                        }

                        dataModel.get(dataView.currentIndex).isBackground = true;
                        dataModel.get(dataView.currentIndex).isReference = false;

                        dataView.forceLayout();
                    }
                }

                ControlButton{
                    id: btnSetReference
                    width: 80
                    text: qsTr("Set REF")

                    onClicked: {
                        for(var i = 0 ; i < dataModel.count; i++){
                                dataModel.get(i).isReference = false;
                        }

                        dataModel.get(dataView.currentIndex).isReference = true;
                        dataModel.get(dataView.currentIndex).isBackground = false;

                        dataView.forceLayout();
                    }
                }
            }
        }
    }

    Rectangle{
        id: rightHalf

        width: parent.width - leftHalf.width
        height: 250
        anchors.right: parent.right

        color: bkColor

        property int labelWidth: 60

        GridLayout{
            anchors.fill: parent
            anchors.topMargin: 30
            anchors.leftMargin: 5

            columns: 2
            columnSpacing: 5
            rowSpacing: 5

            ControlText{
                text:qsTr("Name")
                width: rightHalf.labelWidth

                Layout.row: 0
                Layout.column: 0
                Layout.alignment: Qt.AlignVCenter
            }

            ControlInput{
                id:inputName

                inputMethodHints: Qt.ImhNone

                text: dataView.currentIndex !== -1 ? dataModel.get(dataView.currentIndex).BaseName : "0"

                Layout.row: 0
                Layout.column: 1
                Layout.alignment: Qt.AlignVCenter
            }

            ControlText{
                text: qsTr("Pressure")
                width: rightHalf.labelWidth

                Layout.row: 1
                Layout.column: 0
                Layout.alignment: Qt.AlignVCenter
            }

            ControlInput{
                id: inputPressure

                text: dataView.currentIndex !== -1 ? dataModel.get(dataView.currentIndex).Pressure : "0"

                Layout.row: 1
                Layout.column: 1
                Layout.alignment: Qt.AlignVCenter
            }

            ControlText{
                text: qsTr("Temperature")
                width: rightHalf.labelWidth

                Layout.row: 2
                Layout.column: 0
                Layout.alignment: Qt.AlignVCenter
            }

            ControlInput{
                id: inputTemperature

                text: dataView.currentIndex !== -1 ? dataModel.get(dataView.currentIndex).Temperature : "0"

                Layout.row: 2
                Layout.column: 1
            }

            ControlButton{
                text: qsTr("Config")

                Layout.row: 3
                Layout.column: 1

                onClicked: {
                    dataModel.get(dataView.currentIndex).Pressure = Number(inputPressure.text);
                    dataModel.get(dataView.currentIndex).Temperature = Number(inputTemperature.text);

                    dataView.forceLayout();
                }
            }
        }
    }

    ControlButton{
        id: btnApplt

        text: qsTr("Apply")

        anchors.right: parent.right
        anchors.bottom: parent.bottom

        onClicked: {
            root.pushDataToProject();
            close();
        }
    }

    function pushDataToProject(){
        var modelArray = new Array();
        for(var i = 0; i < dataModel.count; i++){
            modelArray.push(dataModel.get(i));
        }

        dataUtils.setModelFormQml(JSON.stringify(modelArray));
    }
}
