import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import BinOM 1.0

Page {
  property var selected_item: null;
  header: ToolBar {
    RowLayout {
      anchors.fill: parent;
      clip: true;

      ToolButton {
        visible: BinOM.is_file_selected;
        icon.source: "qrc:/icons/icons/save_white_24dp.svg";
      }

      ListView {
        id: open_files
        Layout.fillWidth: true;
        height: parent.height;
        anchors.top: parent.top;
        orientation: ListView.Horizontal;
        model: BinOM.open_files;
//        required model;

        delegate: ToolButton {
//          required property var modelData;
          icon.source: (modelData.type === "file storage")
                       ? "qrc:/icons/icons/storage_white_24dp.svg"
                       : "qrc:/icons/icons/file_white_24dp.svg";
          text: modelData.name;
          onClicked: BinOM.selectFile(modelData.name);
        }
      }
    }
  }

  ListView {
    id: tree_view;
    anchors.fill: parent;
    model: BinOM.tree_model;
//    required model;
    delegate: ToolButton {
//      required property var modelData;

      Rectangle {
        height: parent.height;
        width: 5;
        x: -5;
        color: (selected_item == parent)?"#00FF00":"#FFFFFF";
      }

      Text {
        id: text_content
        anchors.centerIn: parent;
        color: (selected_item == parent)?"#00FF00":"#FFFFFF";
        text: `${(typeof(modelData.value) != "number")?(modelData.is_open? "-" : "+"):""} ${modelData.key}: [${modelData.type}] ${(typeof(modelData.value)=="undefined")?"":modelData.value}`;
      }
      width: text_content.width + 10;
      x: 5 + modelData.depth * 10;
      onClicked: selected_item = this;
      onDoubleClicked: if(typeof(modelData.value) != "number")BinOM.switchNodeVisibility(modelData.path);
    }
  }

  footer: ToolBar {
    Label {
      anchors.verticalCenter: parent.verticalCenter;
      text: "Selected: " + (selected_item?selected_item.modelData.path:"<Nothing>");
    }

//    height: 150;
  }
}




