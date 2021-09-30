import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import BinOM 1.0


Page {
  id: tree_view_root;
  property alias editor: editor_win;
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
          icon.color: tree_view_root.selected_container?((tree_view_root.selected_container.name === modelData.name)?app_root.accent:app_root.text_color):app_root.text_color;
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
//    onModelChanged: console.log(JSON.stringify(model, null, 4));
    delegate: ToolButton {

      Rectangle {
        height: parent.height;
        width: 5;
        x: -5;
        color: tree_view_root.selected_item?((tree_view_root.selected_item.path === modelData.path)?app_root.accent_second:app_root.highlight_text_color):app_root.highlight_text_color;
      }

      Label {
        id: text_content
        anchors.centerIn: parent;
        color: tree_view_root.selected_item?((tree_view_root.selected_item.path === modelData.path)?app_root.accent_second:app_root.highlight_text_color):app_root.highlight_text_color;
        text: `${(modelData.type_class !== "primitive")?(modelData.is_open? "-" : "+"):""} ${modelData.key}: [${modelData.type}] ${(typeof(modelData.value)=="undefined")?"":modelData.value}`;
      }
      width: text_content.width + 10;
      x: 5 + modelData.depth * 10;
      onClicked: tree_view_root.selected_item = modelData;
      onDoubleClicked: if(modelData.type_class !== "primitive") BinOM.switchNodeVisibility(modelData.path);
    }
  }

  Label {
    anchors.centerIn: parent;
    visible: !tree_view.model.length;
    font.pixelSize: 25;
//    color: app_root.accent;
    text: "Select or create file in \"Open\" tab";
    opacity: .125;
  }

  NodeEditor {
    id: editor_win;
    node_properties: selected_item;
  }


  footer: ToolBar {
    RowLayout {
      anchors.fill: parent;
      clip: true;

      Button {
        text: "Edit";
        visible: !!tree_view_root.selected_item;
        onClicked: {
          editor_win.mode = "edit";
          editor_win.visible = true;
        }
      }

      Button {
        text: "Add";
        visible: (!!tree_view_root.selected_item)? (tree_view_root.selected_item.type_class !== "primitive") : false;
        onClicked: {
          editor_win.mode = "add";
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




