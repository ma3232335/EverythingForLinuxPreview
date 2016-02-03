#ifndef CTABLEMODEL_H
#define CTABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QThread>
#include <QFileIconProvider>

enum{
    REC_NAME = 0,
    REC_PATH,
    REC_SIZE,
    REC_MTIME,
    RECORD_COLUMNS
};

struct Record_st
{
    QString name;
    QString path;
    size_t size;
    time_t time;
};

class CTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit CTableModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;


protected:


private:
    void updateTable();
    void doLocate(const QString& key);
    QVariant timeValue(const time_t timep) const;
    QVariant sizeValue(const size_t size) const;
    void removeAllRows();

signals:
    void statusBarTextChanged(const QString &text);

public slots:
    void setKeyword(const QString& key);

private:
    QVector<Record_st> m_table;
    QString m_keyword;
    QFileIconProvider m_iconProvider;
};

#endif // CTABLEMODEL_H
