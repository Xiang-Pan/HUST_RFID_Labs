#include "borrow_return.h"

//借书界面
Borrow_Return::Borrow_Return(QWidget *parent) : QWidget(parent)
{
    tools = new Tools();
    QString LabelNameUser[] = {"卡号：", "姓名：", "性别：", "年龄："}; //标签文本

    //布局
    QGridLayout *MainLayout = new QGridLayout();//主布局
    QVBoxLayout *UserLayout = new QVBoxLayout();//用户区域布局
    QVBoxLayout *RightLayout = new QVBoxLayout();//右侧布局
    QHBoxLayout *ButtonLayout = new QHBoxLayout();//右侧布局

    //组合框
    QGroupBox *BooksGroupBox = new QGroupBox();
    QGroupBox *UserGroupBox = new QGroupBox();

    sql = new Sqlite();

    //初始化文本框和标签 将文本框和标签添加到布局中
    for(int i = 0; i < Edit_Count_BORROW_RETURN; i++)
    {
        QHBoxLayout *Layout = new QHBoxLayout();
        Edit_User[i] = new QLineEdit();
        Label_User[i] = new QLabel(LabelNameUser[i]);
        //Edit_User[i]->setFocusPolicy(Qt::NoFocus); //设置为禁止编辑
        Layout->addWidget(Label_User[i]);
        Layout->addWidget(Edit_User[i]);
        UserLayout->addLayout(Layout);
    }

    //借还书单选按钮
    Borrow = new QRadioButton("借书");
    Return = new QRadioButton("还书");
    Borrow->setChecked(true);
    Function = new QButtonGroup();
    Function->addButton(Borrow);//单选按钮加入按钮组
    Function->addButton(Return);

    ButtonLayout->addWidget(Borrow);
    ButtonLayout->addWidget(Return);
    UserLayout->addLayout(ButtonLayout);
    UserGroupBox->setTitle("用户信息");//设置标题
    UserGroupBox->setLayout(UserLayout);
    UserGroupBox->setFixedSize(200,300);//设置大小

    Table = new QTableWidget();//表格
    Table->setColumnCount(Table_Column_BORROW_RETURN);//设置列数
    Table->setSelectionBehavior ( QAbstractItemView::SelectRows);//选择方式为选中整行
    Table->setEditTriggers ( QAbstractItemView::NoEditTriggers );//不可编辑
    Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//列宽度自适应

    RightLayout->addWidget(Table);

    BooksGroupBox->setTitle("借书列表");//设置组合框标题
    BooksGroupBox->setLayout(RightLayout);

    /*设置图片*/
    QLabel *Picture = new QLabel();
    QImage *jpg = new QImage(":/img/img/book.jpg");
    Picture->setPixmap(QPixmap::fromImage(*jpg));

    MainLayout->addWidget(UserGroupBox,0,0,1,1);
    MainLayout->addWidget(BooksGroupBox,0,1,2,1);
    MainLayout->addWidget(Picture,1,0,1,1);
    MainLayout->setSpacing(20);
    this->setLayout(MainLayout);

    // for test
    QString borrow_time = tools->CurrentDateTime();
    sql->InsertRecord("999", "123",borrow_time,NULL);


    //for test

    SetInfo("999");



}
//表格显示
void Borrow_Return::ShowTable(QSqlQuery query)
{
    //设置表格表头
    Table->setHorizontalHeaderLabels(QStringList()<<"卡号"<<"书名"<<"作者"<<"出版社"<<"总数（本）"<<"剩余（本）"<<"可借阅时间（天）");
    if(!query.next())
    {
        Table->setRowCount(0);//表格设置行数
        return;
    }
    /*计算record表中数据行数*/
    query.last();//跳转到最后一条数据
    int nRow = query.at() + 1;//取所在行数
    Table->setRowCount(nRow);//表格设置行数
    int row = 0;
    int skip_num=0;
    query.first();//返回第一条数据
    do
    {
        QSqlQuery myquery=sql->SelectRecord(Edit_User[CardId_User_Borrow]->text(), query.value(0).toString());
        bool skip=true;
        while(myquery.next())
        {
            //QMessageBox::warning(NULL, "warning", myquery.value(3).toString(), QMessageBox::Yes, QMessageBox::Yes);
            if(myquery.value(3).toString()=="")
            {
                //QMessageBox::warning(NULL, "warning", "不为空！", QMessageBox::Yes, QMessageBox::Yes);
                skip=false;
            }

        }
        if(skip)
        {
            skip_num++;
            continue;
        }
        for (int col = 0; col<7; col++)//按字段添加数据
        {
            //表格中添加数据库中的数据
            Table->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));

        }
        row++;//行数增加
    }while(query.next());
    if((nRow-skip_num)==0)
    {
        QMessageBox::warning(NULL, "warning", "无查询记录", QMessageBox::Yes, QMessageBox::Yes);
    }
    Table->setRowCount(nRow-skip_num);//表格设置行数

//    while (1)
//    {
//        SetInfo()

//    }
}

void Borrow_Return::checkReturnDate(QString userID)
{
    if(userID==last_notification_ID)
    {
        return;
    }
    qDebug()<<"checkReturnDate"<<"borrow and return"<<endl;
    QSqlQuery query;
    //userID="999";
    QString cmd="select\
    record_15693.booksID,\
    record_15693.borrow_time ,\
    books_15693.borrowed_days\
    from record_15693\
    left join  books_15693 on record_15693.booksID = books_15693.booksID\
    where record_15693.cardID=='"+userID+"'   \
    and record_15693.return_time is NULL";
    query.exec(cmd);
    if(!query.next())
    {
        //no result
        qDebug()<<"no result"<<endl;
        return;
    }
    qDebug()<<query.size()<<endl;
    QString bookID;
    QDateTime borrowDate;
    int borrowed_days;
    int warnning_days=10;
    QDateTime dueDate;
    QDateTime nowDate=QDateTime::currentDateTime();

    query.first();//返回第一条数据
    do
    {
        //check numd;
        qDebug()<<query.value(0)<<query.value(1)<<query.value(2)<<endl;
        bookID=query.value(0).toString();
        borrowDate = QDateTime::fromString(query.value(1).toString(), "yyyy-MM-dd hh:mm:ss");
        borrowed_days=query.value(2).toInt();
        dueDate=borrowDate.addDays(borrowed_days);
        int ndaysec = 24*60*60;
        uint stime = nowDate.toTime_t();
        uint etime = dueDate.toTime_t();
        uint left_days=(etime - stime)/(ndaysec) + ((etime - stime)%(ndaysec)+(ndaysec-1))/(ndaysec) - 1;
        qDebug() << "Day = " <<left_days<<endl;
        if(left_days<0)
        {
            QMessageBox::warning(NULL, "逾期警告！", "书籍："+bookID+"\n"+"还书日期："+dueDate.toString()+"\n"+"逾期时间："+QString::number(-left_days, 10)+"Days\n", QMessageBox::Yes, QMessageBox::Yes);
        }
        else if(left_days<warnning_days)
        {
            QMessageBox::warning(NULL, "还书警告！", "书籍："+bookID+"\n"+"还书日期："+dueDate.toString()+"\n"+"剩余时间："+QString::number(left_days, 10)+"Days\n", QMessageBox::Yes, QMessageBox::Yes);
        }

        qDebug()<<bookID<<borrowDate<<borrowed_days<<endl;
//        bookID=query.value(0)
    }while(query.next());
    last_notification_ID=userID;
}


bool canBorrow(QString userID)
{
    qDebug()<<"checkReturnDate"<<"borrow and return"<<endl;
    QSqlQuery query;
    QString cmd="select\
    record_15693.booksID,\
    record_15693.borrow_time ,\
    books_15693.borrowed_days\
    from record_15693\
    left join  books_15693 on record_15693.booksID = books_15693.booksID\
    where record_15693.cardID=='"+userID+"'   ";
    query.exec(cmd);
    if(!query.next())
    {
        //no result
        qDebug()<<"no result"<<endl;
        return true;
    }
    qDebug()<<query.size()<<endl;
    QString bookID;
    QDateTime borrowDate;
    int borrowed_days;
    int warnning_days=10;
    QDateTime dueDate;
    QDateTime nowDate=QDateTime::currentDateTime();
    bool can_borrow=true;
    query.first();//返回第一条数据
    do
    {
        //check numd;
        qDebug()<<query.value(0)<<query.value(1)<<query.value(2)<<endl;
        bookID=query.value(0).toString();
        borrowDate = QDateTime::fromString(query.value(1).toString(), "yyyy-MM-dd hh:mm:ss");
        borrowed_days=query.value(2).toInt();
        dueDate=borrowDate.addDays(borrowed_days);
        int ndaysec = 24*60*60;
        uint stime = nowDate.toTime_t();
        uint etime = dueDate.toTime_t();
        uint left_days=(etime - stime)/(ndaysec) + ((etime - stime)%(ndaysec)+(ndaysec-1))/(ndaysec) - 1;
        qDebug() << "Day = " <<left_days<<endl;
        if(left_days<0)
        {
            can_borrow=false;
            //QMessageBox::warning(NULL, "逾期警告！", "书籍："+bookID+"\n"+"还书日期："+dueDate.toString()+"\n"+"逾期时间："+QString::number(-left_days, 10)+"Days\n", QMessageBox::Yes, QMessageBox::Yes);
        }
        qDebug()<<bookID<<borrowDate<<borrowed_days<<endl;
    }while(query.next());
    return can_borrow;
}



//设置用户信息(卡ID)
void Borrow_Return::SetInfo(QString cardID)
{
    //将用户信息显示到文本框中
    QSqlQuery query = sql->SelectUser(cardID);
    if(query.next())//如果是用户
    {
        checkReturnDate(cardID);
        //检查卡是否已被挂失
        if(query.value(5) == 0)
        {
            QMessageBox::warning(NULL, "warning", "此卡已被挂失,请联系管理员！", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
        for(int i=0; i < Edit_Count_BORROW_RETURN; i++)
        {
            Edit_User[i]->setText(query.value(i).toString());
        }

        //check if there are any books need to be return


        //将书信息显示到表格中
        ShowTable(sql->SelectBooksOfBorrow(cardID));//显示表格内容
        return;
    }




    query = sql->SelectBooks(cardID);
    if(query.next())//如果是书
    {
        if(Edit_User[CardId_User_Borrow]->text().isEmpty())
        {
            return;
        }
        if(Borrow->isChecked())
        {
            //check user credit
            bool can_borrow=canBorrow(Edit_User[CardId_User_Borrow]->text());
            if(!can_borrow)
            {
                QMessageBox::warning(NULL, "warning", "存在逾期书籍，请先还书！", QMessageBox::Yes, QMessageBox::Yes);
                return;
            }
            QSqlQuery myquery=sql->SelectRecord(Edit_User[CardId_User_Borrow]->text(), query.value(0).toString());
            while(myquery.next())
            {
                if(myquery.value(3).toString()=="")
                {
                    QMessageBox::warning(NULL, "warning", "借书错误,该书已借！", QMessageBox::Yes, QMessageBox::Yes);
                    return;
                }
            }

            //剩余书为0，不能再借
            if(query.value(5).toInt() <= 0)
            {
                return;
            }
            QString borrow_time = tools->CurrentDateTime();
            //借书
            //set RFID Record
            set_record(Edit_User[CardId_User_Borrow]->text(), query.value(0).toString());
            //widget->Set
            if(sql->InsertRecord(Edit_User[CardId_User_Borrow]->text(), query.value(0).toString(),borrow_time,NULL))//将用户ID和书籍编号添加到数据表中
            {
                //书籍的剩余数量-1
                sql->UpdataBooks(query.value(0).toString(),query.value(1).toString(),query.value(2).toString(),query.value(3).toString(),query.value(4).toInt(),query.value(5).toInt()-1,query.value(6).toInt());
            }
        }
        //还书按钮被选择
        else
        {
            //查询不到借书记录
            if(!sql->SelectRecord(Edit_User[CardId_User_Borrow]->text(), query.value(0).toString()).next())
            {
                QMessageBox::warning(NULL, "warning", "无借书记录！", QMessageBox::Yes, QMessageBox::Yes);
                return;
            }
            QSqlQuery myquery=sql->SelectRecord(Edit_User[CardId_User_Borrow]->text(), query.value(0).toString());
            bool can_return;
            while(myquery.next())
            {
                if(myquery.value(3).toString()=="")
                {
                    can_return=true;
                }
            }
            if(!can_return)
            {
                QMessageBox::warning(NULL, "warning", "还书错误,该书已还！", QMessageBox::Yes, QMessageBox::Yes);
                return;
            }
            QMessageBox::warning(NULL, "warning",myquery.value(0).toString()+myquery.value(1).toString()+myquery.value(2).toString()+myquery.value(3).toString()+myquery.value(4).toString()+myquery.value(5).toString()+myquery.value(6).toString(), QMessageBox::Yes, QMessageBox::Yes);
//            QString sql_cmd="SELECT * FROM record_15693 WHERE cardID = "+Edit_User[CardId_User_Borrow]->text()+" AND booksID = "+query.value(0).toString();//+" AND return_time IS NULL"
//            QSqlQuery myquery;
//            myquery.exec(sql_cmd);
//            if(myquery.first())//query is empty
//            {
//                QMessageBox::warning(NULL, "warning",myquery.value(0).toString()+myquery.value(1).toString()+myquery.value(2).toString()+myquery.value(3).toString()+myquery.value(4).toString()+myquery.value(5).toString()+myquery.value(6).toString(), QMessageBox::Yes, QMessageBox::Yes);
//                QMessageBox::warning(NULL, "warning", "还书错误！", QMessageBox::Yes, QMessageBox::Yes);
//                return;
//            }
//            else
//            {
//                QMessageBox::warning(NULL, "warning",myquery.value(0).toString()+myquery.value(1).toString()+myquery.value(2).toString()+myquery.value(3).toString()+myquery.value(4).toString()+myquery.value(5).toString()+myquery.value(6).toString(), QMessageBox::Yes, QMessageBox::Yes);
//                //QMessageBox::warning(NULL, "warning",query.value(0).toString(), QMessageBox::Yes, QMessageBox::Yes);
//            }
//            if(query.value(2).toString()=="")
//            {
//                QMessageBox::warning(NULL, "warning", "还书错误！", QMessageBox::Yes, QMessageBox::Yes);
//                return;
//            }

            QString return_time = tools->CurrentDateTime();
            if(sql->UpdataRecord(Edit_User[CardId_User_Borrow]->text(), query.value(0).toString(),return_time))//更新还书日期
            //if(sql->DeleteRecord(Edit_User[CardId_User_Borrow]->text(), query.value(0).toString()))//将用户ID和书籍编号添加到数据表中
            {
                //书籍的剩余数量+1
                sql->UpdataBooks(query.value(0).toString(),query.value(1).toString(),query.value(2).toString(),query.value(3).toString(),query.value(4).toInt(),query.value(5).toInt()+1,query.value(6).toInt());
            }
        }
        ShowTable(sql->SelectBooksOfBorrow(Edit_User[0]->text()));//显示表格内容
    }
}

//清空文本框和刷新表格

void Borrow_Return::Clear()
{
    for(int i = 0; i < Edit_Count_BORROW_RETURN; i++)
    {
        Edit_User[i]->clear();
    }
    ShowTable(sql->SelectBooksOfBorrow(Edit_User[0]->text()));//显示表格内容
}
