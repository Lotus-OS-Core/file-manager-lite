#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListView>
#include <QTableView>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QMimeData>
#include <QUrl>
#include <QClipboard>
#include <QApplication>
#include <QKeySequence>
#include <QStatusBar>
#include <QDockWidget>
#include <QFileInfo>
#include <QDebug>
#include <QMenu>
#include <QDesktopServices>
#include <QDateTime>
#include <QLocale>
#include <QDir>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , fileModel(new FileModel(this))
    , proxyModel(new QSortFilterProxyModel(this))
    , isDarkMode(false)
    , sidebarVisible(true)
    , previewVisible(false)
    , currentPath(QDir::homePath())
{
    proxyModel->setSourceModel(fileModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(0);
    
    setupUI();
    setupToolbar();
    setupConnections();
    
    // Start at home directory
    goToDirectory(QDir::homePath());
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    // Central widget with layout
    centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Path label (like breadcrumb)
    pathLabel = new QLabel(this);
    pathLabel->setObjectName("pathLabel");
    pathLabel->setFixedHeight(30);
    pathLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainLayout->addWidget(pathLabel);
    
    // Main content area with sidebar
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setObjectName("mainSplitter");
    splitter->setHandleWidth(1);
    
    // Sidebar
    sidebar = new Sidebar(this);
    splitter->addWidget(sidebar);
    
    // View stack (Icon view + List view)
    viewStack = new QStackedWidget(this);
    
    iconView = new QListView(this);
    iconView->setObjectName("iconView");
    iconView->setModel(proxyModel);
    iconView->setViewMode(QListView::IconMode);
    iconView->setGridSize(QSize(90, 90));
    iconView->setSpacing(10);
    iconView->setResizeMode(QListView::Adjust);
    iconView->setSelectionMode(QListView::ExtendedSelection);
    iconView->setAlternatingRowColors(false);
    iconView->setWrapping(true);
    
    listView = new QTableView(this);
    listView->setObjectName("listView");
    listView->setModel(proxyModel);
    listView->setSelectionMode(QListView::ExtendedSelection);
    listView->setShowGrid(false);
    listView->setAlternatingRowColors(true);
    listView->horizontalHeader()->setStretchLastSection(true);
    listView->verticalHeader()->setVisible(false);
    
    viewStack->addWidget(iconView);
    viewStack->addWidget(listView);
    splitter->addWidget(viewStack);
    
    splitter->setSizes({220, 780});
    mainLayout->addWidget(splitter);
    
    // Status bar
    statusBar()->setFixedHeight(24);
    
    setCentralWidget(centralWidget);
    setWindowTitle("Lotus-DIR");
    resize(1000, 700);
}

void MainWindow::setupToolbar() {
    toolbar = new QToolBar(this);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setFixedHeight(50);
    toolbar->setObjectName("mainToolbar");
    addToolBar(toolbar);
    
    // Navigation actions
    actionBack = new QAction(QIcon(":/icons/back.png"), "Back", this);
    actionBack->setShortcut(QKeySequence::Back);
    toolbar->addAction(actionBack);
    
    actionForward = new QAction(QIcon(":/icons/forward.png"), "Forward", this);
    actionForward->setShortcut(QKeySequence::Forward);
    toolbar->addAction(actionForward);
    
    actionUp = new QAction(QIcon(":/icons/up.png"), "Up", this);
    actionUp->setShortcut(QKeySequence::Backspace);
    toolbar->addAction(actionUp);
    
    actionHome = new QAction(QIcon(":/icons/home.png"), "Home", this);
    toolbar->addAction(actionHome);
    
    toolbar->addSeparator();
    
    // Search bar
    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("Search");
    searchBar->setFixedWidth(200);
    searchBar->setObjectName("searchBar");
    toolbar->addWidget(searchBar);
    
    toolbar->addSeparator();
    
    // View toggle actions
    actionViewIcons = new QAction(QIcon(":/icons/icon-view.png"), "Icon View", this);
    actionViewIcons->setCheckable(true);
    actionViewIcons->setChecked(true);
    toolbar->addAction(actionViewIcons);
    
    actionViewList = new QAction(QIcon(":/icons/list-view.png"), "List View", this);
    actionViewList->setCheckable(true);
    toolbar->addAction(actionViewList);
    
    toolbar->addSeparator();
    
    // File operations
    actionCopy = new QAction(QIcon(":/icons/copy.png"), "Copy", this);
    actionCopy->setShortcut(QKeySequence::Copy);
    toolbar->addAction(actionCopy);
    
    actionCut = new QAction(QIcon(":/icons/cut.png"), "Cut", this);
    actionCut->setShortcut(QKeySequence::Cut);
    toolbar->addAction(actionCut);
    
    actionPaste = new QAction(QIcon(":/icons/paste.png"), "Paste", this);
    actionPaste->setShortcut(QKeySequence::Paste);
    toolbar->addAction(actionPaste);
    
    actionDelete = new QAction(QIcon(":/icons/delete.png"), "Delete", this);
    actionDelete->setShortcut(QKeySequence::Delete);
    toolbar->addAction(actionDelete);
    
    toolbar->addSeparator();
    
    // Refresh action
    actionRefresh = new QAction(QIcon(":/icons/refresh.png"), "Refresh", this);
    actionRefresh->setShortcut(QKeySequence::Refresh);
    toolbar->addAction(actionRefresh);
    
    toolbar->addSeparator();
    
    // Toggle actions
    actionToggleSidebar = new QAction(QIcon(":/icons/sidebar.png"), "Toggle Sidebar", this);
    actionToggleSidebar->setCheckable(true);
    actionToggleSidebar->setChecked(true);
    toolbar->addAction(actionToggleSidebar);
    
    actionTogglePreview = new QAction(QIcon(":/icons/preview.png"), "Toggle Preview", this);
    actionTogglePreview->setCheckable(true);
    toolbar->addAction(actionTogglePreview);
    
    toolbar->addSeparator();
    
    actionDarkMode = new QAction(QIcon(":/icons/dark.png"), "Dark Mode", this);
    actionDarkMode->setCheckable(true);
    toolbar->addAction(actionDarkMode);
}

void MainWindow::setupConnections() {
    // Navigation
    connect(actionBack, &QAction::triggered, this, &MainWindow::navigateBack);
    connect(actionForward, &QAction::triggered, this, &MainWindow::navigateForward);
    connect(actionUp, &QAction::triggered, this, &MainWindow::navigateUp);
    connect(actionHome, &QAction::triggered, this, &MainWindow::navigateToHome);
    
    // Sidebar navigation
    connect(sidebar, &Sidebar::navigateTo, this, &MainWindow::goToDirectory);
    connect(sidebar, &Sidebar::navigateToHome, this, &MainWindow::navigateToHome);
    connect(sidebar, &Sidebar::navigateToDesktop, this, &MainWindow::navigateToDesktop);
    connect(sidebar, &Sidebar::navigateToDocuments, this, &MainWindow::navigateToDocuments);
    connect(sidebar, &Sidebar::navigateToDownloads, this, &MainWindow::navigateToDownloads);
    
    // File view connections
    connect(iconView, &QListView::doubleClicked, this, &MainWindow::handleFileDoubleClick);
    connect(listView, &QTableView::doubleClicked, this, &MainWindow::handleFileDoubleClick);
    
    // Context menu
    iconView->setContextMenuPolicy(Qt::CustomContextMenu);
    listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(iconView, &QListView::customContextMenuRequested, this, &MainWindow::showContextMenu);
    connect(listView, &QListView::customContextMenuRequested, this, &MainWindow::showContextMenu);
    
    // Search
    connect(searchBar, &QLineEdit::textChanged, this, &MainWindow::searchFiles);
    
    // View toggle
    connect(actionViewIcons, &QAction::triggered, [this]() {
        viewStack->setCurrentIndex(0);
        actionViewIcons->setChecked(true);
        actionViewList->setChecked(false);
    });
    
    connect(actionViewList, &QAction::triggered, [this]() {
        viewStack->setCurrentIndex(1);
        actionViewIcons->setChecked(false);
        actionViewList->setChecked(true);
    });
    
    // Toggle actions
    connect(actionToggleSidebar, &QAction::toggled, [this](bool checked) {
        sidebar->setVisible(checked);
        sidebarVisible = checked;
    });
    
    connect(actionDarkMode, &QAction::toggled, this, &MainWindow::toggleDarkMode);
    
    // File operations connections
    connect(actionCopy, &QAction::triggered, this, &MainWindow::copyFiles);
    connect(actionCut, &QAction::triggered, this, &MainWindow::cutFiles);
    connect(actionPaste, &QAction::triggered, this, &MainWindow::pasteFiles);
    connect(actionDelete, &QAction::triggered, this, &MainWindow::deleteFiles);
    
    // Refresh action connection
    connect(actionRefresh, &QAction::triggered, this, &MainWindow::refreshView);
}

void MainWindow::navigateBack() {
    if (backHistory.isEmpty()) return;
    
    forwardHistory.append(currentPath);
    QString targetPath = backHistory.takeLast();
    goToDirectory(targetPath);
    updateNavigationState();
}

void MainWindow::navigateForward() {
    if (forwardHistory.isEmpty()) return;
    
    backHistory.append(currentPath);
    QString targetPath = forwardHistory.takeLast();
    goToDirectory(targetPath);
    updateNavigationState();
}

void MainWindow::navigateUp() {
    QDir dir(currentPath);
    if (dir.cdUp()) {
        goToDirectory(dir.absolutePath());
    }
}

void MainWindow::navigateToHome() {
    goToDirectory(QDir::homePath());
}

void MainWindow::navigateToDesktop() {
    goToDirectory(QDir::homePath() + "/Desktop");
}

void MainWindow::navigateToDocuments() {
    goToDirectory(QDir::homePath() + "/Documents");
}

void MainWindow::navigateToDownloads() {
    goToDirectory(QDir::homePath() + "/Downloads");
}

void MainWindow::refreshView() {
    fileModel->setRootPath(currentPath);
    QModelIndex rootIndex = fileModel->index(currentPath);
    iconView->setRootIndex(proxyModel->mapFromSource(rootIndex));
    listView->setRootIndex(proxyModel->mapFromSource(rootIndex));
}

void MainWindow::toggleDarkMode() {
    isDarkMode = !isDarkMode;
    actionDarkMode->setChecked(isDarkMode);
    
    QApplication* app = qApp;
    QString styleFile = isDarkMode ? ":/qss/dark.qss" : ":/qss/light.qss";
    QFile file(styleFile);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        app->setStyleSheet(stream.readAll());
        file.close();
    }
}

void MainWindow::showContextMenu(const QPoint& pos) {
    QAbstractItemView* view = currentView();
    QModelIndex index = view->indexAt(pos);
    
    if (!index.isValid()) {
        // Right-click on empty area - show general menu
        QMenu contextMenu(this);
        contextMenu.addAction(actionPaste);
        contextMenu.addSeparator();
        contextMenu.addAction(actionRefresh);
        contextMenu.exec(QCursor::pos());
        return;
    }
    
    QModelIndex sourceIndex = proxyModel->mapToSource(index);
    QFileInfo fileInfo = fileModel->fileInfo(sourceIndex);
    bool isDir = fileInfo.isDir();
    
    QMenu contextMenu(this);
    
    // Open/Open With
    if (isDir) {
        contextMenu.addAction(QIcon(":/icons/open.png"), "Open", this, [this, index]() {
            handleFileDoubleClick(index);
        });
    } else {
        contextMenu.addAction(QIcon(":/icons/open.png"), "Open", this, [this, index]() {
            handleFileDoubleClick(index);
        });
        contextMenu.addAction("Open With...");
    }
    contextMenu.addSeparator();
    
    // Cut/Copy/Paste
    contextMenu.addAction(QIcon(":/icons/cut.png"), "Cut", this, &MainWindow::cutFiles);
    contextMenu.addAction(QIcon(":/icons/copy.png"), "Copy", this, &MainWindow::copyFiles);
    contextMenu.addAction(QIcon(":/icons/paste.png"), "Paste", this, &MainWindow::pasteFiles);
    contextMenu.addSeparator();
    
    // Rename/Delete
    contextMenu.addAction(QIcon(":/icons/rename.png"), "Rename", this, &MainWindow::renameFile);
    contextMenu.addAction(QIcon(":/icons/delete.png"), "Move to Trash", this, &MainWindow::deleteFiles);
    contextMenu.addSeparator();
    
    // Get Info
    contextMenu.addAction(QIcon(":/icons/info.png"), "Get Info", this, &MainWindow::showFileInfo);
    
    contextMenu.exec(QCursor::pos());
}

void MainWindow::cutFiles() {
    QItemSelectionModel* selection = iconView->selectionModel();
    if (!selection->hasSelection()) {
        selection = listView->selectionModel();
    }
    
    QList<QUrl> urls;
    QModelIndexList indexes = selection->selectedIndexes();
    
    for (const QModelIndex& idx : indexes) {
        if (idx.column() == 0) {
            QModelIndex sourceIdx = proxyModel->mapToSource(idx);
            urls.append(QUrl::fromLocalFile(fileModel->filePath(sourceIdx)));
        }
    }
    
    QMimeData* mimeData = new QMimeData();
    mimeData->setUrls(urls);
    
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData);
    
    // Mark as cut for later paste operation
    clipboard->setText("CUT_OPERATION", QClipboard::Selection);
    
    statusBar()->showMessage(QString("Cut %1 item(s)").arg(urls.size()), 2000);
}

void MainWindow::handleFileDoubleClick(const QModelIndex& index) {
    if (!index.isValid()) return;
    
    // Map through proxy model if needed
    QModelIndex sourceIndex = proxyModel->mapToSource(index);
    QFileSystemModel* model = qobject_cast<QFileSystemModel*>(fileModel);
    QFileInfo fileInfo = model->fileInfo(sourceIndex);
    
    if (fileInfo.isDir()) {
        backHistory.append(currentPath);
        forwardHistory.clear();
        goToDirectory(fileInfo.absoluteFilePath());
    } else {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    }
}

void MainWindow::updateCurrentPath(const QModelIndex& index) {
    if (index.isValid()) {
        currentPath = fileModel->filePath(index);
        pathLabel->setText(currentPath);
        updateWindowTitle();
    }
}

void MainWindow::searchFiles(const QString& text) {
    proxyModel->setFilterFixedString(text);
}

void MainWindow::copyFiles() {
    QItemSelectionModel* selection = iconView->selectionModel();
    if (!selection->hasSelection()) {
        selection = listView->selectionModel();
    }
    
    QList<QUrl> urls;
    QModelIndexList indexes = selection->selectedIndexes();
    
    for (const QModelIndex& index : indexes) {
        if (index.column() == 0) {
            QModelIndex sourceIndex = proxyModel->mapToSource(index);
            urls.append(QUrl::fromLocalFile(fileModel->filePath(sourceIndex)));
        }
    }
    
    QMimeData* mimeData = new QMimeData();
    mimeData->setUrls(urls);
    
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData);
    
    statusBar()->showMessage(QString("Copied %1 item(s)").arg(urls.size()), 2000);
}

void MainWindow::pasteFiles() {
    QClipboard* clipboard = QApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();
    
    if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        for (const QUrl& url : urls) {
            if (url.isLocalFile()) {
                QString sourcePath = url.toLocalFile();
                QString destPath = currentPath + "/" + QFileInfo(sourcePath).fileName();
                
                QFileInfo sourceInfo(sourcePath);
                if (sourceInfo.isDir()) {
                    copyDirectory(sourcePath, destPath);
                } else {
                    QFile::copy(sourcePath, destPath);
                }
            }
        }
        refreshView();
    }
}

void MainWindow::deleteFiles() {
    QItemSelectionModel* selection = iconView->selectionModel();
    if (!selection->hasSelection()) {
        selection = listView->selectionModel();
    }
    
    QModelIndexList indexes = selection->selectedIndexes();
    if (indexes.isEmpty()) return;
    
    int itemCount = 0;
    for (const QModelIndex& index : indexes) {
        if (index.column() == 0) itemCount++;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Delete Files",
        QString("Move %1 item(s) to Trash?").arg(itemCount),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        for (const QModelIndex& index : indexes) {
            if (index.column() == 0) {
                QModelIndex sourceIndex = proxyModel->mapToSource(index);
                QString filePath = fileModel->filePath(sourceIndex);
                QFile::moveToTrash(filePath);
            }
        }
        refreshView();
    }
}

void MainWindow::renameFile() {
    QAbstractItemView* view = currentView();
    QModelIndex index = view->currentIndex();
    
    if (!index.isValid()) return;
    
    QModelIndex sourceIndex = proxyModel->mapToSource(index);
    QString oldName = fileModel->fileName(sourceIndex);
    bool ok;
    QString newName = QInputDialog::getText(
        this, "Rename",
        "Enter new name:",
        QLineEdit::Normal,
        oldName,
        &ok
    );
    
    if (ok && !newName.isEmpty() && newName != oldName) {
        QString oldPath = fileModel->filePath(sourceIndex);
        QString newPath = QFileInfo(oldPath).absolutePath() + "/" + newName;
        QFile::rename(oldPath, newPath);
        refreshView();
    }
}

void MainWindow::showFileInfo() {
    QAbstractItemView* view = currentView();
    QModelIndex index = view->currentIndex();
    
    if (!index.isValid()) return;
    
    QModelIndex sourceIndex = proxyModel->mapToSource(index);
    QFileInfo fileInfo = fileModel->fileInfo(sourceIndex);
    
    QString modifiedDate = fileInfo.lastModified().toString(QLocale::system().dateTimeFormat(QLocale::ShortFormat));
    
    QString info = QString(
        "<b>Name:</b> %1<br>"
        "<b>Path:</b> %2<br>"
        "<b>Size:</b> %3<br>"
        "<b>Type:</b> %4<br>"
        "<b>Modified:</b> %5"
    ).arg(fileInfo.fileName())
     .arg(fileInfo.absoluteFilePath())
     .arg(fileInfo.isDir() ? "Folder" : QString::number(fileInfo.size()) + " bytes")
     .arg(fileInfo.isDir() ? "Folder" : fileInfo.suffix())
     .arg(modifiedDate);
    
    QMessageBox::information(this, "File Info", info);
}

QAbstractItemView* MainWindow::currentView() const {
    return viewStack->currentIndex() == 0 ? static_cast<QAbstractItemView*>(iconView) : static_cast<QAbstractItemView*>(listView);
}

void MainWindow::copyDirectory(const QString& src, const QString& dst) {
    QDir sourceDir(src);
    QDir destDir(dst);
    
    if (!destDir.exists()) {
        destDir.mkpath(".");
    }
    
    QStringList entries = sourceDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for (const QString& entry : entries) {
        QString sourcePath = src + "/" + entry;
        QString destPath = dst + "/" + entry;
        QFileInfo info(sourcePath);
        
        if (info.isDir()) {
            copyDirectory(sourcePath, destPath);
        } else {
            QFile::copy(sourcePath, destPath);
        }
    }
}

void MainWindow::toggleSidebar() {
    sidebar->setVisible(!sidebar->isVisible());
}

void MainWindow::togglePreview() {
    // Preview panel toggle - can be extended
    previewVisible = !previewVisible;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    QMainWindow::closeEvent(event);
}

void MainWindow::updateWindowTitle() {
    QString folderName = QDir(currentPath).dirName();
    if (folderName.isEmpty()) folderName = currentPath;
    setWindowTitle(folderName + " - Lotus-DIR");
}

void MainWindow::updateNavigationState() {
    actionBack->setEnabled(!backHistory.isEmpty());
    actionForward->setEnabled(!forwardHistory.isEmpty());
}

void MainWindow::goToDirectory(const QString& path) {
    if (!QDir(path).exists()) return;
    
    backHistory.append(currentPath);
    forwardHistory.clear();
    currentPath = path;
    
    fileModel->setRootPath(path);
    QModelIndex rootIndex = fileModel->index(path);
    iconView->setRootIndex(proxyModel->mapFromSource(rootIndex));
    listView->setRootIndex(proxyModel->mapFromSource(rootIndex));
    
    pathLabel->setText(path);
    updateWindowTitle();
    updateNavigationState();
}

void MainWindow::goToIndex(const QModelIndex& index) {
    if (index.isValid() && fileModel->isDir(index)) {
        goToDirectory(fileModel->filePath(index));
    }
}
