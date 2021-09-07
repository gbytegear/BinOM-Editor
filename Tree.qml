import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.12

import BinOM 1.0

Page {
    header: ToolBar {
        Row {
            anchors.fill: parent;

            ToolButton {
                visible: BinOM.is_file_selected;
                icon.source: "qrc:/icons/icons/save_white_24dp.svg";
            }

            Row {
                id: tabs;
                spacing: 5;
                height: parent.height;

                Button {
                    icon.source: "qrc:/icons/icons/storage_white_24dp.svg";
                    text: "Test.binomdb"
                }

                Button {
                    icon.source: "qrc:/icons/icons/file_white_24dp.svg";
                    text: "Test.binom"
                }
            }
        }
    }

    TreeView {
        anchors.fill: parent;
        model: BinOM.tree_model;
        itemDelegate: ToolButton {
            text: "Path: " + path + "Type: " + type;
        }


        TableViewColumn {
            role: "path";
            title: "Path";
        }

        TableViewColumn {
            role: "type";
            title: "Type";
        }
    }


    footer: ToolBar {
        height: 150;
    }
}
