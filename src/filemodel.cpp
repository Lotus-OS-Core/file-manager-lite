#include "filemodel.h"
#include <QFileIconProvider>
#include <QFont>
#include <QDebug>

FileModel::FileModel(QObject *parent)
    : QFileSystemModel(parent)
{
    setRootPath("/");
    setFilter(QDir::AllEntries | QDir::NoDot | QDir::AllDirs);
    setNameFilterDisables(false);
}

QVariant FileModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();
    
    if (role == Qt::DisplayRole && index.column() == 0) {
        return QFileSystemModel::data(index, Qt::DisplayRole);
    }
    
    if (role == Qt::DecorationRole && index.column() == 0) {
        QFileInfo info = fileInfo(index);
        return getFileIcon(info);
    }
    
    if (role == Qt::FontRole) {
        QFont font;
        font.setPointSize(11);
        return font;
    }
    
    return QFileSystemModel::data(index, role);
}

QIcon FileModel::getFileIcon(const QFileInfo& info) const {
    static QFileIconProvider iconProvider;
    
    if (info.isDir()) {
        return QIcon(":/icons/folder.png");
    }
    
    QString suffix = info.suffix().toLower();
    
    // Document icons
    if (suffix == "pdf") return QIcon(":/icons/pdf.png");
    if (suffix == "doc" || suffix == "docx") return QIcon(":/icons/doc.png");
    if (suffix == "txt") return QIcon(":/icons/text.png");
    if (suffix == "xls" || suffix == "xlsx") return QIcon(":/icons/spreadsheet.png");
    
    // Image icons
    if (suffix == "png" || suffix == "jpg" || suffix == "jpeg" || suffix == "gif" || suffix == "bmp") {
        return QIcon(":/icons/image.png");
    }
    
    // Video icons
    if (suffix == "mp4" || suffix == "avi" || suffix == "mkv" || suffix == "mov") {
        return QIcon(":/icons/video.png");
    }
    
    // Audio icons
    if (suffix == "mp3" || suffix == "wav" || suffix == "flac" || suffix == "aac") {
        return QIcon(":/icons/audio.png");
    }
    
    // Archive icons
    if (suffix == "zip" || suffix == "rar" || suffix == "tar" || suffix == "gz" || suffix == "7z") {
        return QIcon(":/icons/archive.png");
    }
    
    // Code icons
    if (suffix == "cpp" || suffix == "c" || suffix == "h" || suffix == "hpp" || suffix == "py" || suffix == "js") {
        return QIcon(":/icons/code.png");
    }
    
    // Executable
    if (info.isExecutable()) return QIcon(":/icons/executable.png");
    
    return iconProvider.icon(info);
}
