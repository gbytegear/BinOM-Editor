import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12

import BinOM 1.0

Page {

  readonly property var binom_types: [
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

  id: editor_root;
  anchors.fill: parent;
  visible: false;

  function call() {
    visible = true;
  }

  function close() {
    visible = false;
  }

  header: ToolBar {
    RowLayout {
      anchors.fill: parent;
      Label {
        Layout.alignment: Qt.AlignVCenter;
        text: "Node editor";
      }

      ToolButton {
        Layout.alignment: Qt.AlignRight;
        icon.source: "qrc:/icons/icons/cancel_white_24dp.svg";
        onClicked: editor_root.close();
      }
    }
  }


  SplitView {
    anchors.fill: parent;

    ColumnLayout {
      RowLayout { Label{text: "Node type: "} ComboBox {model: binom_types;} }
      RowLayout { Label{text: "Input type: "} ComboBox {model: ["array", "string", "file"];}}


    }

    ListView {
      id: node_element_list;
      model: [{type: "byte", value: 10}];
      delegate: RowLayout {
        anchors {right: parent.right; left: parent.left;}
        ComboBox {
          model: binom_types;
          currentIndex: indexOfValue(modelData.type)
          onCurrentTextChanged: node_element_list.model[index].type = currentText;
        }
        TextField {
          Layout.fillWidth: true;
          text: modelData.value;
        }
        Button {
          text: "Print"
          onClicked: console.log(JSON.stringify(node_element_list.model, null, 4));
        }

      }
    }


  }


}
