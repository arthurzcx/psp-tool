import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import "qrc:/qml/"

import "qrc:/js/PageTwoImageCalculation.js" as Logic

Rectangle
{
    id: page

    property int m_width: 0
    property int m_height:0

    width: m_width
    height: m_height

    visible: true
    color: "transparent"

    GridLayout
    {
        id:grid

        anchors.fill: parent

        rows: 3
        columns: 2
        rowSpacing: 10
        columnSpacing: 10

        Rectangle
        {
            id: btnFirstImage

            width: 200
            height: 200

            Layout.row: 0
            Layout.column: 0
            Layout.columnSpan: 2

            color: "transparent"
            border.color: "black"
            border.width: 1

            Text
            {
                text:qsTr("First image")

                color:"red"

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            }

            Image
            {
                id: imageFirst

                width: parent.width
                height: parent.height

                source: ""

                fillMode: Image.PreserveAspectFit

                anchors.centerIn: parent

                visible: source === "" ? false : true

                Connections{
                    target: logicTwoImageCalculation

                    onUpdateFirstImage:{
                        imageFirst.source = firstImagePath;
                    }
                }
            }

            Colorize
            {
                anchors.fill: imageFirst
                source: imageFirst
                hue: 0.5
                saturation: 0.3
                lightness: 0.2
            }

            MouseArea
            {
                anchors.fill: parent

                onClicked:
                {
                    Logic.selectFirstImage();
                }
            }
        }

        Rectangle
        {
            id: btnSecondImage
            width: 200
            height: 200

            Layout.row: 1
            Layout.column: 0
            Layout.columnSpan: 2

            color: "transparent"
            border.color: "black"
            border.width: 1

            Text
            {
                text:qsTr("Second image")

                color:"red"

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            }

            Image
            {
                id: imageSecond

                width: parent.width
                height: parent.height

                source: ""
                fillMode: Image.PreserveAspectFit

                anchors.centerIn: parent

                visible: source === "" ? false : true

                Connections{
                    target: logicTwoImageCalculation

                    onUpdateSecondImage:{
                        imageSecond.source = secondImagePath;
                    }
                }
            }

            Colorize
            {
                anchors.fill: imageSecond
                source: imageSecond
                hue: 0.5
                saturation: 0.3
                lightness: 0.2
            }

            MouseArea{
                anchors.fill: parent

                onClicked:{
                    Logic.selectSecondImage();
                }
            }
        }

        RoundButton
        {
            id:btnCancel

            text: qsTr("Cancel")

            width: 90
            height: 30
            radius: 5

            Layout.row: 2
            Layout.column: 0
            Layout.alignment: Qt.AlignLeft

            onClicked:
            {
                Logic.emitReject();
            }
        }

        RoundButton
        {
            id:btnAccept

            text: qsTr("Accept")

            width: 90
            height: 30
            radius: 5

            Layout.row: 2
            Layout.column: 1
            Layout.alignment: Qt.AlignRight

            onClicked:
            {
                Logic.emitAccept();
            }
        }
    }
}
