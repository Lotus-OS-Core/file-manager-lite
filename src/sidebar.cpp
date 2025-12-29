#include "sidebar.h"
#include <QTreeWidgetItem>
#include <QDir>
#include <QDebug>

Sidebar::Sidebar(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void Sidebar::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 5, 0, 0);
    layout->setSpacing(0);
    
    treeWidget = new QTreeWidget(this);
    treeWidget->setObjectName("sidebarTree");
    treeWidget->setHeaderHidden(true);
    treeWidget->setIndentation(12);
    treeWidget->setExpandsOnDoubleClick(false);
    treeWidget->setRootIsDecorated(true);
    treeWidget->setFrameShape(QFrame::NoFrame);
    treeWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    
    // Favorites section - create items first
    QTreeWidgetItem* airDropItem = createItem("AirDrop", ":/icons/airdrop.png", "airdrop");
    QTreeWidgetItem* recentsItem = createItem("Recents", ":/icons/recents.png", "recents");
    QTreeWidgetItem* applicationsItem = createItem("Applications", ":/icons/applications.png", "/usr/share/applications");
    QTreeWidgetItem* desktopItem = createItem("Desktop", ":/icons/desktop.png", QDir::homePath() + "/Desktop");
    QTreeWidgetItem* documentsItem = createItem("Documents", ":/icons/documents.png", QDir::homePath() + "/Documents");
    QTreeWidgetItem* downloadsItem = createItem("Downloads", ":/icons/downloads.png", QDir::homePath() + "/Downloads");
    QTreeWidgetItem* moviesItem = createItem("Movies", ":/icons/movies.png", QDir::homePath() + "/Movies");
    QTreeWidgetItem* musicItem = createItem("Music", ":/icons/music.png", QDir::homePath() + "/Music");
    QTreeWidgetItem* picturesItem = createItem("Pictures", ":/icons/pictures.png", QDir::homePath() + "/Pictures");
    
    QTreeWidgetItem* favoritesItem = new QTreeWidgetItem(treeWidget);
    favoritesItem->setText(0, "Favorites");
    favoritesItem->setExpanded(true);
    // Add children to favorites
    favoritesItem->addChild(airDropItem);
    favoritesItem->addChild(recentsItem);
    favoritesItem->addChild(applicationsItem);
    favoritesItem->addChild(desktopItem);
    favoritesItem->addChild(documentsItem);
    favoritesItem->addChild(downloadsItem);
    favoritesItem->addChild(moviesItem);
    favoritesItem->addChild(musicItem);
    favoritesItem->addChild(picturesItem);
    
    // Locations section - create items first
    QTreeWidgetItem* homeItem = createItem("Home", ":/icons/home.png", QDir::homePath());
    QTreeWidgetItem* computerItem = createItem("Computer", ":/icons/computer.png", "/");
    
    QTreeWidgetItem* locationsItem = new QTreeWidgetItem(treeWidget);
    locationsItem->setText(0, "Locations");
    locationsItem->setExpanded(true);
    // Add children to locations
    locationsItem->addChild(homeItem);
    locationsItem->addChild(computerItem);
    
    // Add top-level items to tree
    treeWidget->addTopLevelItem(favoritesItem);
    treeWidget->addTopLevelItem(locationsItem);
    
    // Set fixed height for top-level items
    for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
        treeWidget->topLevelItem(i)->setSizeHint(0, QSize(-1, 28));
    }
    
    connect(treeWidget, &QTreeWidget::itemClicked, this, &Sidebar::handleItemClicked);
    
    layout->addWidget(treeWidget);
}

QTreeWidgetItem* Sidebar::createItem(const QString& name, const QString& icon, const QString& path) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, name);
    item->setData(0, Qt::UserRole, path);
    item->setSizeHint(0, QSize(-1, 24));
    return item;
}

void Sidebar::handleItemClicked(QTreeWidgetItem* item, int column) {
    if (!item) return;
    
    QString path = item->data(0, Qt::UserRole).toString();
    QString name = item->text(0);
    
    if (path == "airdrop") {
        emit navigateTo(QDir::homePath());
    } else if (path == "recents") {
        emit navigateTo(QDir::homePath());
    } else if (path == "/usr/share/applications") {
        emit navigateTo(path);
    } else if (name == "Home") {
        emit navigateToHome();
    } else if (name == "Desktop") {
        emit navigateToDesktop();
    } else if (name == "Documents") {
        emit navigateToDocuments();
    } else if (name == "Downloads") {
        emit navigateToDownloads();
    } else if (!path.isEmpty() && path != "airdrop" && path != "recents") {
        emit navigateTo(path);
    }
}
