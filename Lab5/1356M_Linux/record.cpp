#include "record.h"

//还书界面
Record::Record(QWidget *parent) : QWidget(parent)
{    
    QVBoxLayout *MainLayout = new QVBoxLayout();//主布局
    QHBoxLayout *TableLayout = new QHBoxLayout();//表格布局
    QHBoxLayout *ButtonLayout = new QHBoxLayout();//按钮布局
    QHBoxLayout *EditLayout = new QHBoxLayout();//按钮布局
    QVBoxLayout *TopLayout = new QVBoxLayout();//上部布局
    QStringList LabelText,ButtonText;
    ButtonText<<"搜索"<<"删除";
    for(int i=0; i<Button_Count_Record; i++)
    {
        Button[i] = new QPushButton();
        Button[i]->setText(ButtonText.at(i));
        ButtonLayout->addWidget(Button[i]);
    }
    ButtonLayout->addStretch();
    LabelText<<"用户卡号"<<"书籍卡号";
    QString pattern("[A-Fa-f9-0]*");
    QRegExp regExp(pattern);
    for(int i=0; i<Edit_Count_Record; i++)
    {
        Label[i] = new QLabel();
        Label[i]->setText(LabelText.at(i));
        EditLayout->addWidget(Label[i]);
        Edit[i] = new QLineEdit();
        EditLayout->addWidget(Edit[i]);
        Edit[i]->setValidator(new QRegExpValidator(regExp, this));
    }
    //组合框
    QGroupBox *TabGroupBox = new QGroupBox();
    QGroupBox *GroupBox = new QGroupBox();
    sql = new Sqlite();

    Table = new QTableWidget();//表格
    Table->setColumnCount(Table_Column_Record);//设置列数
    Table->setSelectionBehavior ( QAbstractItemView::SelectRows);//选中整行
    Table->setEditTriggers ( QAbstractItemView::NoEditTriggers );//不可编辑
    Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//列宽度自适应

    TableLayout->addWidget(Table);
    TopLayout->addLayout(EditLayout);
    GroupBox->setLayout(TopLayout);
    TabGroupBox->setTitle("借书列表");//设置组合框标题
    TabGroupBox->setLayout(TableLayout);//这是组合框布局

    //设置布局
    MainLayout->addWidget(GroupBox);
    MainLayout->addLayout(ButtonLayout);
    MainLayout->addWidget(TabGroupBox);
    this->setLayout(MainLayout);
    SetSlot();
}

void Record::SetSlot()
{
    connect(Button[Delete_Record],SIGNAL(clicked()),this,SLOT(delete_record()));//删除按钮连接槽函数delete_Record()
    connect(Button[Select_Record],SIGNAL(clicked()),this,SLOT(select_record()));//查找按钮连接槽函数select_Record()
    connect(Table,SIGNAL(cellClicked(int,int)),this,SLOT(get_table_line(int, int)));//表格单击事件连接槽函数get_table_line(int, int)
}


void checkReturnDate(QString userID)
{
    qDebug()<<"checkReturnDate"<<"borrow and return"<<endl;
    QSqlQuery query;
    userID="999";
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
        if(left_days<warnning_days)
        {
            QMessageBox::warning(NULL, "还书警告！", "书籍："+bookID+"\n"+"还书日期："+dueDate.toString()+"\n"+"剩余时间："+QString::number(left_days, 10)+"Days\n", QMessageBox::Yes, QMessageBox::Yes);
        }
        qDebug()<<bookID<<borrowDate<<borrowed_days<<endl;
//        bookID=query.value(0)
    }while(query.next());
}



//搜索按钮单击事件
void Record::select_record()
{
    QSqlQuery query;
    query = sql->SelectRecord(Edit[UserID_Record]->text(),Edit[BookID_Record]->text());
    checkReturnDate(Edit[UserID_Record]->text());
    ShowTable(query);//更新表格
    ClearEdit();//清空文本框
}

//删除按钮槽函数
void Record::delete_record()
{
    //删除书籍
    bool ret = sql->DeleteRecord(Edit[UserID_Record]->text(),Edit[BookID_Record]->text());
    if(!ret)
    {
        QMessageBox::warning(NULL, "warning", "删除失败！", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    QMessageBox::warning(NULL, "warning", "删除成功！", QMessageBox::Yes, QMessageBox::Yes);
    ClearEdit();//清空文本框
    ShowTable(sql->SelectRecord());//更新表格
}

//清空文本框
void Record::ClearEdit()
{
    for(int i = 0; i < Edit_Count_Record; i++)
    {
        Edit[i]->clear();
    }
}

//单击表格 在文本框中显示表格点击的行的数据
void Record::get_table_line(int row, int col)
{
    for(int i = 0; i < Edit_Count_Record; i++)
    {
        Edit[i]->setText(Table->item(row,i)->text());
    }
}
//显示表格
void Record::ShowTable(QSqlQuery query)
{
    //表头
    Table->setHorizontalHeaderLabels(QStringList()<<"用户卡号"<<"书籍卡号"<<"借阅时间"<<"还书时间");
    if(!query.next())
    {
        Table->setRowCount(0);//表格设置行数
        if(!Edit[UserID_Record]->text().isEmpty())
        {
            QMessageBox::warning(NULL, "warning", "当前用户无借阅记录！", QMessageBox::Yes, QMessageBox::Yes);
        }
        return;
    }
    /*计算record表中数据行数*/
    query.last();//跳转到最后一条数据
    int nRow = query.at() + 1;//取所在行数
    Table->setRowCount(nRow);//表格设置行数
    int row = 0;
    query.first();//返回第一条数据
    do
    {
        for(int col = 0; col < Table->columnCount(); col++)
        {
            Table->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));//显示信息
        }
        row++;
    }while(query.next());
}

//清空文本框和表格
void Record::Clear()
{
    ShowTable(sql->SelectRecord());
}

//设置卡号
void Record::SetCard(QString cardID)
{
    QSqlQuery query = sql->SelectUser(cardID);
    if(query.next())//如果是用户
    {
        Edit[UserID_Record]->setText(cardID);//显示用户卡号
        return;
    }
    query = sql->SelectBooks(cardID);
    if(query.next())//如果是书
    {
        Edit[BookID_Record]->setText(cardID);//显示用户卡号
    }
}
