#ifndef SQLITE_H
#define SQLITE_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QDebug>

#define DATABASE "database.db"//数据库名

class Sqlite
{
public:
    Sqlite();
    bool Connect();//连接数据库
    bool ExecSQL(QString cmd);//执行Sql语句
    bool Insert(QString table, QString value);//插入
    bool Delete(QString table, QString where);//删除
    bool Updata(QString table, QString value,QString where);//修改
    QSqlQuery Select(QString table, QString value, QString where);//查询
    bool InsertUser(QString cardID, QString name, QString gender, int age, QString tele, bool status);//插入用户表
    bool InsertBooks(QString booksID, QString name, QString author, QString publishing_house, int count, int residue, int borrowed_days);//插入图书表
    bool InsertRecord(QString cardID, QString booksID, QString borrow_time, QString return_time);//插入记录表
    bool DeleteUser(QString cardID = NULL, QString name = NULL, QString gender = NULL, int age = -1);//删除用户表中数据
    bool DeleteBooks(QString booksID = NULL, QString name = NULL, QString author = NULL, QString publishing_house = NULL, int count = -1, int residue = -1);//删除图书表中数据
    bool DeleteRecord(QString cardID = NULL, QString booksID = NULL);//删除记录表中数据
    bool UpdataUser(QString cardID, QString name, QString gender, int age, QString tele, bool status);//修改用户表中信息
    bool UpdataUserStatus(QString cardID, bool status);//修改用户状态
    bool UpdataRecord(QString cardID, QString booksID, QString return_time);//更新还书时间
    bool UpdataBooks(QString booksID, QString name, QString author, QString publishing_house, int count, int residue,int borrowed_days);//修改图书表中信息
    QSqlQuery SelectUser(QString cardID = NULL, QString name = NULL, QString tele = NULL, QString gender = NULL, int age = -1);//查找用户表中信息
    QSqlQuery SelectBooks(QString booksID = NULL, QString book_name = NULL, QString author = NULL, QString press = NULL, int count = -1);//查找图书表中信息
    QSqlQuery SelectRecord(QString cardID = NULL, QString booksID = NULL);
    QSqlQuery SelectBooksOfBorrow(QString cardID);//查找某用户借的书
    bool CreateBookTable();
    bool CreateUserTable();
    bool CreateRecordTable();

    ~Sqlite();

private:
    QSqlDatabase db;
};

#endif // SQLITE_H
