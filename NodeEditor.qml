import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.12
import QtQml.Models 2.15
import QtQuick.Layouts 1.12

import BinOM 1.0


/* Input list
 * var_type_input
 * add_type
 *
*/


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
    if(element_model.count) element_model.clear();
  }

  function push(edit_index) {
    let form_data = {
      mode,
      type: var_type_input.currentText,
      data: element_model.getData(),
      edit_index,
    };
    if(mode == "add")
      form_data["add_type"] = add_type.checkedButton.text == "By index"
          ? (insert_index_input.text-1+1)
          : add_type.checkedButton.text;

    console.log("Push form stack struct: ", JSON.stringify(form_data, null, 4));
    edit_stack.push(form_data);

    editor_win.mode = "element";
    var_type_input.currentIndex = var_type_input.indexOfValue(form_data.data[edit_index].type);
    element_model.setData(form_data.data[edit_index].value);
  }



  function popAndSave() {
    let form_data = edit_stack.pop();
    let current_data = element_model.getData();
    mode = form_data.mode;
    var_type_input.currentIndex = var_type_input.indexOfValue(form_data.type);
    form_data.data[form_data.edit_index].value = current_data;
    element_model.setData(form_data.data);
  }

  function pop() {
    let form_data = edit_stack.pop();
    mode = form_data.mode;
    var_type_input.currentIndex = var_type_input.indexOfValue(form_data.type);
    element_model.setData(form_data.data);
  }


  header: ToolBar { // HEADER
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
              :(editor_win.mode === "element")
              ? "Edit element"
              : "";
      }


      ToolButton {
        Layout.alignment: Qt.AlignRight;
        icon.source: mode === "element"? "qrc:/icons/icons/reply_white_24dp.svg" : "qrc:/icons/icons/cancel_white_24dp.svg";
        onClicked: if(mode === "element") pop(); else close();
      }
    }
  } // HEADER


  SplitView {
    id: editor_form;
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



      RowLayout { // Type input
        Label {text: "Node type: ";}
        ComboBox {
          Layout.fillWidth: true;
          id: var_type_input;
          enabled: mode !== "add" && mode !== "element" && (node_properties?!node_properties.is_value_ref:true);
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
          onCurrentIndexChanged: if(element_model.count) element_model.clear();
        }
      }  // Type input



      ColumnLayout { // Add type input
        visible: mode === "add" && var_type_input.currentIndex != 9;
        ButtonGroup { id: add_type; }
        RadioButton {
          text: "Push front";
          ButtonGroup.group: add_type;
        }
        RowLayout {
          RadioButton {
            id: is_index_insert;
            ButtonGroup.group: add_type;
            text: "By index";
            onCheckedChanged: if(!checked) insert_index_input.text = "";
          }
          TextField {
            Layout.fillWidth: true;
            id: insert_index_input;
            enabled: is_index_insert.checked;
            placeholderText: `Insert by index 0...${node_properties? node_properties.element_count : 0}`;
            validator: IntValidator {
              bottom: 0;
              top: node_properties? node_properties.element_count : 0;
            }
          }
        }
        RadioButton {
          ButtonGroup.group: add_type;
          checked: true
          text: "Push back";
        }

      } // Add type input





      RowLayout { // Buffer Array type input
        id: input_type_input_row;
        visible: false;
        Label {text: "Input type: ";}
        ComboBox {
          Layout.fillWidth: true;
          id: input_type_input;
          model: ["Array", "String", "Load from file"];
        }
      } // Buffer Array type input





      RowLayout { // Value input
        id: value_input_row;
        visible: false;
        Label {text: "Value: ";}
        TextField {
          Layout.fillWidth: true;
          id: value_input;
          validator: value_validator;
        }
      } // Value input




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
        onClicked: {
          if(mode === "element") popAndSave();
          console.log("\n", JSON.stringify(element_model.getData(), null, 4));
        }
      }

      Button {
        Layout.fillWidth: true;
        text: "Create first element";
        visible: (!element_model.count && (var_type_input.currentIndex >= 4 && var_type_input.currentIndex <= 7)) ||
                 (!element_model.count && (var_type_input.currentIndex >= 8 && var_type_input.currentIndex <= 9));
        onClicked: element_model.addElement(0);
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
      PropertyChanges {target: editor_win; list_element_proto: Object({value: 0})}
      PropertyChanges {target: input_type_input_row; visible: true;}
      PropertyChanges {target: input_type_input_row; visible: true;}
      PropertyChanges {target: element_list; visible: true; model: element_model; delegate: value_element;}
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
      PropertyChanges {target: editor_win; list_element_proto: Object({value: 0})}
      PropertyChanges {target: element_list; visible: true; model: element_model; delegate: value_element}
      PropertyChanges {target: value_validator; bottom: -32768; top: 65535;}
      PropertyChanges {target: value_input; placeholderText: "-32768...65535"}
    },

    State {
      name: "dword_array_state";
      when: var_type_input.currentIndex == 6;
      PropertyChanges {target: editor_win; list_element_proto: Object({value: 0})}
      PropertyChanges {target: element_list; visible: true; model: element_model; delegate: value_element}
      PropertyChanges {target: value_validator; bottom: -2147483648; top: 4294967295;}
      PropertyChanges {target: value_input; placeholderText: "-2147483648...4294967295"}
    },

    State {
      name: "qword_array_state";
      when: var_type_input.currentIndex == 7;
      PropertyChanges {target: editor_win; list_element_proto: Object({value: 0})}
      PropertyChanges {target: element_list; visible: true; model: element_model; delegate: value_element}
      PropertyChanges {target: value_validator; bottom: -9223372036854775808; top: 18446744073709551615;}
      PropertyChanges {target: value_input; placeholderText: "-9223372036854775808...18446744073709551615"}
    },

    State {
      name: "array";
      when: var_type_input.currentIndex == 8;
      PropertyChanges {target: editor_win; list_element_proto: Object({type: "byte"}); dynamic_list_element_proto: Object({value: 0})}
      PropertyChanges {target: element_list; visible: true; delegate: variable_element; model: element_model;}
    },

    State {
      name: "object";
      when: var_type_input.currentIndex == 9;
      PropertyChanges {target: editor_win; list_element_proto: Object({type: "byte", key_type: "byte_array"}); dynamic_list_element_proto: Object({key: "", value: 0})}
      PropertyChanges {target: element_list; visible: true; delegate: variable_element; model: element_model;}
    }

  ]

  VariableDelegate{id: variable_element;}
  ValueDelegate{id: value_element;}

  IntValidator {
    id: value_validator;
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

  property var list_element_proto: null;
  property var dynamic_list_element_proto: null;
  property var dynamic_variables: [];
  ListModel {
    id: element_model;

    function claerAll() {
      if(dynamic_variables.length) dynamic_variables = new Array;
      clear();
    }

    function addElement(index) {
      console.log("Element proto;\nstatic:", JSON.stringify(list_element_proto), "\ndynamic:", JSON.stringify(dynamic_list_element_proto));
      if(dynamic_list_element_proto)
        dynamic_variables.splice(index, 0, Object.assign({}, dynamic_list_element_proto));
      if(list_element_proto)
        insert(index, Object.assign({}, list_element_proto));
    }

    function removeElement(index) {
      if(dynamic_list_element_proto)
        dynamic_variables.splice(index, 1);
    }

    function getData() {
      let data = new Array;
      for(let i = 0; i < count; ++i) {
        if(var_type_input.currentIndex >= 4 && var_type_input.currentIndex <= 7) {
          data.push(get(i).value);
        } else if(var_type_input.currentIndex == 8) {
          data.push({type: get(i).type, value: dynamic_variables[i].value});
        } else if(var_type_input.currentIndex == 9) {
          data.push({type: get(i).type, key_type: get(i).key_type, value: dynamic_variables[i].value, key: dynamic_variables[i].key_type});
        } else return null;
      }
      return data;
    }

    function setData(data) {
      claerAll();
      console.log("Set items in model:");
      data.forEach(
            (element, index) => {
              console.log(`${index} try_set:${JSON.stringify(element)}`);
              if(var_type_input.currentIndex >= 4 && var_type_input.currentIndex <= 7) {
                append(element);
                console.log(`${index} static:${JSON.stringify(get(index))}; dynamic: null`);
              } else if(var_type_input.currentIndex == 8) {
                dynamic_variables.push({value: element.value});
                append({type: element.type});
                console.log(`${index} static:${JSON.stringify(get(index))}; dynamic: ${JSON.stringify(dynamic_variables[index])}`);
              } else if(var_type_input.currentIndex == 9) {
                dynamic_variables.push({value: element.value, key: element.key});
                append({type: element.type, key_type: element.key_type});
                console.log(`${index} static:${JSON.stringify(get(index))}; dynamic: ${JSON.stringify(dynamic_variables[index])}`);
              }
            });
    }

  }

}
