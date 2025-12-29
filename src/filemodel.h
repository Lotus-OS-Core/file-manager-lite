#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QFileSystemModel>
#include <QSortFilterProxyModel>

class FileModel : public QFileSystemModel {
    Q_OBJECT

public:
    explicit FileModel(QObject *parent = nullptr);
    
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    
private:
    QIcon getFileIcon(const QFileInfo& info) const;
};

#endif // FILEMODEL_H
