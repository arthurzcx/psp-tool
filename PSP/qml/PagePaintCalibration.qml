import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import "qrc:/qml/"

import "qrc:/js/PagePaintCalibration.js" as Logic

Rectangle {
    id: page

    property int m_width: 0
    property int m_height: 0

    width: m_width
    height: m_height

    visible: true
    color: "transparent"

    GridLayout {
        id: grid

        anchors.fill: parent

        columns: 4
        rows: 3
        columnSpacing: 10
        rowSpacing: 10

        ControlButton {
            width: 240

            Layout.row: 0
            Layout.column: 0
            Layout.columnSpan: 2

            text: qsTr("Select directory")

            onClicked: {
                if (Logic.selectDirectory() === true) {
                    listViewImages.model = Logic.getModelImages();
                    listViewParameters.model = Logic.getModelParameter();
                }
            }
        }

        ControlButton {
            Layout.row: 0
            Layout.column: 2

            text: qsTr("Move up")

            onClicked: {
                var ret = Logic.moveUpIndexItem(listViewParameters.currentIndex, listViewParameters.model);
                if (ret !== false) {
                    listViewParameters.model = ret[1];
                    listViewParameters.currentIndex = ret[0];
                }
            }
        }

        ControlButton {
            Layout.row: 0
            Layout.column: 3

            text: qsTr("Move down")

            onClicked: {
                var ret = Logic.moveDownIndexItem(listViewParameters.currentIndex, listViewParameters.model);
                if (ret !== false) {
                    listViewParameters.model = ret[1];
                    listViewParameters.currentIndex = ret[0];
                }
            }
        }

        ListView {
            id: listViewImages

            Layout.row: 1
            Layout.column: 0
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignLeft

            width: 240
            Layout.fillHeight: true

            clip: true

            delegate: delegateImage

            ScrollBar.vertical: ScrollBar {
            }
        }

        ListView {
            id: listViewParameters

            Layout.row: 1
            Layout.column: 2
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignRight

            width: 240
            Layout.fillHeight: true

            clip: true

            delegate: delegateParameter

            ScrollBar.vertical: ScrollBar {
            }
        }

        ControlButton {
            Layout.row: 2
            Layout.column: 0

            text: qsTr("Set background")

            onClicked: {
                var ret = Logic.setAsBackground(listViewImages.currentIndex, listViewImages.model);
                if (ret !== false) {
                    listViewImages.model = ret[1];
                    listViewImages.currentIndex = ret[0];
                }
            }
        }

        ControlButton {
            Layout.row: 2
            Layout.column: 1

            text: qsTr("Set reference")

            onClicked: {
                var ret = Logic.setAsReference(listViewImages.currentIndex, listViewImages.model);
                if (ret !== false) {
                    listViewImages.model = ret[1];
                    listViewImages.currentIndex = ret[0];
                }
            }
        }

        ControlButton {
            Layout.row: 2
            Layout.column: 3

            text: qsTr("Apply")

            onClicked: {
                var ret = Logic.applyFit(listViewImages.model, listViewParameters.model);
                if (ret !== false) {
                    listViewImages.model = ret[0];
                    listViewParameters.model = ret[1];

                    Logic.applyModelImages(listViewImages.model);
                }

            }
        }

        Component {
            id: delegateImage

            Item {
                id: wrapper

                width: listViewImages.width
                height: 200

                property color highlightColor: "blue"
                property color generalColor: "yellow"

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        wrapper.ListView.view.currentIndex = index;
                    }
                }

                Rectangle {
                    anchors.fill: parent
                    width: parent.width
                    height: parent.height

                    color: wrapper.ListView.view.currentIndex === index ? wrapper.highlightColor : "white"
                    opacity: 0.5
                }

                Image {
                    id: image

                    width: parent.width
                    height: 180

                    source: listViewImages.model[index].url
                    fillMode: Image.PreserveAspectFit

                    anchors.centerIn: parent
                }

                Colorize {
                    anchors.fill: image
                    source: image
                    hue: 0.5
                    saturation: 0.3
                    lightness: 0.2
                }

                Text {
                    id: textIndex

                    color: wrapper.ListView.view.currentIndex === index ? wrapper.highlightColor : wrapper.generalColor
                    text: index

                    anchors.top: image.top
                    anchors.left: image.left
                    anchors.leftMargin: 5
                }

                Text {
                    id: textBackgoundOrReferece

                    color: wrapper.ListView.view.currentIndex === index ? wrapper.highlightColor : wrapper.generalColor

                    text: listViewImages.model[index].isBackground === true ? qsTr("background") :
                        (listViewImages.model[index].isReference === true ? qsTr("reference") : "")
                    visible: (listViewImages.model[index].isBackground === true ||
                        listViewImages.model[index].isReference === true)

                    anchors.centerIn: image
                }


                Text {
                    id: textP

                    text: "P:" + listViewImages.model[index].P
                    color: wrapper.ListView.view.currentIndex === index ? wrapper.highlightColor : wrapper.generalColor
                    visible: listViewImages.model[index].P !== ""

                    anchors.left: image.left
                    anchors.bottom: textT.top
                    anchors.bottomMargin: 2
                }

                Text {
                    id: textT

                    text: "T:" + listViewImages.model[index].T
                    color: wrapper.ListView.view.currentIndex === index ? wrapper.highlightColor : wrapper.generalColor
                    visible: listViewImages.model[index].T !== ""

                    anchors.left: image.left
                    anchors.bottom: textImageName.top
                    anchors.bottomMargin: 2
                }


                Text {
                    id: textImageName

                    text: listViewImages.model[index].name

                    color: wrapper.ListView.view.currentIndex === index ? wrapper.highlightColor : wrapper.generalColor

                    anchors.left: image.left
                    anchors.bottom: image.bottom
                    anchors.bottomMargin: 2
                }

                Rectangle {
                    width: parent.width
                    height: 2
                    color: "red"
                    anchors.bottom: parent.bottom
                }
            }
        }

        Component {
            id: delegateParameter

            Item {
                id: wrapperPapameter

                width: listViewParameters.width
                height: 40

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        wrapperPapameter.ListView.view.currentIndex = index;
                    }
                }

                Rectangle {
                    anchors.fill: parent

                    width: parent.width
                    height: parent.height

                    color: wrapperPapameter.ListView.view.currentIndex === index ? "blue" : "white"
                }

                Text {
                    id: textIndex

                    text: index

                    width: 30
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    id: textPressure

                    text: "P: " + listViewParameters.model[index].P

                    width: (parent.width - textIndex.width) / 2
                    anchors.left: textIndex.right
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    id: textTemperature

                    text: "T: " + listViewParameters.model[index].T

                    width: (parent.width - textIndex.width) / 2
                    anchors.left: textPressure.right
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    width: parent.width
                    height: 2

                    color: "red"
                    anchors.bottom: parent.bottom
                }
            }
        }

    }
}


