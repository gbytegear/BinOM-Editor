import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Dialogs 1.3

import BinOM 1.0

Window {
    width: 640;
    height: 480;
    visible: true;
    title: qsTr("BinOM Manager");
    Material.theme: Material.Dark;
    Material.background: "#101018";
    Material.primary: "#303038";
    Material.accent: Material.BlueGrey;

    FileDialog {
        id: file_dialog;
        title: "Chose BinOM file";
        folder: shortcuts.home;
        nameFilters: [ "BinOM files (*.binom *.binomdb)", "All files (*)" ]

        onAccepted: {
          BinOM.openFile(file_dialog.fileUrl.toString().replace(/^(file:\/{3})/,""));
        }

        onRejected: {

        }
    }


    SwipeView {
        id: main_content;
        anchors {
            fill: parent;
            leftMargin: 50;
        }
        orientation: Qt.Vertical;
        interactive: false;

        Open {}
        Tree {}
        Settings {}
    }


    ToolBar {
        height: parent.height;
        width: 50;
        Column {
            anchors.fill: parent;

            ToolButton {
                width: 50;
                icon.source: "qrc:/icons/icons/folder_white_24dp.svg";
                onClicked: main_content.currentIndex = 0;
            }

            ToolButton {
                width: 50;
                icon.source: "qrc:/icons/icons/account_tree_white_24dp.svg";
                onClicked: main_content.currentIndex = 1;
            }

            ToolButton {
                width: 50;
                icon.source: "qrc:/icons/icons/settings_white_24dp.svg";
                onClicked: main_content.currentIndex = 2;
            }
        }
    }


}
