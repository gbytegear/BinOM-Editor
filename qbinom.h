#ifndef QBINOM_H
#define QBINOM_H

#include "qbinommodel.h"

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
  } storage_union;

  QBinOMModel model;

public:
  BinOMFile(binom::FileType file_type, QString file_path);
  ~BinOMFile();

  std::unique_ptr<binom::NodeVisitorBase> getRoot();
  binom::FileType getType() {return file_type;}
  QVariantList getModel() {return model;}
};


class QBinOM : public QObject {

  // Note: QMap doesn't have emplace methods
  std::map<QString, std::unique_ptr<BinOMFile>> files;
  std::map<QString, std::unique_ptr<BinOMFile>>::iterator selected_file;

  Q_PROPERTY(bool is_file_selected READ isFileSelected NOTIFY isFileSelectedChanged)
  Q_PROPERTY(QVariantList open_files READ getOpenFiles NOTIFY openFilesChanged)
  Q_PROPERTY(QVariantList tree_model READ getTreeModel NOTIFY treeModelChanged)
  Q_OBJECT

public:
  QBinOM() : QObject(nullptr), selected_file(files.end()) {}

  Q_INVOKABLE bool openFile(QString file_path);
  Q_INVOKABLE void closeFile(QString file_name);
  Q_INVOKABLE bool selectFile(QString file_name);
  Q_INVOKABLE bool isFileSelected() const {return selected_file != files.end();}

  QVariantList getOpenFiles() const {
    QVariantList files_info;
    for(auto& [name, file] : files) {
      files_info.push_back(QVariantMap{
                             {"name", name},
                             {"type", (file->getType() == binom::FileType::file_storage)
                                      ? "file storage"
                                      :(file->getType() == binom::FileType::serialized_file_storage)
                                      ? "serialized storage"
                                      : "undefined"}
                           });
    }
    return files_info;
  }

  QVariantList getTreeModel() const {return isFileSelected()?selected_file->second->getModel() : QVariantList();}

signals:
  void isFileSelectedChanged(bool is_file_selected);
  void openFilesChanged(QVariantList open_files);
  void treeModelChanged(QVariantList tree_mode);
};

#endif // QBINOM_H
