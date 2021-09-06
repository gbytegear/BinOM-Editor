import QtQuick 2.12
import QtQuick.Controls 2.12

Page {
//    anchors.fill: parent
    Column {
        anchors.fill: parent;
        ToolButton {
            width: parent.width;
            icon.source: "qrc:/icons/icons/file_white_24dp.svg";
            text: "Create file";
        }

        ToolButton {
            width: parent.width;
            icon.source: "qrc:/icons/icons/folder_white_24dp.svg";
            text: "Open file";
            onClicked: file_dialog.visible = true;
        }

        ToolBar {
            width: parent.width;
            height: 25;
            Label {
                anchors.centerIn: parent;
                text: "History";
            }
        }

    }




}
