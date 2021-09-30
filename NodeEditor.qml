import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import BinOM 1.0


Page{ // Editor
  id: editor_win;
  property var node_properties: null;
  property var add_mode: false
  property var value: null;
  visible: false;
  anchors.fill: parent;
  onVisibleChanged: {
    if(visible) {
      var_type_input.currentIndex = var_type_input.indexOfValue(node_properties.type);
    }
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
  } // HEADER


  SplitView {
    anchors{
      top: parent.top;
      bottom: parent.bottom;
      horizontalCenter: parent.horizontalCenter;
    }
    width: parent.width > 640? 640 : parent.width;
    anchors.rightMargin: anchors.leftMargin;
    orientation: Qt.Vertical;

    ColumnLayout {
      anchors {
        horizontalCenter: parent.horizontalCenter;
        margins: 10;
      }

      //    anchors.centerIn: parent;



      RowLayout {
        Label {text: "Node type: ";}
        ComboBox {
          Layout.fillWidth: true;
          id: var_type_input;
          model: [
            "byte",
            "word",
            "dword",
            "qword",
            "byte_array",
            "word_array",
            "dword_array",
            "qword_array",
            "array",
            "object"
          ];
          onCurrentIndexChanged: {
            if(element_model.count) element_model.clear();
            switch(currentIndex) {
            case 4:
            case 0: // Byte
              value_validator.bottom = -128;
              value_validator.top = 255;
              value_input.placeholderText = "-128...255";
              break;
            case 5:
            case 1: // Word
              value_validator.bottom = -32768;
              value_validator.top = 65535;
              value_input.placeholderText = "-32768...65535";
              break;
            case 6:
            case 2: // DWord
              value_validator.bottom = -2147483648;
              value_validator.top = 4294967295;
              value_input.placeholderText = "-2147483648...4294967295";
              break;
            case 7:
            case 3: // QWord
              value_validator.bottom = -9223372036854775808;
              value_validator.top = 18446744073709551615;
              value_input.placeholderText = "-9223372036854775808...18446744073709551615";
              break;
            }
          }
        }
      }





      RowLayout {
        id: input_type_input_row;
        visible: var_type_input.currentIndex == 4;
        Label {text: "Value: ";}
        ComboBox {
          Layout.fillWidth: true;
          id: input_type_input;
          model: ["Array", "String", "Load from file"];
        }
      }





      RowLayout {
        id: value_input_row;
        visible: false;
        Label {text: "Value: ";}
        TextField {
          Layout.fillWidth: true;
          id: value_input;
          validator: value_validator;
        }
      }




      Button {
        Layout.fillWidth: true;
        visible: false;
        id: open_load_dialog_button;
        text: "Load file";
      }





      Button {
        Layout.fillWidth: true;
        id: confirm_button;
        text: "Confirm";
      }




      Button {
        Layout.fillWidth: true;
        text: "Create first element";
        visible: !element_model.count && element_list.visible;
        onClicked: {
          element_model.insert(0, {value:0});
        }
      }
    } // ColumnLayout

    ListView {
      clip: true;
      id: element_list;
      visible: false;
      model: element_model;
      delegate: value_element;
    }

  } // SplitView




  states: [
    State {
      name: "primitive_state";
      when: (var_type_input.currentIndex >= 0 && var_type_input.currentIndex <= 3);
      PropertyChanges {target: value_input_row; visible: true;}
      PropertyChanges {target: confirm_button; enabled: !!value_input.text}
    },

    State {
      name: "buffer_array_state";
      when: (var_type_input.currentIndex > 4 && var_type_input.currentIndex <= 7);
      PropertyChanges {target: element_list; visible: true;}
    },

    State {
      name: "byte_array_array_state";
      when: (var_type_input.currentIndex == 4 && input_type_input.currentIndex == 0);
      PropertyChanges {target: element_list; visible: true;}
      PropertyChanges {target: value_input_row; visible: false;}
    },

    State {
      name: "byte_array_string_state";
      when: (var_type_input.currentIndex == 4 && input_type_input.currentIndex == 1);
      PropertyChanges {target: element_list; visible: false;}
      PropertyChanges {target: value_input_row; visible: true;}
      PropertyChanges {target: value_input; validator: null;}
      PropertyChanges {target: value_input; placeholderText: "Enter string...";}
    },

    State {
      name: "byte_array_load_state";
      when: (var_type_input.currentIndex == 4 && input_type_input.currentIndex == 2);
      PropertyChanges {target: open_load_dialog_button; visible: true;}
    }

  ]



  IntValidator {
    id: value_validator;
  }

  Component { // ListView Buffer Array delegate
    id: value_element;
    RowLayout {
      anchors.margins: 10;
      width: parent.width;
      Label {text: `[${index}] Value: `;}
      TextField {
        Layout.fillWidth: true;
        validator: value_validator;
        placeholderText: value_input.placeholderText;
        text: value;
        onTextChanged: {}
      }

      ToolButton {
        icon.source: "qrc:/icons/icons/add_circle_white_24dp.svg";
        onClicked: {
          element_model.insert(index + 1, {value:0});
        }
      }

      ToolButton {
        text: "Add first";
        visible: !index;
        onClicked: {
          element_model.insert(0, {value:0});
        }
      }

      ToolButton {
        icon.source: "qrc:/icons/icons/cancel_white_24dp.svg";
        onClicked: {
          element_model.remove(index);
        }
      }

    }
  } // ListView Buffer Array delegate


  ListModel {
    id: element_model;
  }

}
