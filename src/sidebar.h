#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QListWidget>

class Sidebar : public QWidget {
    Q_OBJECT

public:
    explicit Sidebar(QWidget *parent = nullptr);

signals:
    void navigateTo(const QString& path);
    void navigateToHome();
    void navigateToDesktop();
    void navigateToDocuments();
    void navigateToDownloads();

private slots:
    void handleItemClicked(QTreeWidgetItem* item, int column);

private:
    void setupUI();
    QTreeWidget* treeWidget;
    QTreeWidgetItem* createItem(const QString& name, const QString& icon, const QString& path);
};

#endif // SIDEBAR_H
