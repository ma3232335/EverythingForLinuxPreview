#include "ctablemodel.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

CTableModel::CTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int CTableModel::rowCount(const QModelIndex &parent) const
{
    return m_table.count();
}

int CTableModel::columnCount(const QModelIndex &parent) const
{
    return RECORD_COLUMNS;
}

QVariant CTableModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QAbstractItemModel::headerData(section, orientation, role);

    switch (section)
    {
    case REC_NAME:
        return "Name";

    case REC_PATH:
        return "Path";

    case REC_SIZE:
        return "Size";

    case REC_MTIME:
        return "Data Modified";

    default:
        return QAbstractItemModel::headerData(section, orientation, role);
    }
}

QVariant CTableModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    int col = index.column();

    /* text alignment in REC_SIZE section*/
    if (role == Qt::TextAlignmentRole && col == REC_SIZE)
    {
        return QVariant(Qt::AlignVCenter | Qt::AlignRight);
    }

    /* display text */
    if (role == Qt::DisplayRole)
    {
        switch(col)
        {
        case (REC_NAME):
            return m_table[row].name;

        case(REC_PATH):
            return m_table[row].path;

        case(REC_SIZE):
            return sizeValue(m_table[row].size);

        case(REC_MTIME):
            return timeValue(m_table[row].time);

        default:
            return QVariant();
        }
    }

    /* icon in REC_NAME section */
    if (role == Qt::DecorationRole && col == REC_NAME)
    {
        QFileInfo fileInfo(m_table[row].path+"/"+m_table[row].name);
        return m_iconProvider.icon(fileInfo);
    }

    return QVariant();
}

QVariant CTableModel::sizeValue(const size_t size) const
{
    return QString("%1 KB").arg(size/1024+1);
}

QVariant CTableModel::timeValue(const time_t timep) const
{
    struct tm *stTime = NULL;
    stTime = localtime(&timep);
    return QString("%1/%2/%3 %4:%5")
            .arg(stTime->tm_year+1900)
            .arg(stTime->tm_mon+1)
            .arg(stTime->tm_mday)
            .arg(stTime->tm_hour)
            .arg(stTime->tm_min);
}

void CTableModel::setKeyword(const QString &key)
{
    m_keyword = key;
    updateTable();
}

void CTableModel::updateTable()
{
    if (m_keyword.isEmpty())
    {
        return;
    }
    removeAllRows();
    doLocate(m_keyword);
}

void CTableModel::removeAllRows()
{
    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    m_table.clear();
    endRemoveRows();
}

void CTableModel::doLocate(const QString& key)
{
    /* get the count of found entries */
    QByteArray cmdCount = QString("locate -b -c %1").arg(key).toLocal8Bit();
    FILE *fp = popen(cmdCount.data(), "r");
    if (fp == NULL)
    {
        return;
    }
    size_t count = 0;
    fscanf(fp, "%ld", &count);
    emit statusBarTextChanged(QString::number(count)); /* set count of entries in status bar */
    fclose(fp);
    fp = NULL;

    /* get every entries */
    QByteArray cmd = QString("locate -b %1").arg(key).toLocal8Bit();
    fp = popen(cmd.data(), "r");
    if (fp == NULL)
    {
        return;
    }
    char buf[1024] = {0};
    while (fscanf(fp, "%[^\n]%*c", buf) != EOF)
    {
        struct stat fileStat;
        stat(buf, &fileStat);
        size_t size = fileStat.st_size;
        time_t time = fileStat.st_mtime;
        char *path = buf;
        char * name = strrchr(buf, '/');
        if (path == name)
            path = "/";

        *name = '\0';
        ++name;

        Record_st rec = {QString::fromUtf8(name), QString::fromUtf8(path), size, time};
        m_table.append(rec);
    }

    /* notify tableview of updating */
    beginInsertRows(QModelIndex(), 0, m_table.size()-1);
    endInsertRows();

    pclose(fp);
}
