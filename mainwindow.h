#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "ctablemodel.h"
#include <QKeyEvent>

class MainWindow : public QMainWindow, public Ui_MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void keywordChanged(QString keyword);

public slots:
    void on_search_clicked();
    void on_tableView_doubleClicked(const QModelIndex & index);
    void setStatusBarText(const QString &text);
    void showContextMenu(const QPoint &pos);
    void openFile();
    void openFilePath();
    void copyFullPath();

protected:
    void keyPressEvent ( QKeyEvent * event );


private:
    bool initTable();
    CTableModel *m_tableModel;
    QThread *m_modelThread;
    int m_showContextRow;
};

#endif // MAINWINDOW_H
