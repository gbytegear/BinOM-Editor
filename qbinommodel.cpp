#include "qbinommodel.h"
#include <QDebug>

QBinOMModel::operator QVariantList() {
  QVariantList data;
  switch (node->getVisitorType()) {
    case binom::VisitorType::ram_storage_visitor: {
      binom::NodeVisitor node = this->node->toRAMVisitor();
      bool is_open = opened.contains(QString::fromStdString(binom::Path().toString()));
      data.push_back(QVariantMap {
                       {"type", binom::toTypeString(node.getType())},
                       {"type_class", binom::toTypeString(node.getTypeClass())},
                       {"path", QString::fromStdString(binom::Path().toString())},
                       {"is_open", is_open},
                       {"key", "root"},
                       {"depth", QVariant(0)},
                       {"is_value_ref", node.isValueRef()},
                       {"element_count", qulonglong(node.getElementCount())},
                       {"value",
                        (node.getTypeClass() == binom::VarTypeClass::primitive)
                        ? qulonglong(node.getValue().asUi64())
                        : (node.getType() == binom::VarType::byte_array)
                          ? (node.getVariable().toBufferArray().isPrintable()
                            ? QString::fromStdString(node.getVariable().toBufferArray())
                            : QVariant())
                          : QVariant()}
                     });
      if(is_open)
        buildData(data, binom::Path(), node, 1);
    } break;
    case binom::VisitorType::file_storage_visitor: {
      binom::FileNodeVisitor node = this->node->toFileVisitor();
      bool is_open = opened.contains(QString::fromStdString(binom::Path().toString()));
      binom::VarType type = node.getType();

      {
        binom::BufferArray value_tmp(binom::ValType::byte);
        data.push_back(QVariantMap {
                         {"type", binom::toTypeString(type)},
                         {"type_class", binom::toTypeString(binom::toTypeClass(type))},
                         {"path", QString::fromStdString(binom::Path().toString())},
                         {"is_open", is_open},
                         {"key", "root"},
                         {"depth", QVariant(0)},
                         {"is_value_ref", node.isValueRef()},
                         {"element_count", qulonglong(node.getElementCount())},
                         {"value", (binom::toTypeClass(type) == binom::VarTypeClass::primitive)
                                   ? qulonglong(node.getVariable().getValue().asUi64())
                                   : (type == binom::VarType::byte_array)
                                     ? ((value_tmp = node.getVariable().toBufferArray()).isPrintable()
                                       ? QString::fromStdString(value_tmp)
                                       : QVariant())
                                     : QVariant()}
                       });
      }

      if(is_open)
        buildData(data, binom::Path(), node, 1);
    } break;
  }
  return data;
}

void QBinOMModel::buildData(QVariantList& data, binom::Path path, binom::NodeVisitor& node, quint64 depth) {
  bool node_is_object = node.getTypeClass() == binom::VarTypeClass::object;
  binom::ui64 index = 0;
  for(binom::NodeVisitor child : node) {

    binom::BufferArray name(binom::ValType::byte);
    if(node_is_object) name = *child.getName();
    binom::Path child_path = (node_is_object? (path + name) : (path + index++));
    bool is_open = opened.contains(QString::fromStdString(child_path.toString()));
    data.push_back(QVariantMap {
                     {"type", binom::toTypeString(child.getType())},
                     {"type_class", binom::toTypeString(child.getTypeClass())},
                     {"path", QString::fromStdString(child_path.toString())},
                     {"is_open", is_open},
                     {"key",
                      (node_is_object
                      ? (name.isPrintable()
                        ? QVariant(QString::fromStdString(name))
                        : QVariant(QString::fromStdString(binom::Path({name}).toString())))
                      : QVariant(qulonglong(index - 1)))},
                     {"depth", QVariant(depth)},
                     {"is_value_ref", child.isValueRef()},
                     {"element_count", qulonglong(child.getElementCount())},
                     {"value",
                      (child.getTypeClass() == binom::VarTypeClass::primitive)
                      ? qulonglong(child.getValue().asUi64())
                      : (child.getType() == binom::VarType::byte_array)
                        ? (child.getVariable().toBufferArray().isPrintable()
                          ? QString::fromStdString(child.getVariable().toBufferArray())
                          : QVariant())
                        : QVariant()}
                   });

    if(is_open)
      buildData(data, child_path, child, depth + 1);
  }
}

void QBinOMModel::buildData(QVariantList& data, binom::Path path, binom::FileNodeVisitor& node, quint64 depth) {
  bool node_is_object = node.getTypeClass() == binom::VarTypeClass::object;
  binom::ui64 index = 0;
  for(binom::FileNodeVisitor child : node) {

    binom::BufferArray name(binom::ValType::byte);
    if(node_is_object) name = *child.getName();
    binom::Path child_path = (node_is_object? (path + name) : (path + index++));
    bool is_open = opened.contains(QString::fromStdString(child_path.toString()));
    binom::VarType type = child.getType();
    {
      binom::BufferArray value_tmp(binom::ValType::byte);
      data.push_back(QVariantMap {
                       {"type", binom::toTypeString(type)},
                       {"type_class", binom::toTypeString(binom::toTypeClass(type))},
                       {"path", QString::fromStdString(child_path.toString())},
                       {"is_open", is_open},
                       {"key", (node_is_object
                        ? (name.isPrintable()
                          ? QVariant(QString::fromStdString(name))
                          : QVariant(QString::fromStdString(binom::Path({name}).toString())))
                        : QVariant(qulonglong(index - 1)))},
                       {"depth", QVariant(depth)},
                       {"is_value_ref", child.isValueRef()},
                       {"element_count", qulonglong(child.getElementCount())},
                       {"value", (binom::toTypeClass(type) == binom::VarTypeClass::primitive)
                                 ? qulonglong(child.getVariable().getValue().asUi64())
                                 : (type == binom::VarType::byte_array)
                                   ? ((value_tmp = child.getVariable().toBufferArray()).isPrintable()
                                     ? QString::fromStdString(value_tmp)
                                     : QVariant())
                                   : QVariant()}
                     });
    }

    if(is_open)
      buildData(data, child_path, child, depth + 1);
  }
}
