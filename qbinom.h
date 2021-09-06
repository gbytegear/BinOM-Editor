#ifndef QBINOM_H
#define QBINOM_H

#include <QMap>
#include <QObject>
#include <QVariant>

#undef foreach
#include "binom/includes/binom.h"
#include <optional>


struct BinOMNode {
  BinOMNode* parent = nullptr;
  binom::Path path;
  binom::VarType type = binom::VarType::end;
  bool is_open = false;
  QList<BinOMNode> childs;

  QVariantMap getProps() {
    QVariantMap props;
    props.insert("type", binom::toTypeString(type));
    props.insert("path", QString::fromStdString(path.toString()));
    if(is_open) {
      QVariantList child_list;
      for(BinOMNode& node : childs)
        child_list.push_back(node.getProps());
      props.insert("childs", child_list);
    }
    return props;
  }

  void clear() {childs.clear();}

  BinOMNode() = default;
  BinOMNode(binom::NodeVisitorBase& node, binom::Path _path, BinOMNode* _parent = nullptr)
    : parent(_parent), path(_path), type(node.getType()) {
    binom::ui64 index = 0;
    switch (node.getVisitorType()) {
      case binom::VisitorType::ram_storage_visitor:
        for(auto child : node.toRAMVisitor()) {

          binom::Path child_path = path;
          if(std::optional<binom::BufferArray> name = child.getName(); name)
            child_path += *name;
          else
            child_path += index++;

          childs.push_back(BinOMNode(child, child_path, this));
        }
      break;

      case binom::VisitorType::file_storage_visitor:
        for(auto child : node.toFileVisitor()) {

          binom::Path child_path = path;
          if(std::optional<binom::BufferArray> name = child.getName(); name)
            child_path += *name;
          else
            child_path += index++;

          childs.push_back(BinOMNode(child, child_path, this));
        }
      break;

    }
  }
};

struct BinOMFile {
  binom::FileType file_type;
  union File {
    struct SerializedStorage {
      binom::SerializedStorage storage;
      binom::Variable var;
      binom::NodeVisitor root_node;
    } serialized;
    struct FileStorage {
      binom::FileStorage storage;
      binom::FileNodeVisitor root_node;
    } file;
  } storage_union;

  BinOMNode tree_root;

  std::unique_ptr<binom::NodeVisitorBase> getRoot() {
    switch (file_type) {
      default: return nullptr;
      case binom::FileType::file_storage:
      return std::make_unique<binom::NodeVisitorBase>(new binom::FileNodeVisitor(storage_union.file.root_node));
      case binom::FileType::serialized_file_storage:
      return std::make_unique<binom::NodeVisitorBase>(new binom::NodeVisitor(storage_union.serialized.root_node));
    }
  }

  void buildTree() {
    tree_root.clear();
    std::unique_ptr<binom::NodeVisitorBase> root = getRoot();
  }

  // TODO consturctor, QVariantMap getTree

};

class QBinOM : QObject {
  QMap<QString, BinOMFile> files;
  BinOMFile* selected = nullptr;

public:
  QBinOM() : QObject(nullptr) {}

  Q_INVOKABLE bool openFile(QString file_path) {
    fs::path path = file_path.toStdString();
    if(!fs::exists(path)) return false;
    binom::FileType file_type = binom::checkFileType(file_path.toStdString());
    switch (file_type) {
      case binom::FileType::undefined_file: return false;
      case binom::FileType::file_storage:
        // TODO
      return true;
      case binom::FileType::serialized_file_storage:
        // TODO
      return true;
    }
  }
  Q_INVOKABLE void closeFile(QString file_name);
};

#endif // QBINOM_H
