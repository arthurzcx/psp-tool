import QtQuick 2.11
import QtQuick.Controls 2.4

Rectangle
{
    id: page

    property int m_width: 0
    property int m_height:0

    width: m_width
    height: m_height

    visible: true
    color: "transparent"

    ListModel
    {
        id: listProjectSingleComponentModel

        ListElement{centerContent:qsTr("background (wind on)"); imageSource:""; floatContent:""; imageSet:false}
        ListElement{centerContent:qsTr("background (wind off)"); imageSource:""; floatContent:""; imageSet:false}
        ListElement{centerContent:qsTr("Wind On"); imageSource:""; floatContent:""; imageSet:false; markerSet:false}
        ListElement{centerContent:qsTr("Wind Off"); imageSource:""; floatContent:""; imageSet:false; markerSet:false}
        ListElement{centerContent:qsTr("Paint Calibration file"); floatContent:""; fileBaseName:""; fileSet:false}
    }

    ListModel
    {
        id: listProjectDualComponentModel

        ListElement{centerContent:qsTr("bk on (paint-sensitive)"); imageSource:""; floatContent:""; imageSet:false}
        ListElement{centerContent:qsTr("bk off (paint-sensitive)"); imageSource:""; floatContent:""; imageSet:false}
        ListElement{centerContent:qsTr("bk on (reference)"); imageSource:""; floatContent:""; imageSet:false}
        ListElement{centerContent:qsTr("bk off (reference)"); imageSource:""; floatContent:""; imageSet:false}
        ListElement{centerContent:qsTr("Wind On (paint-sensitive)"); imageSource:""; floatContent:""; imageSet:false; markerSet:false}
        ListElement{centerContent:qsTr("Wind Off (paint-sensitive)"); imageSource:""; floatContent:""; imageSet:false; markerSet:false}
        ListElement{centerContent:qsTr("Wind On (reference)"); imageSource:""; floatContent:""; imageSet:false; markerSet:false}
        ListElement{centerContent:qsTr("Wind Off (reference)"); imageSource:""; floatContent:""; imageSet:false; markerSet:false}
        ListElement{centerContent:qsTr("Paint Calibration file"); floatContent:""; fileBaseName:""; fileSet:false}
    }

    ListView
    {
        id: listProject

        width: parent.width
        height: parent.height

        clip: true

        delegate: projectDelegate
        model: logicProjectNew.getProjectType() === 0 ? listProjectSingleComponentModel : listProjectDualComponentModel

        ScrollBar.vertical: ScrollBar{}
    }

    Component
    {
        id: projectDelegate
        Rectangle
        {
            id:delegateRect

            width: listProject.width
            height: width

            border.width: 1
            border.color: "red"

            color:"black"

            Image
            {
                id: listImage

                width: parent.width
                height: parent.height

                source: imageSource !== undefined ? imageSource : ""

                fillMode: Image.PreserveAspectFit

                anchors.centerIn: parent

                visible: source === "" ? false : true
            }

            Text
            {
                id: centerText
                text: centerContent
                anchors.centerIn: parent
                color:"yellow"
            }

            Text
            {
                id:paintCalibrationFileText
                text: index === listProject.model.count - 1 ? fileBaseName : ""
                anchors.horizontalCenter: centerText.horizontalCenter
                anchors.top: centerText.bottom
                color:"yellow"
                visible: index === listProject.model.count - 1
            }

            Text
            {
                id: floatText

                text: floatContent
                color: "yellow"
                width: 200
                wrapMode: Text.WrapAnywhere

                visible: false
            }

            Rectangle
            {
                id: infoRight

                anchors.right: delegateRect.right
                anchors.verticalCenter: delegateRect.verticalCenter

                color:"transparent"

                width: 70
                height: 48

                Rectangle
                {
                    id:infoImage
                    width: parent.width
                    height: parent.height/2
                    color: "transparent"
                    anchors.top:parent.top

                    Text
                    {
                        text:qsTr("image")
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        color:"yellow"

                        visible: index !== listProject.model.count - 1
                    }
                    Image
                    {
                        anchors.right: parent.right
                        source: (imageSet !== undefined && imageSet === true) ? "qrc:/res-image/ok.png" : "qrc:/res-image/notok.png"

                        width: 24
                        height: 24

                        fillMode: Image.PreserveAspectFit
                        clip:true

                        visible: index !== listProject.model.count - 1
                    }
                }

                Rectangle
                {
                    id:infoMark
                    width: parent.width
                    height: parent.height/2
                    color: "transparent"
                    anchors.top: infoImage.bottom

                    Text
                    {
                        text:qsTr("marker")
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        color:"yellow"

                        visible: logicProjectNew.getProjectType() === 0 ? (index > 1 && index !== listProject.model.count - 1) : (index > 3 && index !== listProject.model.count - 1)
                    }

                    Image
                    {
                        anchors.right: parent.right
                        source: (markerSet !== undefined && markerSet === true) ? "qrc:/res-image/ok.png" : "qrc:/res-image/notok.png"

                        width: 24
                        height: 24

                        fillMode: Image.PreserveAspectFit

                        visible: logicProjectNew.getProjectType() === 0 ? (index > 1 && index !== listProject.model.count - 1) : (index > 3 && index !== listProject.model.count - 1)
                    }
                }

                Rectangle
                {
                    id:infoPaintCalibrationFile
                    width: parent.width
                    height: parent.height/2
                    color: "transparent"

                    Text
                    {
                        text: qsTr("file")
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        color:"yellow"

                        visible: index === listProject.model.count - 1
                    }

                    Image
                    {
                        anchors.right: parent.right
                        source: (fileSet !== undefined && fileSet === true) ? "qrc:/res-image/ok.png" : "qrc:/res-image/notok.png"

                        width: 24
                        height: 24

                        fillMode: Image.PreserveAspectFit

                        visible: index === listProject.model.count - 1
                    }
                }
            }

            MouseArea
            {
                anchors.fill: parent

                hoverEnabled: true

                onClicked:
                {
                    switch(index)
                    {
                    case 0:
                        logicProjectNew.readPathBackground();
                        break;
                    case 1:                        
                        logicProjectNew.readPathBackground2();
                        break;
                    case 2:
                        if(logicProjectNew.getProjectType() === 0)
                        {
                            logicProjectNew.readPathWindOn0();
                            break;
                        }
                        else
                        {
                            logicProjectNew.readPathBackground3();
                            break;
                        }
                    case 3:
                        if(logicProjectNew.getProjectType() === 0)
                        {
                            logicProjectNew.readPathWindOff0();
                            break;
                        }
                        else
                        {
                            logicProjectNew.readPathBackground4();
                            break;
                        }
                    case 4:
                        if(logicProjectNew.getProjectType() === 0)
                        {
                            logicProjectNew.readPathPaintCalibration();
                            break;
                        }
                        else
                        {
                            logicProjectNew.readPathWindOn0();
                            break;
                        }
                    case 5:
                        logicProjectNew.readPathWindOff0();
                        break;
                    case 6:
                        logicProjectNew.readPathWindOn1();
                        break;
                    case 7:
                        logicProjectNew.readPathWindOff1();
                        break;
                    case 8:
                        logicProjectNew.readPathPaintCalibration();
                        break;
                    default:
                        break;
                    }
                }

                onPositionChanged:
                {
                    if(containsMouse === true && floatText.text !== "")
                    {
                        if(mouseX >= delegateRect.width - floatText.width)
                        {
                            floatText.x = delegateRect.width - floatText.width;
                        }
                        else
                        {
                            floatText.x = mouseX;
                        }

                        if(mouseY >= delegateRect.height - floatText.height)
                        {
                            floatText.y = delegateRect.height - floatText.height
                        }
                        else
                        {
                            floatText.y = mouseY;
                        }
                    }
                }

                onHoveredChanged:
                {
                    if(containsMouse === true && floatText.text !== "")
                    {
                        floatText.visible = true;
                    }
                    else
                    {
                        floatText.visible = false;
                    }
                }
            }
        }
    }

    Connections
    {
        target: logicProjectNew

        onPathBackgroundChanged:
        {
            listProject.model.get(0).imageSource = logicProjectNew.getPathBackground();
            listProject.model.get(0).floatContent = logicProjectNew.getPathBackground();
            listProject.model.get(0).imageSet = true;
        }

        onPathBackground2Changed:
        {
            listProject.model.get(1).imageSource = logicProjectNew.getPathBackground2();
            listProject.model.get(1).floatContent = logicProjectNew.getPathBackground2();
            listProject.model.get(1).imageSet = true;
        }

        onPathBackground3Changed:
        {
            listProject.model.get(2).imageSource = logicProjectNew.getPathBackground3();
            listProject.model.get(2).floatContent = logicProjectNew.getPathBackground3();
            listProject.model.get(2).imageSet = true;
        }

        onPathBackground4Changed:
        {
            listProject.model.get(3).imageSource = logicProjectNew.getPathBackground4();
            listProject.model.get(3).floatContent = logicProjectNew.getPathBackground4();
            listProject.model.get(3).imageSet = true;
        }

        onPathWindOn0Changed:
        {
            if(logicProjectNew.getProjectType() === 1)
            {
                listProject.model.get(4).imageSource = logicProjectNew.getPathWindOn0();
                listProject.model.get(4).imageSet = true;
                listProject.model.get(4).floatContent = logicProjectNew.getPathWindOn0() + "\n" + logicProjectNew.getPathWindOn0Mark();
            }
            else
            {
                listProject.model.get(2).imageSource = logicProjectNew.getPathWindOn0();
                listProject.model.get(2).imageSet = true;
                listProject.model.get(2).floatContent = logicProjectNew.getPathWindOn0() + "\n" + logicProjectNew.getPathWindOn0Mark();
            }
        }

        onPathWindOn0MarkChanged:
        {
            if(logicProjectNew.getProjectType() === 1)
            {
                listProject.model.get(4).markerSet = true;
                listProject.model.get(4).floatContent = logicProjectNew.getPathWindOn0() + "\n" + logicProjectNew.getPathWindOn0Mark();
            }
            else
            {
                listProject.model.get(2).markerSet = true;
                listProject.model.get(2).floatContent = logicProjectNew.getPathWindOn0() + "\n" + logicProjectNew.getPathWindOn0Mark();
            }
        }

        onPathWindOff0Changed:
        {
            if(logicProjectNew.getProjectType() === 1)
            {
                listProject.model.get(5).imageSource = logicProjectNew.getPathWindOff0();
                listProject.model.get(5).imageSet = true;
                listProject.model.get(5).floatContent = logicProjectNew.getPathWindOff0() + "\n" + logicProjectNew.getPathWindOff0Mark();
            }
            else
            {
                listProject.model.get(3).imageSource = logicProjectNew.getPathWindOff0();
                listProject.model.get(3).imageSet = true;
                listProject.model.get(3).floatContent = logicProjectNew.getPathWindOff0() + "\n" + logicProjectNew.getPathWindOff0Mark();
            }
        }

        onPathWindOff0MarkChanged:
        {
            if(logicProjectNew.getProjectType() === 1)
            {
                listProject.model.get(5).markerSet = true;
                listProject.model.get(5).floatContent = logicProjectNew.getPathWindOff0() + "\n" + logicProjectNew.getPathWindOff0Mark();
            }
            else
            {
                listProject.model.get(3).markerSet = true;
                listProject.model.get(3).floatContent = logicProjectNew.getPathWindOff0() + "\n" + logicProjectNew.getPathWindOff0Mark();
            }
        }

        onPathWindOn1Changed:
        {
            if(logicProjectNew.getProjectType() === 1)
            {
                listProject.model.get(6).imageSource = logicProjectNew.getPathWindOn1();
                listProject.model.get(6).imageSet = true;
                listProject.model.get(6).floatContent = logicProjectNew.getPathWindOn1() + "\n" + logicProjectNew.getPathWindOn1Mark();
            }
        }
        onPathWindOn1MarkChanged:
        {
            if(logicProjectNew.getProjectType() === 1)
            {
                listProject.model.get(6).markerSet = true;
                listProject.model.get(6).floatContent = logicProjectNew.getPathWindOn1() + "\n" + logicProjectNew.getPathWindOn1Mark();
            }
        }
        onPathWindOff1Changed:
        {
            if(logicProjectNew.getProjectType() === 1)
            {
                listProject.model.get(7).imageSource = logicProjectNew.getPathWindOff1();
                listProject.model.get(7).imageSet = true;
                listProject.model.get(7).floatContent = logicProjectNew.getPathWindOff1() + "\n" + logicProjectNew.getPathWindOff1Mark();
            }
        }
        onPathWindOff1MarkChanged:
        {
            if(logicProjectNew.getProjectType() === 1)
            {
                listProject.model.get(7).markerSet = true;
                listProject.model.get(7).floatContent = logicProjectNew.getPathWindOff1() + "\n" + logicProjectNew.getPathWindOff1Mark();
            }
        }

        onPathPainCalibrationChanged:
        {
            if(logicProjectNew.getProjectType() === 1)
            {
                listProject.model.get(8).floatContent = logicProjectNew.getPathPainCalibration();
                listProject.model.get(8).fileBaseName = logicProjectNew.getBaseName(logicProjectNew.getPathPainCalibration());
                listProject.model.get(8).fileSet = true;
            }
            else
            {
                listProject.model.get(4).floatContent = logicProjectNew.getPathPainCalibration();
                listProject.model.get(4).fileBaseName = logicProjectNew.getBaseName(logicProjectNew.getPathPainCalibration());
                listProject.model.get(4).fileSet = true;
            }
        }
    }

}
