import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0

import BinOM 1.0


Page {
  id: tree_view_root;
  property var selected_item: null;
  property var selected_container: null;
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

        delegate: ToolButton {
          icon.source: (modelData.type === "file storage")
                       ? "qrc:/icons/icons/storage_white_24dp.svg"
                       : "qrc:/icons/icons/file_white_24dp.svg";
          icon.color: tree_view_root.selected_container?((tree_view_root.selected_container.name === modelData.name)?"#669bd1":"#FFFFFF"):"#FFFFFF";
          text: modelData.name;
          onClicked: {
            tree_view_root.selected_item = null;
            editor_win.visible = false;
            BinOM.selectFile(modelData.name);
            tree_view_root.selected_container = modelData;
          }
        }
      }
    }
  }

  ListView {
    id: tree_view;
    anchors.fill: parent;
    model: BinOM.tree_model;
    delegate: ToolButton {

      Rectangle {
        height: parent.height;
        width: 5;
        x: -5;
        color: tree_view_root.selected_item?((tree_view_root.selected_item.path === modelData.path)?"#669bd1":"#FFFFFF"):"#FFFFFF";
      }

      Label {
        id: text_content
        anchors.centerIn: parent;
        color: tree_view_root.selected_item?((tree_view_root.selected_item.path === modelData.path)?"#669bd1":"#FFFFFF"):"#FFFFFF";
        text: `${(modelData.type_class !== "primitive")?(modelData.is_open? "-" : "+"):""} ${modelData.key}: [${modelData.type}] ${(typeof(modelData.value)=="undefined")?"":modelData.value}`;
      }
      width: text_content.width + 10;
      x: 5 + modelData.depth * 10;
      onClicked: tree_view_root.selected_item = modelData;
      onDoubleClicked: if(modelData.type_class !== "primitive") BinOM.switchNodeVisibility(modelData.path);
    }
  }

  Page{ // Editor
    id: editor_win;
    property var add_mode: false
    visible: false;
    anchors {
      fill: parent;
//      margins: 10;
    }

    header: ToolBar {
      RowLayout {
        anchors.fill: parent;
        clip: true;

        Label {
          Layout.alignment: Qt.AlignLeft;
          Layout.leftMargin: 10;
          font.pixelSize: 19;
          text: editor_win.add_mode? `Add to node with path "${tree_view_root.selected_item.path}"` : `Edit node with path "${tree_view_root.selected_item.path}"`;
        }


        ToolButton {
          Layout.alignment: Qt.AlignRight;
          icon.source: "qrc:/icons/icons/cancel_white_24dp.svg"
          onClicked: editor_win.visible = false;
        }
      }
    }

  }

  DropShadow {
    anchors.fill: editor_win;
    visible: editor_win.visible;
    horizontalOffset: 3;
    verticalOffset: 3;
    radius: 8.0;
    samples: 17;
    color: "#000000";
    source: editor_win;
  }


  footer: ToolBar {
    RowLayout {
      anchors.fill: parent;
      clip: true;

      Button {
        text: "Edit";
        visible: !!tree_view_root.selected_item;
        onClicked: {
          editor_win.add_mode = false;
          editor_win.visible = true;
        }
      }

      Button {
        text: "Add";
        visible: (!!tree_view_root.selected_item)? (tree_view_root.selected_item.type_class !== "primitive") : false;
        onClicked: {
          editor_win.add_mode = true;
          editor_win.visible = true;
        }
      }

      Label {
        Layout.fillWidth: true;
        anchors.verticalCenter: parent.verticalCenter;
        text: tree_view_root.selected_item
              ? `Selected: ${tree_view_root.selected_item.path}` : "";
      }
    }
  }
}




