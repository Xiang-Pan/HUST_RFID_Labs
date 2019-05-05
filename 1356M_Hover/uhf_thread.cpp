#include "uhf_thread.h"
#define BUILD_UINT16(loByte, hiByte) \
          ((uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))


UHF_Thread::UHF_Thread(QObject *parent) : QThread(parent)
{
    serialport = new QSerialPort();
    Dll = new M1356Dll();
}
void UHF_Thread::run()
{
    int retryTimes = 45;//超时
    int count = retryTimes;
    enum {UHF_RPC_SOF = 0, UHF_RPC_LEN, UHF_RPC_dev_id, UHF_RPC_CMD, UHF_RPC_STA, UHF_RPC_DAT, UHF_RPC_EOF};
    //帧的标志，数据的长度，长度，指令，状态，数据数组下标
    QByteArray data;
    while(nRunFlag)
    {
        if(serialport->bytesAvailable() >= 4) //只有串口中数据有4个字节时才开始读取
        {
            data = serialport->readAll();//读取全部数据
            if(data.at(0) != (char) 0xAA && data.at(1) != (char)0xBB)//AABB是一帧开始标记
                continue;
            qint16 waitforReadLen = (qint16)((data.at(2) & 0x00FF) + ((data.at(3) & 0x00FF) << 8)) + 4;//计算长度
            while ((waitforReadLen - data.length()) > 0 && count -- > 0 )//count超时时间
            {
                if(serialport->bytesAvailable() == 0)//读不到数据时等待一下，数据到来，继续读取数据
                     QThread::usleep(10);
                else
                {
                    data += serialport->readAll();//读取全部数据
                    count = retryTimes;//超时时间重新计时
                }
            }
            count = retryTimes;//超时时间重新计时
            for(int i = 0 ; i < data.length() ; i ++)
            {
                if(data.at(i) == (char)0xAA && data.at(i+1) == (char)0x00)//0xAA在传输过程中需要加上0x00 读取时需要将0x00去掉
                {
                    data.remove(i + 1,1);
                }
            }

            emit receivedMsg(data);  //receive data signal
        }
        QThread::msleep(1000);
        emit cycle();//循环读取数据
    }
}


bool UHF_Thread::ReadCardID()
{
    last_cmd="ReadCardID";
    uint16 cmd = RC632_CMD_ISO15693_INVENTORY16;//   0x1000
    char *data = (char *)Dll->RC632_SendCmdReq(cmd,NULL,0);//获取读卡指令
//    qDebug()<<last_cmd<<*data<<endl;
    qDebug("%x",cmd);
    if(serialport->write(data+2,data[0]))//data,len
    {
        return true;
    }
    else
    {
        return false;
    }
}


uint8 RC632_UartCalcFCS_Test( uint8 *msg_ptr, uint8 len )
{
    uint8 x;
    uint8 xorResult;
    xorResult = 0;
    for ( x = 0; x < len; x++, msg_ptr++ )
      xorResult = xorResult ^ *msg_ptr;
    return ( xorResult );
}



uint8* RC632_SendCmdReq_Test(uint16 cmd, const uint8 *data, uint16 len)
{
    uint8 *p;
    uint16 frameLen, uartdatalen;
    frameLen = len+9;   // 9 = sop(2)+len(2)+nc(2)+cmd(2)+fcs(1)
    uartdatalen = frameLen;
    for(uint16 i=0; i<len; i++)
        if(data[i] == 0xAA)  uartdatalen++;
    p = new uint8[uartdatalen+2];  // p[0] is used return frameLen
    p[0] = LO_UINT16(uartdatalen);
    p[1] = HI_UINT16(uartdatalen);     //framelen
    p[2] = LO_UINT16(RC632_FRAME_SOP);
    p[3] = HI_UINT16(RC632_FRAME_SOP); //BBAA
    p[4] = LO_UINT16(frameLen-4);
    p[5] = HI_UINT16(frameLen-4);      // 4 = sop(2)+len(2)
    p[6] = 0;
    p[7] = 0;                          // NC
    p[8] = LO_UINT16(cmd);
    p[9] = HI_UINT16(cmd);
    //memcpy(p+10, data, len);
    int k=0;
    for(int j=0; j<len; j++, k++){
        p[10+k] = data[j];
        if(data[j]==0xAA)  p[10+ ++k] = 0x00;
    }
    p[10+k] = RC632_UartCalcFCS_Test(p+5, uartdatalen-4);

    return p;
}



bool UHF_Thread::SetCardRecord(QString userID,QString bookID)
{
    last_cmd="RC632_CMD_ISO15693_WRITE_SM";
    userID="46F87856000104E0";
    bookID="46F87856000104E0";
    //02
    uint16 cmd = RC632_CMD_ISO15693_WRITE_SM;  //写单个块
    uint8* vdata1;
    vdata1 = new uint8[14];
    vdata1[0]=0x02;  //cut
    memcpy(vdata1+1,userID.toLatin1().data(),8);    //write ID
    vdata1[9]=0x02;  //cut
    memcpy(vdata1+10,bookID.toLatin1().data(),4);   //record ID
    char *p1 = (char *)Dll->RC632_SendCmdReq (cmd,vdata1,14);

    //03
    uint8* vdata2;
    vdata2 = new uint8[14];
    vdata2[0]=0x02;  //cut
    memcpy(vdata2+1,userID.toLatin1().data(),8);    //write ID
    vdata2[9]=0x02;  //address
    memcpy(vdata2+10,bookID.toLatin1().data()+4,4);   //record ID
    char *p2= (char *)Dll->RC632_SendCmdReq (cmd,vdata2,14);

    if(serialport->write((char*)(p1 + 2), BUILD_UINT16(p1[0], p1[1])))//data,len
    {
        serialport->write((char*)(p2 + 2), BUILD_UINT16(p2[0], p2[1]));
        return true;
    }
    else
    {
        return false;
    }
}


bool UHF_Thread::GetCardRecord(QString userID,QString bookID)
{
    last_cmd="RC632_CMD_ISO15693_READ_SM";
    uint16 cmd = RC632_CMD_ISO15693_READ_SM;  //写单个块
    uint8* vdata1;
    vdata1 = new uint8[14];
    vdata1[0]=0x02;  //cut
    memcpy(vdata1+1,userID.toLatin1().data(),8);    //read ID
    vdata1[9]=0x02;  //address
    vdata1[10]=0x02; // read block num
    char *p1 = (char *)Dll->RC632_SendCmdReq (cmd,vdata1,14);
    if(serialport->write((char*)(p1 + 2), BUILD_UINT16(p1[0], p1[1])))//data,len
    {
        return true;
    }
    else
    {
        return false;
    }
}





bool UHF_Thread::InitUhf()
{
    uint8 data = RC632_WORK_MODEL_15693;
    uint16 cmd = RC632_CMD_CONFIG_ISOTYPE;
    char *senddata = (char *)Dll->RC632_SendCmdReq(cmd,&data,1);    //设置为15693类型
    for(int i = 0;i<15;i++)
    {
        qDebug("%x",senddata[i]);
    }

    if(serialport->write(senddata+2,senddata[0]))//data,len
    {
        qDebug("WRITE SUCCESS!");
    }
    else
    {
        qDebug("WRITE FAILED!");
    }
    return true;
}

bool UHF_Thread::UART_Disconnect()
{
    serialport->close();
    return true;
}

//连接串口
bool UHF_Thread::UART_Connect(QString ComName,int Baudrate)
{
    serialport->setPortName(ComName);    //端口号
    serialport->setBaudRate(Baudrate);    //波特率
    serialport->setDataBits(QSerialPort::Data8);//数据位
    serialport->setParity(QSerialPort::NoParity);//奇偶校验
    serialport->setStopBits(QSerialPort::OneStop);//停止位
    serialport->setFlowControl(QSerialPort::NoFlowControl);//流控制
    if (serialport->open(QIODevice::ReadWrite))//以读写方式打开
    {
        qDebug("OPNE SUCCESS!");
        return true;
    }
    else
    {
        qDebug("OPNE FAILED!");
        return false;
    }
}

