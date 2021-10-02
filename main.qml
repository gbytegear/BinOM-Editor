import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Dialogs 1.3

import BinOM 1.0

Window {
  id: app_root;
  width: 640;
  height: 480;
  visible: true;
  title: qsTr("BinOM Manager");

  property color background: "#131a21";
  property color primary: "#0c1016";
  property color accent: "#602dd3";
  property color accent_second: "#5fd462";
  property color text_color: "#2e3746";
  property color highlight_text_color: "#ffffff";

  Material.theme: Material.Dark;
  Material.background: background;
  Material.primary: primary;
  Material.accent: accent;

  FileDialog {
    id: file_dialog;
    title: "Chose BinOM file";
    folder: shortcuts.home;
    nameFilters: [ "BinOM files (*.binom *.binomdb)", "All files (*)" ]

    onAccepted: {
      if(BinOM.openFile(file_dialog.fileUrl.toString().replace(/^(file:\/{2})/,"")))
        main_content.currentIndex = 1;
    }

    onRejected: {}
  }


  SwipeView {
    id: main_content;
    anchors {
      fill: parent;
      leftMargin: 50;
    }
    orientation: Qt.Vertical;
    interactive: false;

    Open {}
    Tree {id: binom_view}
    Settings {}
  }


  ToolBar {
    height: parent.height;
    width: 50;
    Column {
      anchors.fill: parent;

      ToolButton {
        width: 50;
        icon.source: "qrc:/icons/icons/folder_white_24dp.svg";
        icon.color: main_content.currentIndex == 0? app_root.accent : app_root.text_color;
        onClicked: main_content.currentIndex = 0;
      }

      ToolButton {
        width: 50;
        icon.source: "qrc:/icons/icons/account_tree_white_24dp.svg";
        icon.color: main_content.currentIndex == 1? app_root.accent : app_root.text_color;
        onClicked: main_content.currentIndex = 1;
      }

      ToolButton {
        width: 50;
        icon.source: "qrc:/icons/icons/settings_white_24dp.svg";
        icon.color: main_content.currentIndex == 2? app_root.accent : app_root.text_color;
        onClicked: main_content.currentIndex = 2;
      }
    }
  }


}
