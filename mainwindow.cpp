#include "mainwindow.h"
#include <QClipboard>
#include <QMenu>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    setWindowIcon(QIcon(QPixmap("://windowIcon.png")));
    initTable();
    connect(m_tableModel, SIGNAL(statusBarTextChanged(const QString &)), this, SLOT(setStatusBarText(const QString &)));
}

MainWindow::~MainWindow()
{
    m_modelThread->quit();
    if (!m_modelThread->wait(3000))
    {
        m_modelThread->terminate();
        m_modelThread->wait();
    }
}

bool MainWindow::initTable()
{
    m_tableModel = new CTableModel();
    m_modelThread = new QThread(this);
    m_tableModel->moveToThread(m_modelThread);
    connect(this, SIGNAL(keywordChanged(QString)), m_tableModel, SLOT(setKeyword(QString)));
    m_modelThread->start();

    tableView->setModel(m_tableModel);
    tableView->setShowGrid(false);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    /* tableview width: 698 */
    tableView->setColumnWidth(REC_NAME, 278);
    tableView->setColumnWidth(REC_PATH, 200);
    tableView->setColumnWidth(REC_SIZE, 70);
    tableView->setColumnWidth(REC_MTIME, 130);

    /* create right button menu */
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    return true;
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex & index)
{
    m_showContextRow = index.row();
    openFile();
}

void MainWindow::on_search_clicked()
{
    if (keyword->text().isEmpty())
    {
        return;
    }
    setWindowTitle(keyword->text() + " - Everything");
    emit keywordChanged(keyword->text());
}

void MainWindow::setStatusBarText(const QString &text)
{
    Ui_MainWindow::statusBar->showMessage(text + " objects");
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    if (tableView->indexAt(pos).row() == -1)
        return;

    m_showContextRow = tableView->indexAt(pos).row();

    QMenu menu(this);
    QAction *open = menu.addAction("Open");
    QAction *openPath = menu.addAction("Open Path");
    QAction *copyFullPath = menu.addAction("Copy Full Name to Clipboard");

    connect(open, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(openPath, SIGNAL(triggered()), this, SLOT(openFilePath()));
    connect(copyFullPath, SIGNAL(triggered()), this, SLOT(copyFullPath()));
    menu.exec(QCursor::pos());
}

void MainWindow::openFile()
{
    QString cmd = "xdg-open " + tableView->model()->index(m_showContextRow, 1).data().toString() + "/"
            + tableView->model()->index(m_showContextRow, 0).data().toString();
    if (system(cmd.toLocal8Bit().data()) != 0)
        QMessageBox::warning(this, "Error opening", "No application is registered as handling this file");
}

void MainWindow::openFilePath()
{
    QString cmd = "xdg-open " + tableView->model()->index(m_showContextRow, 1).data().toString();
    system(cmd.toLocal8Bit().data());
}

void MainWindow::copyFullPath()
{
    QClipboard *board = QApplication::clipboard();
    QString path = tableView->model()->index(m_showContextRow, 1).data().toString();
    QString name = tableView->model()->index(m_showContextRow, 0).data().toString();
    if (path == "/")
    {
        board->setText(path + name);
    }
    else
    {
        board->setText(path + "/" + name);
    }
}

void MainWindow::keyPressEvent ( QKeyEvent * event )
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        on_search_clicked();
}
