#ifndef QBINOMTREEMODEL_H
#define QBINOMTREEMODEL_H

#include <QAbstractItemModel>

#undef foreach
#include "binom/includes/binom.h"


class QBinOMTreeItem {
  enum class Row : int {
    type = 0,
    path = 1,
  };

  QBinOMTreeItem* parent;
  binom::VarType type;
  binom::Path path;

  QVector<QBinOMTreeItem*> childs;
public:
  explicit QBinOMTreeItem(binom::NodeVisitorBase &data, binom::Path path, QBinOMTreeItem* parent = nullptr);

  ~QBinOMTreeItem();

  int childCount() const;
  void appendChild(QBinOMTreeItem* child);
  QBinOMTreeItem* child(int row);

  int columnCount() const;
  QVariant data(int column);

  QBinOMTreeItem* parentItem();;
  int row() const;

};


class QBinOMTreeModel : public QAbstractItemModel {
  Q_OBJECT
  static QBinOMTreeModel empty_model;
  QBinOMTreeItem* root_item;
public:
  enum class Roles : int {
    type = Qt::UserRole + 1,
    path
  };

  explicit QBinOMTreeModel(binom::NodeVisitorBase& root_node_visitor, QObject* parent = nullptr);
  ~QBinOMTreeModel();

  QVariant data(const QModelIndex &index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QHash<int, QByteArray> roleNames() const override;

  static QBinOMTreeModel& getEmptyModel();

private:
  static inline QHash<int, QByteArray> roles = {
    {int(Roles::type), "type"},
    {int(Roles::path), "path"}
  };

};
#endif // QBINOMTREEMODEL_H
