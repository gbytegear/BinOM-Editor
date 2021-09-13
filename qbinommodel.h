#ifndef QBINOMMODEL_H
#define QBINOMMODEL_H

#undef foreach
#include "BinOM/src/binom/includes/binom.h"
//#include "include/binom/includes/binom.h"
#include <QString>
#include <QObject>
#include <QVariant>

#include <QSet>

class QBinOMModel {
  std::unique_ptr<binom::NodeVisitorBase> node;
  QSet<QString> opened;

  void buildData(QVariantList& data, binom::Path path, binom::NodeVisitor& node);
  void buildData(QVariantList& data, binom::Path path, binom::FileNodeVisitor& node);

public:
  QBinOMModel() = default;
  QBinOMModel(std::unique_ptr<binom::NodeVisitorBase> node) : node(std::move(node)), opened() {}
  operator QVariantList();
  void open(binom::Path path) {opened.insert(QString::fromStdString(path.toString()));}
  void close(binom::Path path) {opened.remove(QString::fromStdString(path.toString()));}

};

#endif // QBINOMMODEL_H
