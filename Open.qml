import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import BinOM 1.0

Page {
//    anchors.fill: parent
    Column {
        anchors.fill: parent;
        ToolButton {
            width: parent.width;
            icon.source: "qrc:/icons/icons/file_white_24dp.svg";
            text: "Create file";
            onClicked: {
              binom_view.editor.mode = "root";
              binom_view.editor.visible = true;
              main_content.currentIndex = 1;
            }
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

//        ListView {
//          id: history_list;
//          model: BinOM.files_history;
////          required model;
//          delegate: ToolButton {
////            required property var modelData;
//            text: modelData.path;
//          }
//        }


    }




}
