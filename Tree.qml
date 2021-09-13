import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import BinOM 1.0

Page {
  header: ToolBar {
    RowLayout {
      anchors.fill: parent;

      ToolButton {
        visible: BinOM.is_file_selected;
        icon.source: "qrc:/icons/icons/save_white_24dp.svg";
      }

      ListView {
        Layout.fillWidth: true;
        spacing: 5;
        orientation: ListView.Horizontal;
        model: BinOM.open_files;
        height: parent.height;
        required model;

        delegate: Button {
          required property var modelData;
          icon.source: (modelData.type === "file storage")
                       ? "qrc:/icons/icons/storage_white_24dp.svg"
                       : "qrc:/icons/icons/file_white_24dp.svg";
          text: modelData.name;
          onClicked: BinOM.selectFile(modelData.name);
        }
      }
    }
  }

  ListView {
    id: tree_view;
    anchors.fill: parent;
    model: BinOM.tree_model;
    required model;
    delegate: Button {
      required property var modelData;
      text: `${modelData.path}: [${modelData.type}] ${modelData.value}`;
    }
  }

//  TreeView {
//    id: tree_view;
//    anchors.fill: parent;
//    model: BinOM.is_file_selected? BinOM.getTreeModel() : undefined;
//    headerVisible: false;
//    itemDelegate: ToolButton {
//      text: "Path: " + path + "Type: " + type;
//    }


//    TableViewColumn {
//      role: "path";
//      title: "Path";
//    }

//    TableViewColumn {
//      role: "type";
//      title: "Type";
//    }


//    style: TreeViewStyle {
//      backgroundColor: "#101018";
//      textColor: "#FFFFFF";
//    }

//  }




  //------------------------------------------
  /*
  TreeView {

    id: viewTree
    anchors.fill: parent
    backgroundVisible: false
    model: treeViewModel;
    sortIndicatorVisible:true

    style:TreeViewStyle{
      backgroundColor: "#2B2F33"
      textColor: "#666666" // Это цвет маленького треугольника при установке слоев
      itemDelegate:Rectangle{

        color: "transparent" // Фон установлен как прозрачный, иначе при выделении строки появится половина выбранного цвета

        RowLayout {// Здесь это зависит от конкретных потребностей человека. Мне нужно реализовать это здесь. Изображение + текст

          Image {

            id: name

            resources: if (styleData.value != "по умолчанию")

                      {

                        if(styleData.selected)

                          "./Image/Split screen view /" + styleData.row + "_ press.png"

                        иначе, если (styleData.value == "1-сегментный")

                        "./Image/Split screen view /" + "1.png"

                        иначе, если (styleData.value == "4-сегментный")

                        "./Image/Split screen view /" + "2.png"

                        иначе, если (styleData.value == "9-сегментный")

                        "./Image/Split screen view /" + "3.png"

                        иначе, если (styleData.value == "16-сегментный")

                        "./Image/Split screen view /" + "4.png"

                      }

                      else {""}

          }

          Text {

            color: "#FEFEFE"

            elide: styleData.elideMode

            text: styleData.value

            padding: 10

            onTextChanged: {

              root.currentItemIndex (styleData.index) // Отправляем сигнал, этот сигнал определяется сам по себе

            }

          }

        }



      }

      rowDelegate:Rectangle{
        id: rowDel
        color: styleData.selected ? "#595F69" : root.color;
        height: 28
      }

    }



    TableViewColumn {// Добавить столбец

      title: "Default"

      role: "text"  //Имя этой роли должно точно совпадать с именем столбца, который вы определили в модели.，

      width: root.width

      resizable: false

    }



    headerVisible:false

    horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff

    onClicked: {

      root.currentChildIndex(viewTree.currentIndex)

    }

    onDoubleClicked: {

      root.currentChildIndex(viewTree.currentIndex)

    }

  }
  */
  //------------------------------------------


  footer: ToolBar {
    height: 150;
  }
}




