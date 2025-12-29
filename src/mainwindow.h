#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QStackedWidget>
#include <QLineEdit>
#include <QLabel>
#include <QSplitter>
#include <QTableView>
#include <QSortFilterProxyModel>
#include "sidebar.h"
#include "filemodel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void navigateBack();
    void navigateForward();
    void navigateUp();
    void navigateToHome();
    void navigateToDesktop();
    void navigateToDocuments();
    void navigateToDownloads();
    void refreshView();
    void toggleDarkMode();
    void showContextMenu(const QPoint& pos);
    void handleFileDoubleClick(const QModelIndex& index);
    void updateCurrentPath(const QModelIndex& index);
    void searchFiles(const QString& text);
    void copyFiles();
    void cutFiles();
    void pasteFiles();
    void deleteFiles();
    void renameFile();
    void showFileInfo();
    void toggleSidebar();
    void togglePreview();

private:
    void setupUI();
    void setupToolbar();
    void setupConnections();
    void updateWindowTitle();
    void updateNavigationState();
    void goToDirectory(const QString& path);
    void goToIndex(const QModelIndex& index);
    QAbstractItemView* currentView() const;
    void copyDirectory(const QString& src, const QString& dst);
    
    QWidget* centralWidget;
    QToolBar* toolbar;
    Sidebar* sidebar;
    QStackedWidget* viewStack;
    FileModel* fileModel;
    QSortFilterProxyModel* proxyModel;
    QListView* iconView;
    QTableView* listView;
    QLineEdit* searchBar;
    QLabel* pathLabel;
    
    // Navigation history
    QList<QString> backHistory;
    QList<QString> forwardHistory;
    QString currentPath;
    
    // Actions
    QAction* actionBack;
    QAction* actionForward;
    QAction* actionUp;
    QAction* actionHome;
    QAction* actionRefresh;
    QAction* actionCut;
    QAction* actionDarkMode;
    QAction* actionToggleSidebar;
    QAction* actionTogglePreview;
    QAction* actionViewIcons;
    QAction* actionViewList;
    QAction* actionCopy;
    QAction* actionPaste;
    QAction* actionDelete;
    QAction* actionRename;
    QAction* actionInfo;
    
    bool isDarkMode;
    bool sidebarVisible;
    bool previewVisible;
};

#endif // MAINWINDOW_H
