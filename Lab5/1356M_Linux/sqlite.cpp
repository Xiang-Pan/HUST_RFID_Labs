#include "sqlite.h"
#include<iostream>
Sqlite::Sqlite()
{
}
/*连接数据库*/
bool Sqlite::Connect()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DATABASE);
    if(!db.open())
    {
        qDebug() << " connect failed!";
        return false;
    }
    else
    {
        qDebug() << "connect success!";
    }
    CreateUserTable();
    CreateBookTable();
    CreateRecordTable();
    return true;
}
//打印SQL语句
bool Sqlite::ExecSQL(QString cmd)
{
    QSqlQuery query;
    qDebug()<<cmd.toUtf8().data();
    return query.exec(cmd);
}
//添加语句
bool Sqlite::Insert(QString table, QString value)
{
    QString cmd = "insert into " + table + " values(" + value + ");";
    qDebug() << "SQL: " << cmd;
    return ExecSQL(cmd);
}
//删除语句
bool Sqlite::Delete(QString table, QString where)
{
    QString cmd = "delete from " + table + " where " + where + ";";
    return ExecSQL(cmd);
}
//修改语句
bool Sqlite::Updata(QString table, QString value,QString where)
{
    QString cmd = "update " + table + " set " + value +" where " + where + ";";
    return ExecSQL(cmd);
}
//查询语句
QSqlQuery Sqlite::Select(QString table, QString value, QString where)
{
    QString cmd;
    if(where.isEmpty())
    {
        cmd = "select " + value + " from " + table + ";";
    }
    else
    {
        cmd = "select " + value + " from " + table + " where " + where + ";";
    }
    QSqlQuery query;
    qDebug()<<cmd.toUtf8();
    query.exec(cmd);
    return query;
}
//向user表中添加
bool Sqlite::InsertUser(QString cardID, QString name, QString gender, int age, QString tele, bool status)
{
    return Insert("user_15693", "'"+cardID+"', '"+name+"', '"+gender+"', "+QString::number(age)+", '"+tele+"', "+QString::number(status));
}
//向books表中添加
bool Sqlite::InsertBooks(QString booksID, QString name, QString author, QString publishing_house, int count, int residue, int borrowed_days)
{
    return Insert("books_15693", "'"+booksID+"', '"+name+"', '"+author+"', '"+publishing_house+"', "+QString::number(count)+", "+QString::number(residue)+", "+QString::number(borrowed_days));
}
//向record表中添加
bool Sqlite::InsertRecord(QString cardID, QString booksID, QString borrow_time, QString return_time)
{
    return Insert("record_15693", "'"+cardID+"', '"+booksID+"','"+ borrow_time+"','"+ return_time+"'");
    //return Insert("record_15693", "'"+cardID+"', '"+booksID+"'");
}

//删除user表中数据
bool Sqlite::DeleteUser(QString cardID, QString name, QString gender, int age)
{    
    QString where;
    if( !cardID.isEmpty() )
        where += ("cardID = '" + cardID +"' ");
    if( !name.isEmpty() )
    {
        if(where.isEmpty())
            where += ("name = '" + name+"' ");
        else
            where += ("and name = '" + name+"' ");
    }
    if( !gender.isEmpty() )
    {
        if(where.isEmpty())
            where += ("gender = '" + gender+"' ");
        else
            where += ("and gender = '" + gender+"' ");
    }
    if( age != -1 )
    {
        if(where.isEmpty())
            where += ("age = " + QString::number(age));
        else
            where += ("and age = " + QString::number(age));
    }
    return Delete("user_15693", where);
}
//删除books表中数据
bool Sqlite::DeleteBooks(QString booksID, QString name, QString author, QString publishing_house, int count, int residue)
{
    QString where;
    if( !booksID.isEmpty() )
        where += ("booksID = '" + booksID +"' ");
    if( !name.isEmpty() )
    {
        if(where.isEmpty())
            where += ("name = '" + name+"' ");
        else
            where += ("and name = '" + name+"' ");
    }
    if( !author.isEmpty() )
    {
        if(where.isEmpty())
            where += ("author = '" + author+"' ");
        else
            where += ("and author = '" + author+"' ");
    }
    if( !publishing_house.isEmpty() )
    {
        if(where.isEmpty())
            where += ("publishing_house = '" + publishing_house+"' ");
        else
            where += ("and publishing_house = '" + publishing_house+"' ");
    }
    if( count != -1 )
    {
        if(where.isEmpty())
            where += ("count = " + QString::number(count)+" ");
        else
            where += ("and count = " + QString::number(count)+" ");
    }
    if( residue != -1 )
    {
        if(where.isEmpty())
            where += ("residue = " + QString::number(residue)+" ");
        else
            where += ("and residue = " + QString::number(residue)+" ");
    }
    return Delete("books_15693", where);
}
//删除record表中数据
bool Sqlite::DeleteRecord(QString cardID, QString booksID)
{
    QString where;
    if( !cardID.isEmpty() )
        where += ("cardID = '" + cardID +"' ");
    if( !booksID.isEmpty() )
    {
        if(where.isEmpty())
            where += ("booksID = '" + booksID+"' ");
        else
            where += ("and booksID = '" + booksID+"' ");
    }
    return Delete("record_15693", where);
}
//修改user表中数据
bool Sqlite::UpdataUser(QString cardID, QString name, QString gender, int age, QString tele, bool status)
{
    return Updata("user_15693","cardID = '"+cardID+"', name = '"+name+"', gender = '"+gender+"', age = "+QString::number(age)+"', tele = '"+tele+", status = "+QString::number(status), "cardID = '"+cardID+"'");
}
//修改用户状态
bool Sqlite::UpdataUserStatus(QString cardID, bool status)
{
    return Updata("user_15693","status = "+QString::number(status),"cardID = '"+cardID+"'");
}

bool Sqlite::UpdataRecord(QString cardID, QString booksID, QString return_time)//更新还书时间
{
    return Updata("record_15693", "return_time = '"+return_time+"'","cardID = '" +cardID+"' and booksID = '"+booksID+"'");
    //update record_15693 set return_time = '20' where cardID = '5A0C6B2C000104E0' and booksID = 'E5EA6A2C000104E0';
}

//修改books表中数据
bool Sqlite::UpdataBooks(QString booksID, QString name, QString author, QString publishing_house, int count, int residue,int borrowed_days)
{
    return Updata("books_15693","booksID = '"+booksID+"', book_name = '"+name+"', author = '"+author+"', press = '"+publishing_house+"', count = "+QString::number(count)+", residue = "+QString::number(residue) +", borrowed_days = "+QString::number(borrowed_days), "booksID = '"+booksID+"'");
}
//查询user表中数据
QSqlQuery Sqlite::SelectUser(QString cardID, QString name, QString tele, QString gender, int age)
{
    QString where;
    if( !cardID.isEmpty() )
        where += ("cardID = '" + cardID +"' ");
    if( !name.isEmpty() )
    {
        if(where.isEmpty())
            where += ("name = '" + name+"' ");
        else
            where += ("and name = '" + name+"' ");
    }
    if( !tele.isEmpty() )
    {
        if(where.isEmpty())
            where += ("tele = '" + tele+"' ");
        else
            where += ("and tele = '" + tele+"' ");
    }
    if( !gender.isEmpty() )
    {
        if(where.isEmpty())
            where += ("gender = '" + gender+"' ");
        else
            where += ("and gender = '" + gender+"' ");
    }
    if( age != -1 )
    {
        if(where.isEmpty())
            where += ("age = " + QString::number(age));
        else
            where += ("and age = " + QString::number(age));
    }

    return Select("user_15693", "*", where);
}

//查询books表中数据
QSqlQuery Sqlite::SelectBooks(QString booksID, QString book_name, QString author, QString press, int count)
{
    QString where;
    if( !booksID.isEmpty() )
        where += ("booksID = '" + booksID +"' ");
    if( !book_name.isEmpty() )
    {
        if(where.isEmpty())
            where += ("book_name = '" + book_name+"' ");
        else
            where += ("and book_name = '" + book_name+"' ");
    }
    if( !author.isEmpty() )
    {
        if(where.isEmpty())
            where += ("author = '" + author+"' ");
        else
            where += ("and author = '" + author+"' ");
    }
    if( !press.isEmpty() )
    {
        if(where.isEmpty())
            where += ("press = '" + press+"' ");
        else
            where += ("and press = '" + press+"' ");
    }
    if( count != -1 )
    {
        if(where.isEmpty())
            where += ("count = " + QString::number(count));
        else
            where += ("and count = " + QString::number(count));
    }

    return Select("books_15693", "*", where);
}




QSqlQuery Sqlite::SelectRecord(QString cardID, QString booksID)
{
    QString where;
    if( !cardID.isEmpty() )
        where += ("cardID = '" + cardID +"' ");
    if( !booksID.isEmpty() )
    {
        if(where.isEmpty())
            where += ("booksID = '" + booksID+"' ");
        else
            where += ("and booksID = '" + booksID+"' ");
    }
    return Select("record_15693", "*", where);
}

//查找借的书
QSqlQuery Sqlite::SelectBooksOfBorrow(QString cardID)
{
    return Select("books_15693", "*", "booksID in (select booksID from record_15693 where cardID = '"+cardID+"')");
}
 bool Sqlite::CreateBookTable()
 {
     QSqlQuery query(db);
     //用户基本信息表
     //staus -0表示未激活，-1表示已激活
     bool isTableExist = query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg("books_15693"));
     if(isTableExist)
     {
         qDebug() << "books_table has existed!";
         //return false;
     }
     bool res = false;
     //图书基本信息表
     //图书名字和ID都表示一类书,borrowed_days 可借出的时常
     res = query.exec("create table books_15693 (booksID vchar,  book_name vchar, author vchar, press vchar,  count int, residue int, borrowed_days int, primary key (booksID))");
     if(res)
         qDebug() << "create table books_15693 success!";
     else
         qDebug() << "create table books_15693 fail!";
     return res;
 }
 bool Sqlite::CreateUserTable()
 {
     QSqlQuery query(db);
     //用户基本信息表
     //staus -0表示未激活，-1表示已激活
     bool isTableExist = query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg("user_15693"));
     if(isTableExist)
     {
         qDebug() << "books_table has existed!";
         //return false;
     }
     bool res = false;
     res = query.exec("create table user_15693 (cardID vchar, name vchar, gender vchar, age int, tele char(11), status boolean, primary key (cardID))");
     if(res)
         qDebug() << "create table user_15693 success!";
     else
         qDebug() << "create table user_15693 fail!";
     return res;
 }
 bool Sqlite ::CreateRecordTable()
 {
     QSqlQuery query(db);
     //用户基本信息表
     //staus -0表示未激活，-1表示已激活
     bool isTableExist = query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg("record_15693"));
     if(isTableExist)
     {
         qDebug() << "record_table has existed!";
         //return false;
     }
     bool res = false;

     res = query.exec("create table record_15693 (cardID vchar, booksID vchar, borrow_time timestamp not null default current_timestamp, return_time timestamp default null,FOREIGN KEY (cardID ) REFERENCES user(cardID), FOREIGN KEY (booksID ) REFERENCES user(booksID))");
     if(res)
         qDebug() << "create table record_15693 success!";
     else
         qDebug() << "create table record_15693 fail!";
     return res;
 }
