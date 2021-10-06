import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Component {
  id: variable_element;


  ColumnLayout {

    anchors {
      left: parent.left;
      right: parent.right;
    }

//    Component.onCompleted: {
//      console.log("Created element with index ", index);
//    }

    RowLayout { // Key
      width: parent.width;
      visible: var_type_input.currentIndex === 9;
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

        currentIndex: (typeof(key_type) == "string") ? indexOfValue(element_model.get(index).key_type): 0;

        Component.onCompleted: {
//          console.log(index, " key_type_input: called Component.onCompleted, type: ", element_model.get(index).key_type);
          currentIndex = indexOfValue(element_model.get(index).key_type)
        }

        onCurrentTextChanged: {
          if(dynamic_variables[index].key.length && currentText !== element_model.get(index).key_type)
            dynamic_variables[index].key = [];
          element_model.setProperty(index, "key_type", currentText);
        }
      }

      Label {
        id: key_view;
        Layout.fillWidth: true;
        text: (typeof(dynamic_variables[index].key) == "string")
              ? dynamic_variables[index].key
              : (typeof(dynamic_variables[index].key) == "object")
                ? JSON.stringify(dynamic_variables[index].key) : "";
      }

      ToolButton {
        id: edit_key_button;
        icon.source: "qrc:/icons/icons/edit_white_24dp.svg";
        onClicked: editor_win.push(index, true);
      }

    } // Key

    RowLayout { // Value
      width: parent.width;

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

        currentIndex: indexOfValue(element_model.get(index).type);

        Component.onCompleted: {
//          console.log(index, " value_type_input: called Component.onCompleted, type: ", element_model.get(index).type);
          currentIndex = indexOfValue(element_model.get(index).type)
        }

        onCurrentTextChanged: {
          if((currentIndex == 8 || currentIndex == 9) && typeof(dynamic_variables[index].value) == "number")
            dynamic_variables[index].value = [];
          element_model.setProperty(index, "type", currentText);
        }
      }

      TextField {
        id: value_input
        Layout.fillWidth: true;
        visible: false;
        validator: IntValidator {
          id: element_value_validator;
        }

        Component.onCompleted: {
//          console.log(index, " value_input: called Component.onCompleted");
          text = (typeof(dynamic_variables[index].value) == "number")? dynamic_variables[index].value : 0;
        }

        onTextChanged: {
//          console.log("value_input: called onTextChanged");
          if(value_type_input.currentIndex == 4)
            dynamic_variables[index].value = text;
          else if(value_type_input.currentIndex >= 0 && value_type_input.currentIndex <= 3)
            dynamic_variables[index].value = text-1+1;
        }
      }

      ToolButton {
        id: edit_button;
        icon.source: "qrc:/icons/icons/edit_white_24dp.svg";
        onClicked: editor_win.push(index);
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
      ToolButton {
        icon.source: "qrc:/icons/icons/add_circle_white_24dp.svg";
        onClicked: {
          element_model.addElement(index + 1);
        }
      }

      ToolButton {
        text: "Add first";
        visible: !index;
        onClicked: {
          element_model.addElement(0);
        }
      }

      ToolButton {
        icon.source: "qrc:/icons/icons/cancel_white_24dp.svg";
        onClicked: {
          element_model.removeElement(index);
        }
      }
    }


  }
}
