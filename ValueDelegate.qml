import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

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
          element_model.setProperty(index, "value", text-1+1);
      }
    }

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
} // ListView Buffer Array delegate
