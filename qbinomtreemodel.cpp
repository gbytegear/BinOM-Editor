#include "qbinomtreemodel.h"

// ================================================== QBinOMTreeItem ==================================================

QBinOMTreeItem::QBinOMTreeItem(binom::NodeVisitorBase& data, binom::Path path, QBinOMTreeItem* parent)
  : parent(parent), type(data.getType()), path(path) {
  switch (binom::toTypeClass(type)) {
    case binom::VarTypeClass::buffer_array:
    case binom::VarTypeClass::primitive: return;
    case binom::VarTypeClass::array: {
      binom::ui64 index = 0;
      switch (data.getVisitorType()) {
        case binom::VisitorType::ram_storage_visitor:
          for(binom::NodeVisitor child_data : data.toRAMVisitor())
            childs.push_back(new QBinOMTreeItem(child_data.upCast(), path + (index++), this));
        break;
        case binom::VisitorType::file_storage_visitor:
          for(binom::FileNodeVisitor child_data : data.toFileVisitor())
            childs.push_back(new QBinOMTreeItem(child_data.upCast(), path + (index++), this));
        break;
      }
    } return;
    case binom::VarTypeClass::object:
      switch (data.getVisitorType()) {
        case binom::VisitorType::ram_storage_visitor:
          for(binom::NodeVisitor child_data : data.toRAMVisitor())
            childs.push_back(new QBinOMTreeItem(child_data.upCast(), path + *child_data.getName(), this));
        break;
        case binom::VisitorType::file_storage_visitor:
          for(binom::FileNodeVisitor child_data : data.toFileVisitor())
            childs.push_back(new QBinOMTreeItem(child_data.upCast(), path + *child_data.getName(), this));
        break;
      }
    return;
    case binom::VarTypeClass::invalid_type:
    break;
  }
}



QBinOMTreeItem::~QBinOMTreeItem() {qDeleteAll(childs);}



int QBinOMTreeItem::childCount() const {return childs.count();}



void QBinOMTreeItem::appendChild(QBinOMTreeItem* child) {childs.push_back(child);}



QBinOMTreeItem* QBinOMTreeItem::child(int row) {
  if (row < 0 || row >= childs.size()) return nullptr;
  return childs[row];
}



int QBinOMTreeItem::columnCount() const {return 2;}



QVariant QBinOMTreeItem::data(int column) {
  switch (Row(column)) {
    case Row::type: return binom::toTypeString(type);
    case Row::path: return QString::fromStdString(path.toString());
    default: return QVariant();
  }
}



QBinOMTreeItem* QBinOMTreeItem::parentItem() {return parent;}



int QBinOMTreeItem::row() const {
  if(parent)
    return parent->childs.indexOf(const_cast<QBinOMTreeItem*>(this));
  return -1;
}

// ================================================== QBinOMTreeModel ==================================================

binom::NodeVisitor empty_node = nullptr;
QBinOMTreeModel QBinOMTreeModel::empty_model(empty_node.upCast(), nullptr);

QBinOMTreeModel::QBinOMTreeModel(binom::NodeVisitorBase& root_node_visitor, QObject* parent)
  : QAbstractItemModel(parent), root_item(new QBinOMTreeItem(root_node_visitor, binom::Path())) {}



QBinOMTreeModel::~QBinOMTreeModel() {if(root_item) delete root_item;}



QVariant QBinOMTreeModel::data(const QModelIndex& index, int role) const {
  if(!index.isValid() || role != Qt::DisplayRole)
    return QVariant();
  return static_cast<QBinOMTreeItem*>(index.internalPointer())->data(index.column());
}



Qt::ItemFlags QBinOMTreeModel::flags(const QModelIndex& index) const {
  if(!index.isValid()) return Qt::NoItemFlags;
  return QAbstractItemModel::flags(index);
}



QVariant QBinOMTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return root_item->data(section);
  return QVariant();
}



QModelIndex QBinOMTreeModel::index(int row, int column, const QModelIndex& parent) const {
  if(!hasIndex(row, column, parent))
    return QModelIndex();

  QBinOMTreeItem* parent_item = parent.isValid()
                                ? static_cast<QBinOMTreeItem*>(parent.internalPointer())
                                : root_item;

  if(QBinOMTreeItem* child_item = parent_item->child(row); child_item)
    return createIndex(row, column, child_item);

  return QModelIndex();
}



QModelIndex QBinOMTreeModel::parent(const QModelIndex& index) const {
  if(!index.isValid()) return QModelIndex();
  QBinOMTreeItem* parent_item = static_cast<QBinOMTreeItem*>(index.internalPointer())->parentItem();
  if(parent_item == root_item) return QModelIndex();
  return createIndex(parent_item->row(), 0, parent_item);
}



int QBinOMTreeModel::rowCount(const QModelIndex& parent) const {
  if(parent.column() > 0) return 0;
  QBinOMTreeItem* parent_item = parent.isValid()
                                ? static_cast<QBinOMTreeItem*>(parent.internalPointer())
                                : root_item;
  return parent_item->childCount();
}



int QBinOMTreeModel::columnCount(const QModelIndex& parent) const {
  return parent.isValid()
      ? static_cast<QBinOMTreeItem*>(parent.internalPointer())->columnCount()
      : root_item->columnCount();
}

QHash<int, QByteArray> QBinOMTreeModel::roleNames() const {return roles;}

QBinOMTreeModel& QBinOMTreeModel::getEmptyModel() {return empty_model;}
