import QtQuick 2.12
import QtQuick.Controls 2.12

Page {
    header: ToolBar {
        Row {
            anchors.fill: parent;

            ToolButton {
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

    footer: ToolBar {
        height: 150;
    }
}
