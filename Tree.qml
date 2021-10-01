import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12

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
        anchors.top: parent.top;
        height: parent.height;
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

      function getIconUrl() {
        switch(modelData.type) {
        case "byte": return "qrc:/icons/icons/byte_white_24dp.svg";
        case "word": return "qrc:/icons/icons/word_white_24dp.svg";
        case "dword": return "qrc:/icons/icons/dword_white_24dp.svg";
        case "qword": return "qrc:/icons/icons/qword_white_24dp.svg";
        case "byte_array": return "qrc:/icons/icons/byte_array_white_24dp.svg";
        case "word_array": return "qrc:/icons/icons/word_array_white_24dp.svg";
        case "dword_array": return "qrc:/icons/icons/dword_array_white_24dp.svg";
        case "qword_array": return "qrc:/icons/icons/qword_array_white_24dp.svg";
        case "array": return "qrc:/icons/icons/data_array_white_24dp.svg";
        case "object": return "qrc:/icons/icons/data_object_white_24dp.svg";
        default: return "";
        }
      }

//      icon.source: getIconUrl();
//      icon.color: tree_view_root.selected_item?((tree_view_root.selected_item.path === modelData.path)?app_root.accent_second:app_root.highlight_text_color):app_root.highlight_text_color;

//      color: tree_view_root.selected_item?((tree_view_root.selected_item.path === modelData.path)?app_root.accent_second:app_root.highlight_text_color):app_root.highlight_text_color;
//      text: `${(modelData.type_class !== "primitive")?(modelData.is_open? "-" : "+"):""} ${modelData.key}: [${modelData.type}] ${(typeof(modelData.value)=="undefined")?"":modelData.value}`;

//      Rectangle {
//        height: parent.height;
//        width: 5;
//        x: -5;
//        color: tree_view_root.selected_item?((tree_view_root.selected_item.path === modelData.path)?app_root.accent_second:app_root.highlight_text_color):app_root.highlight_text_color;
//      }

      Row {
        anchors.fill: parent;

        Rectangle {
          height: parent.height;
          width: 5;
          x: -5;
          color: tree_view_root.selected_item?((tree_view_root.selected_item.path === modelData.path)?app_root.accent_second:app_root.highlight_text_color):app_root.highlight_text_color;
        }

        Image {
          id: node_icon;
          anchors.verticalCenter: parent.verticalCenter;
          source: getIconUrl();
          height: parent.height/2;
          width: height;
          visible: false;
        }

        ColorOverlay {
          anchors.fill: node_icon;
          source: node_icon;
          color: tree_view_root.selected_item?((tree_view_root.selected_item.path === modelData.path)?app_root.accent_second:app_root.highlight_text_color):app_root.highlight_text_color;
        }

        Label {
          id: text_content
          x: parent.height;
          anchors.centerIn: parent;
          color: tree_view_root.selected_item?((tree_view_root.selected_item.path === modelData.path)?app_root.accent_second:app_root.highlight_text_color):app_root.highlight_text_color;
          text: `${(modelData.type_class !== "primitive")?(modelData.is_open? "-" : "+"):""} ${modelData.key}: [${modelData.type}] ${(typeof(modelData.value)=="undefined")?"":modelData.value}`;
        }
      }
      width: text_content.width + 10 + height;
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

  NodeEditor { id: editor_win; }


  footer: ToolBar {
    RowLayout {
      anchors.fill: parent;
      clip: true;

      ToolButton {
        icon.source: "qrc:/icons/icons/edit_white_24dp.svg"
        visible: !!tree_view_root.selected_item;
        icon.color: editor_win.mode?((editor_win.mode === "edit")?app_root.accent:app_root.highlight_text_color):app_root.highlight_text_color;
        onClicked: editor_win.call(selected_item, "edit");
      }

      ToolButton {
        icon.source: "qrc:/icons/icons/add_circle_white_24dp.svg"
        visible: (!!tree_view_root.selected_item)? (tree_view_root.selected_item.type_class !== "primitive") : false;
        icon.color: editor_win.mode?((editor_win.mode === "add")?app_root.accent:app_root.highlight_text_color):app_root.highlight_text_color;
        onClicked: editor_win.call(selected_item, "add");
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




