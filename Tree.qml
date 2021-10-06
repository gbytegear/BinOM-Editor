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
        visible: BinOM.selected_file_type == "serialized storage";
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
          icon.color: (BinOM.selected_file_name === modelData.name)?app_root.accent:app_root.text_color;
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
      width: text_content.width + 10 + height;
      x: 5 + modelData.depth * 10;

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

      function getColor(unselected_color = app_root.highlight_text_color, selected_color = app_root.accent_second) {
        return tree_view_root.selected_item ?(
                  (tree_view_root.selected_item.path === modelData.path)
                  ? selected_color
                  : unselected_color
               ): unselected_color;
      }

      RowLayout {
        anchors.fill: parent;
        id: element_content;
        Layout.alignment: Qt.AlignVCenter

        Item {
          Layout.fillHeight: true
          width: 10;

          Rectangle {
            height: parent.height;
            x: -modelData.depth*10
            width: modelData.depth*10 + 5;
            color: getColor(app_root.text_color);
          }

        }

        Label {
          color: getColor();
          font.pixelSize: 25;
          text: (modelData.type_class !== "primitive")?(modelData.is_open? "-" : "+"):"";
        }

        Image {
          id: node_icon;
          source: getIconUrl();
          height: 50;
          width: height;
          visible: true;
          ColorOverlay {
            anchors.fill: node_icon;
            source: node_icon;
            color: getColor();
          }
        }

        Label {
          id: text_content;
          color: getColor();
          text: `${modelData.key}: [${modelData.type}]${(typeof(modelData.value)=="undefined")?"("+modelData.element_count+")":" "+modelData.value}`;
        }

      }

      Glow {
        anchors.fill: element_content;
        radius: 12;
        samples: 25;
        visible: tree_view_root.selected_item ?(
                   (tree_view_root.selected_item.path === modelData.path)
                     ? true
                     : false
                   ): false;
        color: "#000000";
        source: element_content;
      }

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
      Layout.alignment: Qt.AlignVCenter

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

      ToolButton {
        icon.source: "qrc:/icons/icons/delete_forever_white_24dp.svg"
        visible: !!tree_view_root.selected_item;
      }

      Label {
        Layout.fillWidth: true;
        color: app_root.accent_second;
        text: tree_view_root.selected_item
              ? `Selected: [${tree_view_root.selected_item.type}] ${tree_view_root.selected_item.path}` : "";
      }
    }
  }
}




