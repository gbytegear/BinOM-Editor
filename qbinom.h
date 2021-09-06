#ifndef QBINOM_H
#define QBINOM_H

#include <QObject>
#include <QVariant>

#undef foreach
#include "binom/includes/binom.h"
#include <optional>
#include <map>


class BinOMFile {
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
    ~File();
    File(binom::FileType file_type, QString file_path);
    File(File&& other) {
      std::memcpy((void*)this, (void*)&other, sizeof (File));
      std::memset((void*)&other, 0, sizeof (File));
    }
  } storage_union;

public:
  BinOMFile(binom::FileType file_type, QString file_path);
  BinOMFile(BinOMFile&& other);
  ~BinOMFile();

  std::unique_ptr<binom::NodeVisitorBase> getRoot();
};

class QBinOM : QObject {

  std::map<std::string, BinOMFile> files;
  BinOMFile* selected = nullptr;

  Q_OBJECT
public:
  QBinOM() : QObject(nullptr) {}

  Q_INVOKABLE bool openFile(QString file_path);
  Q_INVOKABLE void closeFile(QString file_name) {}
};

#endif // QBINOM_H
