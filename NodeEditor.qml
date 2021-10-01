import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import BinOM 1.0


Page{ // Editor
  id: editor_win;
  property var node_properties: null;
  property var mode: null;
  property var edit_stack: new Array;

  visible: false;
  anchors.fill: parent;
  function call(new_node_props, method) {
    node_properties = new_node_props;
    var_type_input.currentIndex = var_type_input.indexOfValue(node_properties.type);
    mode = method;
    visible = true;
  }

  function close() {
    node_properties = null;
    if(mode === "root")
      main_content.currentIndex = 0;
    mode = null;
    visible = false;
    if(value_element_model.count) value_element_model.clear();
    if(variable_element_model.count) variable_element_model.clear();
  }


  header: ToolBar {
    RowLayout {
      anchors.fill: parent;
      clip: true;

      Label {
        Layout.alignment: Qt.AlignLeft;
        Layout.leftMargin: 10;
        font.pixelSize: 19;
        text: (editor_win.mode === "add")
              ? `Add to node with path "${tree_view_root.selected_item.path}"`
              :(editor_win.mode === "edit")
              ? `Edit node with path "${tree_view_root.selected_item.path}"`
              :(editor_win.mode === "root")
              ? "Edit root node of new file"
              : "";
      }


      ToolButton {
        Layout.alignment: Qt.AlignRight;
        icon.source: "qrc:/icons/icons/cancel_white_24dp.svg"
        onClicked: close();
      }
    }
  } // HEADER


  SplitView {
    anchors{
      top: parent.top;
      bottom: parent.bottom;
      horizontalCenter: parent.horizontalCenter;
    }
    width: parent.width > 720? 720 : parent.width;
    anchors.rightMargin: anchors.leftMargin;
    orientation: Qt.Vertical;

    ColumnLayout {
      anchors {
        horizontalCenter: parent.horizontalCenter;
        margins: 10;
      }



      RowLayout {
        Label {text: "Node type: ";}
        ComboBox {
          Layout.fillWidth: true;
          id: var_type_input;
          enabled: mode !== "add" && !node_properties.is_value_ref;
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
            if(value_element_model.count) value_element_model.clear();
            if(variable_element_model.count) variable_element_model.clear();
          }
        }
      }



      ColumnLayout {
        visible: mode === "add" && var_type_input.currentIndex != 9;
        RadioButton {
          text: "Push front";
        }
        RadioButton {
          Layout.fillWidth: true
          TextField {
            anchors.right: parent.right;
            anchors.left: parent.left;
            anchors.leftMargin: 50;
//            x: 50;
            id: insert_index_input;
            enabled: parent.checked;
            placeholderText: `Insert by index 0...${node_properties? node_properties.element_count : 0}`;
            validator: IntValidator {
              bottom: 0;
              top: node_properties? node_properties.element_count : 0;
            }
          }
        }
        RadioButton {
          checked: true
          text: "Push back";
        }

      }





      RowLayout {
        id: input_type_input_row;
        visible: false;
        Label {text: "Input type: ";}
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
        onClicked: console.log(JSON.stringify(value_element_model.getData()));
      }




      Button {
        Layout.fillWidth: true;
        text: "Create first element";
        visible: (!value_element_model.count && (var_type_input.currentIndex >= 4 && var_type_input.currentIndex <= 7)) ||
                 (!variable_element_model.count && (var_type_input.currentIndex >= 8 && var_type_input.currentIndex <= 9));
        onClicked: {
          switch(var_type_input.currentIndex) {
          case 4:
          case 5:
          case 6:
          case 7:
            value_element_model.insert(0, {value:0});
          break;
          case 8:
            variable_element_model.insert(0, {type: "byte", value: []});
          break;
          case 9:
            variable_element_model.insert(0, {type: "byte", key_type: "byte", key: null, value: []});
          break;
          }
        }
      }
    } // ColumnLayout

    ListView {
      clip: true;
      id: element_list;
      visible: false;
    }

  } // SplitView




  states: [

    State {
      name: "byte_state"
      when: var_type_input.currentIndex == 0;
      PropertyChanges {target: value_input_row; visible: true;}
      PropertyChanges {target: confirm_button; enabled: !!value_input.text}
      PropertyChanges {target: value_validator; bottom: -128; top: 255;}
      PropertyChanges {target: value_input; placeholderText: "-128...255"}
    },

    State {
      name: "word_state"
      when: var_type_input.currentIndex == 1;
      PropertyChanges {target: value_input_row; visible: true;}
      PropertyChanges {target: confirm_button; enabled: !!value_input.text}
      PropertyChanges {target: value_validator; bottom: -32768; top: 65535;}
      PropertyChanges {target: value_input; placeholderText: "-32768...65535"}
    },

    State {
      name: "dword_state"
      when: var_type_input.currentIndex == 2;
      PropertyChanges {target: value_input_row; visible: true;}
      PropertyChanges {target: confirm_button; enabled: !!value_input.text}
      PropertyChanges {target: value_validator; bottom: -2147483648; top: 4294967295;}
      PropertyChanges {target: value_input; placeholderText: "-2147483648...4294967295"}
    },

    State {
      name: "qword_state"
      when: var_type_input.currentIndex == 3;
      PropertyChanges {target: value_input_row; visible: true;}
      PropertyChanges {target: confirm_button; enabled: !!value_input.text}
      PropertyChanges {target: value_validator; bottom: -9223372036854775808; top: 18446744073709551615;}
      PropertyChanges {target: value_input; placeholderText: "-9223372036854775808...18446744073709551615"}
    },

    State {
      name: "byte_array_array_edit_state";
      when: var_type_input.currentIndex == 4 && input_type_input.currentIndex == 0;
      PropertyChanges {target: input_type_input_row; visible: true;}
      PropertyChanges {target: input_type_input_row; visible: true;}
      PropertyChanges {target: element_list; visible: true; model: value_element_model; delegate: value_element}
    },

    State {
      name: "byte_array_string_edit_state";
      when: var_type_input.currentIndex == 4 && input_type_input.currentIndex == 1;
      PropertyChanges {target: input_type_input_row; visible: true;}
      PropertyChanges {target: element_list; visible: false;}
      PropertyChanges {target: value_input_row; visible: true;}
      PropertyChanges {target: value_input; validator: null; placeholderText: "Enter string...";}
    },

    State {
      name: "byte_array_load_file_state";
      when: var_type_input.currentIndex == 4 && input_type_input.currentIndex == 2;
      PropertyChanges {target: input_type_input_row; visible: true;}
      PropertyChanges {target: open_load_dialog_button; visible: true;}
    },

    State {
      name: "word_array_state";
      when: var_type_input.currentIndex == 5;
      PropertyChanges {target: element_list; visible: true; model: value_element_model; delegate: value_element}
      PropertyChanges {target: value_validator; bottom: -32768; top: 65535;}
      PropertyChanges {target: value_input; placeholderText: "-32768...65535"}
    },

    State {
      name: "dword_array_state";
      when: var_type_input.currentIndex == 6;
      PropertyChanges {target: element_list; visible: true; model: value_element_model; delegate: value_element}
      PropertyChanges {target: value_validator; bottom: -2147483648; top: 4294967295;}
      PropertyChanges {target: value_input; placeholderText: "-2147483648...4294967295"}
    },

    State {
      name: "qword_array_state";
      when: var_type_input.currentIndex == 7;
      PropertyChanges {target: element_list; visible: true; model: value_element_model; delegate: value_element}
      PropertyChanges {target: value_validator; bottom: -9223372036854775808; top: 18446744073709551615;}
      PropertyChanges {target: value_input; placeholderText: "-9223372036854775808...18446744073709551615"}
    },

    State {
      name: "array";
      when: var_type_input.currentIndex == 8;
      PropertyChanges {target: element_list; visible: true; delegate: variable_element; model: variable_element_model;}
    },

    State {
      name: "object";
      when: var_type_input.currentIndex == 9;
      PropertyChanges {target: element_list; visible: true; delegate: variable_element; model: variable_element_model;}
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
        onTextChanged: {
          if(validator)
            value = text-1+1;
        }
      }

      ToolButton {
        icon.source: "qrc:/icons/icons/add_circle_white_24dp.svg";
        onClicked: {
          value_element_model.insert(index + 1, {value:0});
        }
      }

      ToolButton {
        text: "Add first";
        visible: !index;
        onClicked: {
          value_element_model.insert(0, {value:0});
        }
      }

      ToolButton {
        icon.source: "qrc:/icons/icons/cancel_white_24dp.svg";
        onClicked: {
          value_element_model.remove(index);
        }
      }

    }
  } // ListView Buffer Array delegate


  Component {
    id: variable_element;

    ColumnLayout {
      width: parent.width;
      RowLayout { // Key
        width: parent.width;
        visible: var_type_input.currentIndex == 9;
        Label {text: "Key: "}
        ComboBox {
          id: key_type_input;
          Layout.fillWidth: true;
          model: [
            "byte_array",
            "word_array",
            "dword_array",
            "qword_array"
          ];
//          onCurrentIndexChanged: {
//            type = currentText;
//          }
        }
      } // Key

      RowLayout { // Value
        width: parent.width;

        Component.onCompleted: {
          value_type_input.currentIndex = value_type_input.indexOfValue(type);
        }

        Label {text: "Value: "}

        ComboBox {
          id: value_type_input;
          Layout.fillWidth: true;
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
            type = currentText;
          }
        }

        TextField {
          id: value_input
          Layout.fillWidth: true;
          visible: false;
          validator: IntValidator {
            id: element_value_validator;
          }
        }

        ToolButton {
          id: edit_button;
          icon.source: "qrc:/icons/icons/edit_white_24dp.svg";
        }

        states: [
          State {
            name: "byte_state"
            when: value_type_input.currentIndex == 0;
            PropertyChanges {target: edit_button; visible: false;}
            PropertyChanges {target: value_input; visible: true; placeholderText: "-128...255"}
            PropertyChanges {target: element_value_validator; bottom: -128; top: 255;}
          },

          State {
            name: "word_state"
            when: value_type_input.currentIndex == 1;
            PropertyChanges {target: edit_button; visible: false;}
            PropertyChanges {target: value_input; visible: true; placeholderText: "-32768...65535"}
            PropertyChanges {target: element_value_validator; bottom: -32768; top: 65535;}
          },

          State {
            name: "dword_state"
            when: value_type_input.currentIndex == 2;
            PropertyChanges {target: edit_button; visible: false;}
            PropertyChanges {target: value_input; visible: true; placeholderText: "-2147483648...4294967295"}
            PropertyChanges {target: element_value_validator; bottom: -2147483648; top: 4294967295;}
          },

          State {
            name: "qword_state"
            when: value_type_input.currentIndex == 3;
            PropertyChanges {target: edit_button; visible: false;}
            PropertyChanges {target: value_input; visible: true; placeholderText: "-9223372036854775808...18446744073709551615"}
            PropertyChanges {target: element_value_validator; bottom: -9223372036854775808; top: 18446744073709551615;}
          }
        ]

      } // Value


      RowLayout {
        anchors.horizontalCenter: parent.horizontalCenter;
        ToolButton {
          icon.source: "qrc:/icons/icons/add_circle_white_24dp.svg";
          onClicked: {
            variable_element_model.insert(index + 1, {type: "byte", value: []});
          }
        }

        ToolButton {
          text: "Add first";
          visible: !index;
          onClicked: {
            variable_element_model.insert(0, {type: "byte", value: []});
          }
        }

        ToolButton {
          icon.source: "qrc:/icons/icons/cancel_white_24dp.svg";
          onClicked: {
            variable_element_model.remove(index);
          }
        }
      }


    }
  }

  function toPrimitiveType() {
    switch(var_type_input.currentIndex) {
    case 4: return "byte";
    case 5: return "word";
    case 6: return "dword";
    case 7: return "qword";
    default: return "invalid_type";
    }
  }


  ListModel {
    id: value_element_model;
    function getData() {
      let data = new Array;
      for(let i = 0; i < count; ++i) {
        let list_element = get(i);
        let data_element = list_element.value;
        data.push(data_element);
      }
      return data;
    }

    function setData(data) {

    }
  }

  ListModel {
    id: variable_element_model;
    function getData() {
      let data = new Array;
      for(let i = 0; i < count; ++i) {
        let list_element = get(i);
        let data_element = new Object;
        for(let name in list_element)
          data_element[name] = list_element[name];
        data.push(data_element);
      }
      return data;
    }

    function setData(data) {

    }
  }

}
