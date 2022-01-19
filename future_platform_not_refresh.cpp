/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: future_platform.cpp
Version: 1.0
Date: 2017.4.25

History:
shengkaishan     2017.4.25   1.0     Create
******************************************************************************/

#include "future_platform.h"
#include "md_server.h"
#include "trader_server.h"
#include "common.h"
#include <QtWidgets/QToolBar>
#include "applog.h"
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/QTableWidget>
#include <iomanip>
#include "Quote.h"
extern CRITICAL_SECTION cs;

namespace future
{

    future_platform::future_platform(QWidget *parent)
        : QMainWindow(parent),
        q_set_dialog_(MD_TYPE),
        t_set_dialog_(TRADER_TYPE)
    {
        m_table_row = 1;
        ismd_login = 0;
        istrade_login = 0;
        firstcreate = 0;
        newwin = NULL;
        fillwin = NULL;
        staterylist = NULL;
        //ui.setupUi(this);
        top_wgt_ = new QWidget(this);
        //top_wgt_->setStyleSheet(
          //  "background-color:#000080;color:#FFFFFF");
        top_wgt_->setGeometry(QRect(0, 0, 800, 50));
        QHBoxLayout* top_layout = new QHBoxLayout(top_wgt_);
        top_layout->setContentsMargins(0, 0, 0, 0);
        accwin = NULL;
        btn3_ = new QPushButton();
        btn3_->setFixedSize(100, 50);
        btn3_->setIcon(QIcon(
            close_picture.c_str()));
        m_order_row = 0;
       // btn3_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        //btn3_->setGeometry(QRect(10, 40, 25, 19));
        //close_btn_->setStyleSheet("QToolButton{border: none; background: rgb(68, 69, 73); color: rgb(0, 160, 230);}");
        //ToolButtonTextUnderIcon
        btn3_->setText("账号信息");
        top_layout->addWidget(btn3_);

        m_isopen = false;

        GetPreInfo();
        btn_ = new QPushButton();
        btn_->setIcon(QIcon("./res/e3.ico"));
        btn_->setFixedSize(100, 50);
        btn_->setText("交易指示");
        top_layout->addWidget(btn_);
        
        strategy = new QPushButton();
        strategy->setIcon(QIcon("./res/e8.ico"));
        strategy->setFixedSize(100, 50);

        //ToolButtonTextUnderIcon
        strategy->setText("做市策略");
        top_layout->addWidget(strategy);

        btn1_ = new QPushButton();
        btn1_->setIcon(QIcon("./res/e2.ico"));

        btn1_->setFixedSize(100, 50);

        btn1_->setText("挂单列表");
        top_layout->addWidget(btn1_);
        
        btn2_ = new QPushButton();
        btn2_->setIcon(QIcon("./res/e4.ico"));
        btn2_->setFixedSize(100, 50);
        
        //ToolButtonTextUnderIcon
        btn2_->setText("成交列表");
        top_layout->addWidget(btn2_);

        strategylist = new QPushButton();
        strategylist->setIcon(QIcon("./res/e9.ico"));
        strategylist->setFixedSize(100, 50);

        //ToolButtonTextUnderIcon
        strategylist->setText("策略列表");
        top_layout->addWidget(strategylist);


        logbtn = new QPushButton();
        logbtn->setIcon(QIcon("./res/e5.ico"));
        logbtn->setFixedSize(100, 50);

        //ToolButtonTextUnderIcon
        logbtn->setText("日记");
        top_layout->addWidget(logbtn);

       // QLabel ww(QStringLiteral("桌面窗口"));
       // ww.show();
        //setFixedSize(800, 800);
        resize(800, 100);
        connect(btn3_, SIGNAL(clicked()),
            this, SLOT(show_account()));

        connect(btn_, SIGNAL(clicked()),
            this, SLOT(transaction_account()));

        connect(strategy, SIGNAL(clicked()),
            this, SLOT(addstrategy()));

        connect(btn1_, SIGNAL(clicked()),
            this, SLOT(show_order()));

        connect(btn2_, SIGNAL(clicked()),
            this, SLOT(show_fill()));

        connect(logbtn, SIGNAL(clicked()),
            this, SLOT(show_log()));

        connect(strategylist, SIGNAL(clicked()),
            this, SLOT(show_stratery()));

        m_sellorder = 0;
        m_buyorder = 0;
        QCalendarWidget* m_pCale;
        m_pDeskdop = QApplication::desktop();
        move((m_pDeskdop->width() - this->width()) / 2, (m_pDeskdop->height() - this->height()) / 2);
        
        QListWidget* m_pList;

        logwin = new CWndDlg();
        logwin->setAttribute(Qt::WA_QuitOnClose, false);
        logwin->setWindowTitle("日记信息");
        QVBoxLayout* vmainlayout = new QVBoxLayout(logwin);
        logtableWidget = new QTableWidget();
        logtableWidget->setColumnCount(1);
        logtableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        vmainlayout->addWidget(logtableWidget);
        
        logwin->resize(1000, 300);
        logwin-> move((m_pDeskdop->width() - this->width()) / 2, (m_pDeskdop->height() - this->height()) / 2+200);
        logwin->show();

        //新建一个托盘图标对象 //2020-09-22更新，在QSystemTrayIcon()中添加this指针指向mainwindow，以便在关闭窗口时销毁托盘图标
        QSystemTrayIcon* m_trayicon = new QSystemTrayIcon(this);

        //设置托盘图标提示：鼠标移动到上面会提示文字
        m_trayicon->setToolTip(QString("托盘图标程序Demo"));
        //设置图标文件，这里先使用路径来设置，后面介绍使用qt资源文件设置图标文件
        //我将图标放在了项目路径下，并使用了绝对路径
        m_trayicon->setIcon(QIcon("./res/e6.ico"));

        //连接信号与槽：点击托盘图标执行事件
        connect(m_trayicon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActived(QSystemTrayIcon::ActivationReason)));

        //显示托盘图标
        m_trayicon->show();
       
        trader_server::create_instance();
        md_server::create_instance();
        md_server::get_instance()->set_future_platform(this);
        trader_server::get_instance()->set_future_platform(this);
        
        tradesvr1 = new trader_server;
        tradesvr1->set_future_platform(this);
        connect(trader_server::get_instance()->get_trader_spi(),
            SIGNAL(signals_write_log(QString)),
            this, SLOT(slot_write_log(QString)));
        connect(tradesvr1->get_trader_spi(),
            SIGNAL(signals_write_log(QString)),
            this, SLOT(slot_write_log(QString)));
        connect(md_server::get_instance()->get_md_spi(),
            SIGNAL(signals_write_log(QString)),
            this, SLOT(slot_write_log(QString)));
#if 1
        /*connect(trader_server::get_instance()->get_trader_spi(),
            SIGNAL(signals_add_order(TapAPIOrderInfo *)),
            this, SLOT(slot_add_order(TapAPIOrderInfo *)));
        connect(trader_server::get_instance()->get_trader_spi(),
            SIGNAL(signals_add_trans(TapAPIFillInfo *)),
            this, SLOT(slot_add_trans(TapAPIFillInfo *)));*/

        connect(trader_server::get_instance()->get_trader_spi(),
            SIGNAL(signals_state_changed(TapAPIOrderInfo* )),
            this, SLOT(slot_state_changed(TapAPIOrderInfo* )));
        connect(tradesvr1->get_trader_spi(),
            SIGNAL(signals_state_changed(TapAPIOrderInfo*)),
            this, SLOT(slot_state_changed(TapAPIOrderInfo*)));
#endif
        
    }



    future_platform::~future_platform()
    {
        slot_close();
    }

    void future_platform::iconActived(QSystemTrayIcon::ActivationReason reason)
    {
        switch (reason)
        {
            //双击托盘显示窗口
        case QSystemTrayIcon::DoubleClick:
        {
            this->show();
            break;
        }
        default:
            break;
        }
    }
    void future_platform::changeEvent(QEvent* event)
    {
        if (event->type() != QEvent::WindowStateChange)
            return;
        if (this->windowState() == Qt::WindowMinimized)
        {
            this->hide();
        }
    }

    void future_platform::start_server()
    {
        md_server::create_instance();
        trader_server::create_instance();

        init();

        md_server::get_instance()->start_server();        
    }

    void future_platform::slot_close()
    {
        md_server::destory_instance();
        trader_server::destory_instance();
        this->close();
        accwin->close();

    }

    void future_platform::slot_open_q_dialog()
    {
        q_set_dialog_.exec();
    }

    void future_platform::slot_open_t_dialog()
    {
        t_set_dialog_.exec();
    }

    void future_platform::tableItemClicked(int row, int column)
    {
        QTableWidgetItem* item = new QTableWidgetItem;
        item = tableWidget->item(row, 1);
        QMap <QString, QString>::iterator it = mapAccount.find(item->text());
        int ret;
        if (it != mapAccount.end())
        {
            QString str = item->text() + "已登录";
            QMessageBox box;
            box.setText(str);
            //box.setText(item->text());
            box.exec();
            return;
        }

        item = tableWidget->item(row, 0);
#if 0

       

      

        connect(md_server::get_instance()->get_md_spi(),
            SIGNAL(signals_quote_changed(QString)),
            this, SLOT(slot_quote_changed(QString)));

        

        connect(md_server::get_instance()->get_md_spi(),
            SIGNAL(signals_quote_reconnect()),
            this, SLOT(slot_quote_reconnect()));
#endif

        if (item->text() == "md" && 0 == ismd_login)
        {
            
#if 0
            QMessageBox box;
            box.setText(item->text());
            box.exec();
#endif
           
            int ret = md_server::get_instance()->start_server();
            if (0 == ret)
            {
                tableWidget->setItem(row, 2, new QTableWidgetItem("已登录"));
                ismd_login = 1;
               
               // istrade_login = 0;
            }
            else 
            {
                ismd_login = 0;
            }
        }
        else if (item->text() == "trade" && istrade_login < 2)
        {

            
#if 0
           

            connect(trader_server::get_instance()->get_trader_spi(),
                SIGNAL(signals_close_position(QString, QString)),
                this, SLOT(slot_close_position(QString, QString)));

            connect(trader_server::get_instance()->get_trader_spi(),
                SIGNAL(signals_withdraw_order(QString)),
                this, SLOT(slot_withdraw_order(QString)));
#endif  
            char a[20] = {0};
            m_row = row;
            if (0 == row - 1)
            {
                snprintf(a, sizeof(a), "trader_info");
                trader_server::get_instance()->SetFlg(a);
            }
            else
            {
                snprintf(a, sizeof(a), "trader_info%d", row - 1);
                tradesvr1->SetFlg(a);
            }
            
            int ret;
            if (0 == row - 1)
                ret = trader_server::get_instance()->start_server(a);
            else
                ret = tradesvr1->start_server(a);
            if (0 == ret)
            {
                tableWidget->setItem(row, 2, new QTableWidgetItem("已登录"));
                if (0 == row - 1)
                    trader_server::get_instance()->GetTradeData();
                else
                    tradesvr1->GetTradeData();
                string key = a;
                key += "/userid";
                QString userid = common::get_config_value(key).toString();
                m_vec_account.push_back(userid);
                item = tableWidget->item(row, 1);
                mapAccount[item->text()] = a;
                istrade_login++;
            }
            else if (ret == 10003)
            {
                string key = a;
                key += "/telephone";
                QString telephone = common::get_config_value(key).toString();
                //char tel[41] = "16600352051";
                while (ret != 0) {
                    if (0 == row - 1)
                        ret = trader_server::get_instance()->get_trader_spi()->RequestVertificateCode(telephone.toLatin1().data());
                    else
                        ret = tradesvr1->get_trader_spi()->RequestVertificateCode(telephone.toLatin1().data());
                    //ret = m_pAPI->RequestVertificateCode(&m_uiSessionID, tel);
                }

                w2 = new QWidget;
                QLineEdit edit;
                QGridLayout gLayout;
                
                QLabel* product_label = new QLabel("校验码");
                verify_line_edit_ = new QLineEdit();
                gLayout.addWidget(product_label, 0, 1);
                gLayout.addWidget(verify_line_edit_, 0, 2);
               
                QPushButton* buy = new QPushButton("确定");
                connect(buy, SIGNAL(clicked()), this, SLOT(verify_code()));
                QPushButton *cancel  = new QPushButton("取消");
                connect(cancel, SIGNAL(clicked()), this, SLOT(Cancel()));
                gLayout.addWidget(buy, 1, 1);
                gLayout.addWidget(cancel, 1, 2);
                w2->setLayout(&gLayout);
                w2->setWindowTitle("确认校验码");
                w2->setAttribute(Qt::WA_QuitOnClose, false);
                w2->show();
            }
        }
        
            /* do some stuff with item */
    }

    void CWndDlg::closeEvent(QCloseEvent* event)
    {
        QString str = "关闭";
        QMessageBox box;
        box.setText(str);
    }

    void future_platform::show_account()
    {
        //APP_LOG(applog::LOG_CRITICAL) << "test";
        QString str1 = "关闭";
        QMessageBox box;
        box.setText(str1);

        if (NULL != accwin && true ==  accwin->isVisible())
            return;
        accwin = new CWndDlg();
        accwin->setAttribute(Qt::WA_QuitOnClose, false);

        QVBoxLayout* vmainlayout = new QVBoxLayout(accwin);
        QWidget* widget = new QWidget();
        vmainlayout->addWidget(widget);

        QHBoxLayout* mainlayout = new QHBoxLayout();
        tableWidget = new QTableWidget();
        tableWidget->setColumnCount(3);
        QStringList header;
        
        string key;
        key = "md_info/userid";
        QString userid = common::get_config_value(key).toString();
        header << QStringLiteral("类型") << QStringLiteral("账号");
        tableWidget->setHorizontalHeaderLabels(QStringList() << tr("类型") << tr("账号") << tr("状态"));
        tableWidget->setRowCount(1);
          tableWidget->setItem(0, 0, new QTableWidgetItem("md"));
        tableWidget->setItem(0, 1, new QTableWidgetItem(userid));
        if (0 == ismd_login)
        tableWidget->setItem(0, 2, new QTableWidgetItem("未登录"));
        else if (1 == ismd_login)
            tableWidget->setItem(0, 2, new QTableWidgetItem("已登录"));
        tableWidget->setRowCount(2);

        key = "trader_info/userid";
        QString str = "未登录";
        userid = common::get_config_value(key).toString();
        tableWidget->setItem(1, 0, new QTableWidgetItem("trade"));
        tableWidget->setItem(1, 1, new QTableWidgetItem(userid));
        tableWidget->setItem(1, 2, new QTableWidgetItem("未登录"));
       
        for (int i = 0; i < m_vec_account.size(); i++)
        {
            if (userid == m_vec_account[i])
            {
                str = "已登录";
                break;
            }
        }

        tableWidget->setItem(1, 2, new QTableWidgetItem(str));
        
        str = "未登录";
        tableWidget->setRowCount(3);
        key = "trader_info1/userid";
        userid = common::get_config_value(key).toString();
        tableWidget->setItem(2, 0, new QTableWidgetItem("trade"));
        tableWidget->setItem(2, 1, new QTableWidgetItem(userid));
        
        for (int i = 0; i < m_vec_account.size(); i++)
        {
            if (userid == m_vec_account[i])
            {
                str = "已登录";
                break;
            }
        }
        
        tableWidget->setItem(2, 2, new QTableWidgetItem(str));

        QHeaderView* headerview = tableWidget->horizontalHeader();
        tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

        //headerview->setSectionResizeMode(QHeaderView::Stretch);
        tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        
        logout_widget_menu = new QMenu(tableWidget);
        logoutaction = new QAction("登出", this);
        connect(logoutaction, SIGNAL(triggered()), this, SLOT(slotLogout()));
        logout_widget_menu->addAction(logoutaction);

        connect(tableWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(logoutMenuRequested(QPoint)));

       connect(tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(tableItemClicked(int, int)));
        mainlayout->addWidget(tableWidget);

       // mainlayout->addWidget(tableWidget);
        mainlayout->setMargin(10);
        mainlayout->setSpacing(5);
        widget->setLayout(mainlayout);
        accwin->setWindowTitle("账号信息");
        accwin->show();
       
      
    }

    void GetTime(char* time, string& strFullTime)
    {
        char year[4] = { 0 };
        char mon[4] = { 0 };
        char day[4] = { 0 };
        char hour[4] = { 0 };
        char min[4] = { 0 };
        char sec[4] = { 0 };
        char fulltime[128] = { 0 };
        int add = 2;
        int move = 0;
        strncpy(year, time + move, add);
        move += 2;
        strncpy(mon, time + move, add);
        move += 2;
        strncpy(day, time + move, add);
        move += 2;
        strncpy(hour, time + move, add);
        move += 2;
        strncpy(min, time + move, add);
        move += 2;
        strncpy(sec, time + move, add);
        snprintf(fulltime, sizeof(fulltime), "%d-%0s-%s %s:%s:%s",
            atoi(year)+2000, mon, day,hour, min, sec);
        strFullTime = fulltime;
    }

    void future_platform::tableContextMenuRequested(const QPoint& pos)
    {
        QTableWidgetItem* item = new QTableWidgetItem;
        item = ordertableWidget->item(ordertableWidget->currentRow(), 13);
        if (item->text() == "PARTFINISHED" || item->text() == "QUEUED")
             action->setEnabled(true);
        else
            action->setEnabled(false);
        table_widget_menu->exec(QCursor::pos());
    }

    void future_platform::logoutMenuRequested(const QPoint& pos)
    {
#if 1
        QTableWidgetItem* item = new QTableWidgetItem;
        item = tableWidget->item(tableWidget->currentRow(), 2);
        if (item->text() == "未登录")
            logoutaction->setEnabled(false);
        else
            logoutaction->setEnabled(true);
#endif
        logout_widget_menu->exec(QCursor::pos());
    }

    void future_platform::slotActionCancel()
    {
       // QMessageBox::information(NULL, "Title", "Content",
         //   QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        QTableWidgetItem* item = new QTableWidgetItem;
        item = ordertableWidget->item(ordertableWidget->currentRow(), 0);
        QMap <QString, QString>::iterator it = mapAccount.find(item->text());
        int ret;
        if (it != mapAccount.end())
        {
            item = ordertableWidget->item(ordertableWidget->currentRow(), 10);
            if (it.value() == "trader_info")
            {
                ret = trader_server::get_instance()->get_trader_spi()->order_withdraw(item->text().toLatin1().data());
            }
            else
            {
                ret = tradesvr1->get_trader_spi()->order_withdraw(item->text().toLatin1().data());
            }

            if (ret)
            {
                QMessageBox::information(newwin, QString().fromLocal8Bit("取消挂单"), QString().fromLocal8Bit("取消挂单失败"));

            }
            else
            {
                QMessageBox::information(newwin, QString().fromLocal8Bit("取消挂单"), QString().fromLocal8Bit("取消挂单成功"));
            }
        }
    }


    void future_platform::slotLogout()
    {
        // QMessageBox::information(NULL, "Title", "Content",
          //   QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        QTableWidgetItem* item = new QTableWidgetItem;
        item = tableWidget->item(tableWidget->currentRow(), 1);
        QMap <QString, QString>::iterator it = mapAccount.find(item->text());
        int ret;
        if (it != mapAccount.end())
        {
            //item = tableWidget->item(tableWidget->currentRow(), 10);
            if (it.value() == "trader_info")
            {
                if (trader_server::get_instance()->ctptrader_api_inst) {
                    //ctptrader_api_inst->SetAPINotify(nullptr);
                    trader_server::get_instance()->ctptrader_api_inst->Disconnect();
                    FreeITapTradeAPI(trader_server::get_instance()->ctptrader_api_inst);
                    QMap<QString, TapAPIOrderInfo> map_order2;
                    map_order2.swap(trader_server::get_instance()->get_trader_spi()->m_map_order2);
                    QVector <TapAPIFillInfo> map_fill;
                    map_fill.swap(trader_server::get_instance()->get_trader_spi()->m_map_fill);
                }
            }
            else
            {
                if (tradesvr1->ctptrader_api_inst) {
                    //ctptrader_api_inst->SetAPINotify(nullptr);
                    tradesvr1->ctptrader_api_inst->Disconnect();
                    FreeITapTradeAPI(tradesvr1->ctptrader_api_inst);
                    QMap<QString, TapAPIOrderInfo> map_order2;
                    map_order2.swap(tradesvr1->get_trader_spi()->m_map_order2);
                    QVector <TapAPIFillInfo> map_fill;
                    map_fill.swap(tradesvr1->get_trader_spi()->m_map_fill);
                    
                }
            }

            mapAccount.erase(it);
            istrade_login--;
            QVector <QString>::iterator iter = m_vec_account.begin();
            while (iter != m_vec_account.end())
            {
                if (*iter == item->text())
                {
                    m_vec_account.erase(iter);
                    break;
                }

                iter++;
            }
          
            tableWidget->setItem(tableWidget->currentRow(), 2, new QTableWidgetItem("未登录"));
        }
    }

    void future_platform::show_order()
    {
        newwin = new QWidget();
        QVBoxLayout* vmainlayout = new QVBoxLayout(newwin);
        QWidget* widget = new QWidget();
        vmainlayout->addWidget(widget);

        QHBoxLayout* mainlayout = new QHBoxLayout();
        ordertableWidget = new QTableWidget();
        ordertableWidget->setColumnCount(15);
        QStringList header;
       // ordertableWidget->resizeColumnsToContents();
        //ordertableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        string key;
        key = "md_info/userid";
        QString userid = common::get_config_value(key).toString();

        ordertableWidget->setHorizontalHeaderLabels(QStringList() << tr("AccountNo") << tr("ExchangeNo")
            << tr("CommodityType") << tr("CommodityNo") << tr("Contract") << tr("StrikePrice") << tr("CallOrPutFlag") << tr("OrderSide")
            << tr("OrderPrice") << tr("OrderQty") << tr("OrderNo") << tr("OrderInsertTime") << tr("OrderUpdateTime") << tr("OrderState") << tr("OrderMatchQty"));
        
        QHeaderView* headerview = ordertableWidget->horizontalHeader();
       

       // connect(tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(tableItemClicked(int, int)));
        QMap<QString, TapAPIOrderInfo>* maptmp;
        QMap<QString, TapAPIOrderInfo>::iterator it;
        
        //last--;
        int line = 0;
        int col = 0;
        for (int k = 0; k < 2; k++)
        {
            if (0 == k)
            {
                QMap<QString, TapAPIOrderInfo>& map = trader_server::get_instance()->GetOrderMap();
                maptmp = &map;
            }
            else
            {
                QMap<QString, TapAPIOrderInfo>& map = tradesvr1->GetOrderMap();
                maptmp = &map;
            }

            QMap<QString, TapAPIOrderInfo>::iterator last = maptmp->end();
            for (it = maptmp->begin(); it != last; it++)
            {
                TapAPIOrderInfo& info = it.value();
                string Contract, Contract2, OrderState;
                Contract = info.ContractNo;
                Contract2 = info.ContractNo2;
                if (Contract2.empty()) {
                }
                else {
                    Contract = Contract + "/" + Contract2;
                }
                switch (info.OrderState) {

                case '0':
                    OrderState = "SUMBITTED";
                    break;
                case '1':
                    OrderState = "ACCEPTED";
                    break;
                case '4':
                    OrderState = "QUEUED";
                    break;
                case '5':
                    OrderState = "PARTFINISHED";
                    break;
                case '6':
                    OrderState = "FINISHED";
                    break;
                case '7':
                    OrderState = "CANCELLING";
                    break;
                case '8':
                    OrderState = "MODIFYING";
                    break;
                case '9':
                    OrderState = "CANCELLED";
                    break;
                case 'A':
                    OrderState = "LEFTDELETED";
                    break;
                case 'B':
                    OrderState = "FAILED";
                    break;
                default:
                    OrderState = info.OrderState;
                }

                ordertableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

                ordertableWidget->setRowCount(line + 1);
                col = 0;
                ordertableWidget->setItem(line, col++, new QTableWidgetItem(info.AccountNo));
                ordertableWidget->setItem(line, col++, new QTableWidgetItem(info.ExchangeNo));
                QString str;
                str.sprintf("%c", info.CommodityType);
                ordertableWidget->setItem(line, col++, new QTableWidgetItem(str));
                ordertableWidget->setItem(line, col++, new QTableWidgetItem(info.CommodityNo));

                ordertableWidget->setItem(line, col++, new QTableWidgetItem(Contract.c_str()));

                ordertableWidget->setItem(line, col++, new QTableWidgetItem(info.StrikePrice));
                str.sprintf("%c", info.CallOrPutFlag);

                ordertableWidget->setItem(line, col++, new QTableWidgetItem(str));
                str.sprintf("%c", info.OrderSide);
                ordertableWidget->setItem(line, col++, new QTableWidgetItem(str));
                str.sprintf("%f", info.OrderPrice);
                ordertableWidget->setItem(line, col++, new QTableWidgetItem(str));
                str.sprintf("%d", info.OrderQty);
                ordertableWidget->setItem(line, col++, new QTableWidgetItem(str));
               // ordertableWidget->horizontalHeader()->setSectionResizeMode(10, QHeaderView::Stretch);
                
                
                ordertableWidget->setItem(line, col++, new QTableWidgetItem(info.OrderNo));
                ordertableWidget->resizeColumnToContents(10);
                char* pos = strchr(info.OrderInsertTime, '.');

                string strFullTime;
                if (pos != NULL)
                {

                    char time[20] = { 0 };
                    strncpy(time, info.OrderInsertTime, pos - info.OrderInsertTime);
                    GetTime(time, strFullTime);
                }
                //tableWidget->setItem(line, col++, new QTableWidgetItem(info.OrderInsertTime));
               
                ordertableWidget->setItem(line, col++, new QTableWidgetItem(strFullTime.c_str()));
                ordertableWidget->resizeColumnToContents(11);
                pos = strchr(info.OrderUpdateTime, '.');


                if (pos != NULL)
                {
                    char time[20] = { 0 };
                    strncpy(time, info.OrderUpdateTime, pos - info.OrderUpdateTime);
                    GetTime(time, strFullTime);
                }
                
                ordertableWidget->setItem(line, col++, new QTableWidgetItem(strFullTime.c_str()));
                ordertableWidget->resizeColumnToContents(12);
                ordertableWidget->setItem(line, col++, new QTableWidgetItem(OrderState.c_str()));
                str.sprintf("%d", info.OrderMatchQty);
                ordertableWidget->setItem(line, col++, new QTableWidgetItem(str));
                line++;
            }
        }

        ordertableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        ordertableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table_widget_menu = new QMenu(ordertableWidget);
        action = new QAction("取消挂单", this);
        connect(action, SIGNAL(triggered()), this, SLOT(slotActionCancel()));
        table_widget_menu->addAction(action);

        connect(ordertableWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tableContextMenuRequested(QPoint)));
        //headerview->setSectionResizeMode(QHeaderView::Stretch);
        mainlayout->addWidget(ordertableWidget);
        ordertableWidget->scrollToBottom();
        // mainlayout->addWidget(tableWidget);
        mainlayout->setMargin(10);
        mainlayout->setSpacing(5);
        widget->setLayout(mainlayout);
        newwin->setWindowTitle("挂单信息");
        //newwin->setFixedSize(1200, 800);
        newwin->resize(1500, 800);
        //connect(ordertableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(tableItemClicked(int, int)));
        newwin->setAttribute(Qt::WA_QuitOnClose, false);
        QVector<TapAPIOrderInfo>& vecOrder1 = tradesvr1->get_trader_spi()->m_add_order;
        QVector<TapAPIOrderInfo> vecSwap;

        m_order_row = trader_server::get_instance()->GetOrderMap().size() + tradesvr1->GetOrderMap().size();
            QVector<TapAPIOrderInfo>& vecOrder2 = trader_server::get_instance()->get_trader_spi()->m_add_order;
        QVector<TapAPIOrderInfo> vecSwap1;
        vecSwap1.swap(vecOrder2);
        newwin->show();
    }

    void future_platform::buy_order()
    {
        
        //QMessageBox message(QMessageBox::NoIcon, "Title", num_line_edit_->text());
       
        char account[64] = {0};
        QByteArray ba = aselect_check_->currentText().toLatin1(); // must
        strcpy(account,ba.data());
        

        char product[128] = { 0 };
        ba = product_line_edit_->text().toLatin1(); // must
        strcpy(product, ba.data());
        

        double price;
        ba = price_line_edit_->text().toLatin1(); // must
        price = atof(ba.data());
        
        QMap <QString, QString>::iterator it;
        int num;
        ba = num_line_edit_->text().toLatin1(); // must
        num = atoi(ba.data());
        it = mapAccount.find(account);
        int ret;
        if (it != mapAccount.end())
        {
            if (it.value() == "trader_info")
            {
                ret = trader_server::get_instance()->order_create(account, product, price, num, TAPI_SIDE_BUY);
            }
            else
            {
                ret = tradesvr1->order_create(account, product, price, num, TAPI_SIDE_BUY);
            }
        }
       // trader_server::get_instance()->order_create(aselect_check_->currentText().toStdString(), product_line_edit_->text().toStdString(), atof(price_line_edit_->text().toStdString().c_str()), atoi(num_line_edit_->text().toStdString().c_str()), TAPI_SIDE_BUY);
        
        if (ret)
        {
            QMessageBox::information(w, QString().fromLocal8Bit("买入"), QString().fromLocal8Bit("买入挂单失败"));

        }
        else
        {
            QMessageBox::information(w, QString().fromLocal8Bit("买入"), QString().fromLocal8Bit("买入挂单成功"));
        }
    }

    void future_platform::show_log()
    {
        QString str1 = "关闭";
        QMessageBox box;
        box.setText(str1);

        if (NULL != logwin && true == logwin->isVisible())
            return;

        logwin = new CWndDlg();
        logwin->setAttribute(Qt::WA_QuitOnClose, false);
        logwin->setWindowTitle("日记信息");

        QVBoxLayout* vmainlayout = new QVBoxLayout(logwin);
        //logtableWidget = new QTableWidget();
      //  logtableWidget->setColumnCount(1);
       // logtableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        vmainlayout->addWidget(logtableWidget);

     //   log_text_edit_ = new QTextBrowser(logwin);
      //  log_text_edit_->resize(300, 300);
        //->addWidget(log_text_edit_);
        logwin->resize(1000, 300);
        logwin->move((m_pDeskdop->width() - this->width()) / 2, (m_pDeskdop->height() - this->height()) / 2 + 200);
        //logwin->resize(300, 300);
        //logwin->move((m_pDeskdop->width() - this->width()) / 2, (m_pDeskdop->height() - this->height()) / 2 + 200);
        logwin->show();
    }

    void future_platform::verify_code()
    {

        //QMessageBox message(QMessageBox::NoIcon, "Title", num_line_edit_->text());
        char a[20] = { 0 };
        
        if (0 == m_row - 1)
        {
            snprintf(a, sizeof(a), "trader_info");
         
        }
        else
        {
            snprintf(a, sizeof(a), "trader_info%d", m_row - 1);
          
        }
       
        QByteArray ba;
        ba = verify_line_edit_->text().toLatin1(); // must
        
        int ret;
        if (0 == m_row - 1)
            ret = trader_server::get_instance()->get_trader_spi()->SetVertificateCode(ba.data());
        else
            ret = tradesvr1->get_trader_spi()->SetVertificateCode(ba.data());
        if (0 == ret)
        {
            tableWidget->setItem(m_row, 2, new QTableWidgetItem("已登录"));
            if (0 == m_row - 1)
                trader_server::get_instance()->GetTradeData();
            else
                tradesvr1->GetTradeData();
            string key = a;
            key += "/userid";
            QString userid = common::get_config_value(key).toString();
            m_vec_account.push_back(userid);
            QTableWidgetItem* item = tableWidget->item(m_row, 1);
            mapAccount[item->text()] = a;
            istrade_login++;
            w2->hide();
        }

       
    }

    void future_platform::Cancel()
    {
        w2->hide();
    }

    void future_platform::sell_order()
    {
        char account[64] = { 0 };
        QByteArray ba = aselect_check_->currentText().toLatin1(); // must
        strcpy(account, ba.data());


        char product[128] = { 0 };
        ba = product_line_edit_->text().toLatin1(); // must
        strcpy(product, ba.data());

        double price;
        ba = price_line_edit_->text().toLatin1(); // must
        price = atof(ba.data());


        int num;
        ba = num_line_edit_->text().toLatin1(); // must
        num = atoi(ba.data());

        QMap <QString, QString>::iterator it;
       
        it = mapAccount.find(account);
        int ret;
        if (it != mapAccount.end())
        {
            if (it.value() == "trader_info")
            {
                ret = trader_server::get_instance()->order_create(account, product, price, num, TAPI_SIDE_SELL);
            }
            else
            {
                ret = tradesvr1->order_create(account, product, price, num, TAPI_SIDE_SELL);
            }
        }

        // trader_server::get_instance()->order_create(aselect_check_->currentText().toStdString(), product_line_edit_->text().toStdString(), atof(price_line_edit_->text().toStdString().c_str()), atoi(num_line_edit_->text().toStdString().c_str()), TAPI_SIDE_BUY);
       // int ret = trader_server::get_instance()->order_create(account, product, price, num, TAPI_SIDE_SELL);
        if (ret)
        {
            QMessageBox::information(w, QString().fromLocal8Bit("卖出"), QString().fromLocal8Bit("卖出挂单失败"));
        }
        else
        {
            QMessageBox::information(w, QString().fromLocal8Bit("卖出"), QString().fromLocal8Bit("卖出挂单成功"));
        }
    }

    void future_platform::sell_create_order()
    {

        //QMessageBox message(QMessageBox::NoIcon, "Title", num_line_edit_->text());

        char* account;
        QByteArray ba = sell_select_check_->currentText().toLatin1(); // must
        account = ba.data();

        char* product;
        ba = sell_product_line_edit_->text().toLatin1(); // must
        product = ba.data();


        double buyer_price;
        ba = buy_price_line_edit_->text().toLatin1(); // must
        buyer_price = atof(ba.data());

        int buyer_num;
        ba = buy_num_line_edit_->text().toLatin1(); // must
        buyer_num = atof(ba.data());

        double sell_price;
        ba = sell_price_line_edit_->text().toLatin1(); // must
        sell_price = atof(ba.data());

        int sell_num;
        ba = sell_num_line_edit_->text().toLatin1(); // must
        sell_num = atof(ba.data());
        QMap <QString, QString>::iterator it = mapAccount.find(sell_select_check_->currentText().toLatin1().data());
        
        int ret;
        if (it != mapAccount.end())
        {
            if (it.value() == "trader_info")
            {
                //ret = trader_server::get_instance()->order_create(account, product, price, num, TAPI_SIDE_BUY);
                ret = trader_server::get_instance()->sell_order_create(
                    sell_select_check_->currentText().toLatin1().data(),
                    sell_product_line_edit_->text().toLatin1().data(),
                    buyer_price,
                    buyer_num,
                    sell_price,
                    sell_num,
                    TAPI_SIDE_BUY);
            }
            else
            {
                //ret = tradesvr1->order_create(account, product, price, num, TAPI_SIDE_BUY);
                ret = tradesvr1->sell_order_create(
                    sell_select_check_->currentText().toLatin1().data(),
                    sell_product_line_edit_->text().toLatin1().data(),
                    buyer_price,
                    buyer_num,
                    sell_price,
                    sell_num,
                    TAPI_SIDE_BUY);
            }
        }
        // trader_server::get_instance()->order_create(aselect_check_->currentText().toStdString(), product_line_edit_->text().toStdString(), atof(price_line_edit_->text().toStdString().c_str()), atoi(num_line_edit_->text().toStdString().c_str()), TAPI_SIDE_BUY);
        
        if (ret)
        {
            QMessageBox::information(w, QString().fromLocal8Bit("买入"), QString().fromLocal8Bit("买入挂单失败"));
        }
        else
        {
            QMessageBox::information(w, QString().fromLocal8Bit("买入"), QString().fromLocal8Bit("买入挂单成功"));
            if (it.value() == "trader_info")
            {
                //ret = trader_server::get_instance()->order_create(account, product, price, num, TAPI_SIDE_BUY);
                Sleep(3000);
                ret = trader_server::get_instance()->get_trader_spi()->sell_order_cancel();
            }
            else
            {
                Sleep(3000);
                //ret = tradesvr1->order_create(account, product, price, num, TAPI_SIDE_BUY);
                ret = tradesvr1->get_trader_spi()->sell_order_cancel();
            }
            
        }
    }

    void future_platform::show_fill()
    {
        fillwin = new QWidget();
       
        QVBoxLayout* vmainlayout = new QVBoxLayout(fillwin);
        QWidget* widget = new QWidget();
        vmainlayout->addWidget(widget);

        QHBoxLayout* mainlayout = new QHBoxLayout();
        filltableWidget = new QTableWidget();
        filltableWidget->setColumnCount(12);
        QStringList header;
        //header << QStringLiteral("时间") << QStringLiteral("经度") << QStringLiteral("纬度") << QStringLiteral("高程") << QStringLiteral("四元数1")
          //  << QStringLiteral("四元数2") << QStringLiteral("四元数3") << QStringLiteral("四元数4");
        string key;
        key = "md_info/userid";
        QString userid = common::get_config_value(key).toString();


        //header << QStringLiteral("产品") << QStringLiteral("账号");
        filltableWidget->setHorizontalHeaderLabels(QStringList() << tr("AccountNo") << tr("ExchangeNo")
            << tr("CommodityType") << tr("CommodityNo") << tr("ContractNo") << tr("StrikePrice") << tr("CallOrPutFlag") << tr("MatchSide")
            << tr("OrderNo") << tr("MatchDateTime") << tr("MatchPrice") << tr("MatchQty"));

        QHeaderView* headerview = filltableWidget->horizontalHeader();


        // connect(tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(tableItemClicked(int, int)));
        QVector <TapAPIFillInfo>& map = trader_server::get_instance()->GetFillMap();
       
        //last--;
        int line = 0;
        int col = 0;
        TapAPIFillInfo *info;
        QVector <TapAPIFillInfo> *maptmp;
        for (int k = 0; k < 2; k++)
        {
            if (0 == k)
            {
                QVector <TapAPIFillInfo>& map = trader_server::get_instance()->GetFillMap();
                maptmp = &map;
            }
            else
            {
                QVector <TapAPIFillInfo>& map = tradesvr1->GetFillMap();
                maptmp = &map;
            }

            for (int i = 0; i < maptmp->size(); i++)
            {
                TapAPIFillInfo& info = (*maptmp)[i];

                filltableWidget->setRowCount(line + 1);
                col = 0;
                filltableWidget->setItem(line, col++, new QTableWidgetItem(info.AccountNo));
                filltableWidget->setItem(line, col++, new QTableWidgetItem(info.ExchangeNo));
                QString str;
                str.sprintf("%c", info.CommodityType);
                filltableWidget->setItem(line, col++, new QTableWidgetItem(str));
                filltableWidget->setItem(line, col++, new QTableWidgetItem(info.CommodityNo));

                filltableWidget->setItem(line, col++, new QTableWidgetItem(info.ContractNo));

                filltableWidget->setItem(line, col++, new QTableWidgetItem(info.StrikePrice));
                str.sprintf("%c", info.CallOrPutFlag);
                filltableWidget->setItem(line, col++, new QTableWidgetItem(str));
                str.sprintf("%c", info.MatchSide);
                filltableWidget->setItem(line, col++, new QTableWidgetItem(str));
                
                filltableWidget->setItem(line, col++, new QTableWidgetItem(info.OrderNo));
                filltableWidget->resizeColumnToContents(8);
                
                filltableWidget->setItem(line, col++, new QTableWidgetItem(info.MatchDateTime));
                filltableWidget->resizeColumnToContents(9);

                str.sprintf("%f", info.MatchPrice);
                filltableWidget->setItem(line, col++, new QTableWidgetItem(str));

                str.sprintf("%d", info.MatchQty);
                filltableWidget->setItem(line, col++, new QTableWidgetItem(str));

                line++;
            }
        }
       
        filltableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        filltableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

        //headerview->setSectionResizeMode(QHeaderView::Stretch);
        mainlayout->addWidget(filltableWidget);
        filltableWidget->scrollToBottom();
        // mainlayout->addWidget(tableWidget);
        mainlayout->setMargin(10);
        mainlayout->setSpacing(5);
        widget->setLayout(mainlayout);
        fillwin->setWindowTitle("成交信息");
        //newwin->setFixedSize(1200, 800);
        fillwin->resize(1200, 800);
        fillwin->setAttribute(Qt::WA_QuitOnClose, false);
        QVector<TapAPIFillInfo>& vecOrder = tradesvr1->get_trader_spi()->m_add_fill;
        QVector<TapAPIFillInfo> vecSwap;
        vecSwap.swap(vecOrder);

        QVector<TapAPIFillInfo>& vecOrder1 = tradesvr1->get_trader_spi()->m_add_fill;
        QVector<TapAPIFillInfo> vecSwap1;
        vecSwap1.swap(vecOrder1);
        m_fill_row = trader_server::get_instance()->GetFillMap().size() + tradesvr1->GetFillMap().size();
        fillwin->show();
    }

    int future_platform::check_select(const QString& text)
    {
        //
        if (text == "庄家订单")
        {   
                //firstcreate = 1;
                w1 = new QWidget;
                sell_gLayout = new QGridLayout;
                QLabel* aselect_label = new QLabel("账号");
                sell_select_check_ = new QComboBox();

                sell_gLayout->addWidget(aselect_label, 0, 1);
                sell_gLayout->addWidget(sell_select_check_, 0, 2);
                QStringList strList;
                for (int i = 0; i < m_vec_account.size(); i++)
                {
                    strList << m_vec_account[i];
                }

                sell_select_check_->addItems(strList);

                QLabel* product_label = new QLabel("产品");
                sell_product_line_edit_ = new QLineEdit();
                sell_gLayout->addWidget(product_label, 1, 1);
                sell_gLayout->addWidget(sell_product_line_edit_, 1, 2);


                QLabel* price_label = new QLabel("买入价");
                buy_price_line_edit_ = new QLineEdit();
                sell_gLayout->addWidget(price_label, 2, 1);
                sell_gLayout->addWidget(buy_price_line_edit_, 2, 2);

                price_label = new QLabel("买入数");
                buy_num_line_edit_ = new QLineEdit();
                sell_gLayout->addWidget(price_label, 2, 3);
                sell_gLayout->addWidget(buy_num_line_edit_, 2, 4);


                QLabel* num_label = new QLabel("沽出价");
                sell_price_line_edit_ = new QLineEdit();
                sell_gLayout->addWidget(num_label, 3, 1);
                sell_gLayout->addWidget(sell_price_line_edit_, 3, 2);

                num_label = new QLabel("沽出数");
                sell_num_line_edit_ = new QLineEdit();
                sell_gLayout->addWidget(num_label, 3, 3);
                sell_gLayout->addWidget(sell_num_line_edit_, 3, 4);

                QLabel* select_ = new QLabel("选项");
                sell_type_check1_ = new QComboBox();
                sell_gLayout->addWidget(select_, 4, 1);
                sell_gLayout->addWidget(sell_type_check1_, 4, 2);
                QStringList strList1;
                strList1 << "庄家订单" << "普通交易";
                sell_type_check1_->addItems(strList1);

                QPushButton* buy = new QPushButton("发出");
                sell_gLayout->addWidget(buy, 5, 1);

                connect(buy, SIGNAL(clicked()), this, SLOT(sell_create_order()));
                //connect(sell, SIGNAL(clicked()), this, SLOT(sell_order()));



               w1->setLayout(sell_gLayout);
                w1->setWindowTitle("交易指示");
                connect(sell_type_check1_, SIGNAL(currentIndexChanged(const QString)), this, SLOT(check_select(const QString)));
                w1->setAttribute(Qt::WA_QuitOnClose, false);
                w->setAttribute(Qt::WA_QuitOnClose, false);
                w1->show();
                w->hide();
               // w->close();
              //  delete w;
        }
        else if (text == "普通交易")
        {
            w = new QWidget;
            QLineEdit edit;
            QGridLayout gLayout;
            QLabel* aselect_label = new QLabel("账号");
            aselect_check_ = new QComboBox();
            gLayout.addWidget(aselect_label, 0, 1);
            gLayout.addWidget(aselect_check_, 0, 2);
            QStringList strList;
            for (int i = 0; i < m_vec_account.size(); i++)
            {
                strList << m_vec_account[i];
            }

            aselect_check_->addItems(strList);
            QLabel* product_label = new QLabel("产品");
            product_line_edit_ = new QLineEdit();
            gLayout.addWidget(product_label, 1, 1);
            gLayout.addWidget(product_line_edit_, 1, 2);


            QLabel* price_label = new QLabel("价格");
            price_line_edit_ = new QLineEdit();
            gLayout.addWidget(price_label, 2, 1);
            gLayout.addWidget(price_line_edit_, 2, 2);

            QLabel* num_label = new QLabel("数量");
            num_line_edit_ = new QLineEdit();
            gLayout.addWidget(num_label, 3, 1);
            gLayout.addWidget(num_line_edit_, 3, 2);

            QLabel* select_label = new QLabel("选项");
            QComboBox* select_check_ = new QComboBox();
            gLayout.addWidget(select_label, 4, 1);
            gLayout.addWidget(select_check_, 4, 2);
            QStringList strList1;
            strList1 << "普通交易" << "庄家订单";
            select_check_->addItems(strList1);

            QPushButton* buy = new QPushButton("买入");
            QPushButton* sell = new QPushButton("卖出");


            gLayout.addWidget(buy, 5, 1);
            gLayout.addWidget(sell, 5, 2);

            connect(buy, SIGNAL(clicked()), this, SLOT(buy_order()));
            connect(sell, SIGNAL(clicked()), this, SLOT(sell_order()));

            w->setLayout(&gLayout);
            w->setWindowTitle("交易指示");
            connect(select_check_, SIGNAL(currentIndexChanged(const QString)), this, SLOT(check_select(const QString)));
            

            w1->setAttribute(Qt::WA_QuitOnClose, false);
            w->setAttribute(Qt::WA_QuitOnClose, false);
            w->show();
            w1->hide();
           // delete w1;
        }
        //
        return 0;

    }

    string future_platform::gettrategystr()
    {
        string strData;

        QByteArray ba;
        ba = product_market_edit_->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";
        
        char tmp[4];
        snprintf(tmp, sizeof(tmp), "%d", market_check_->currentIndex());
        strData += tmp;
        strData += ",";
        QMessageBox box;
        box.setText(tmp);
        //box.exec();
        ba = buy_market_num_->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        ba = sell_market_num_->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        ba = Targetcontract1->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        ba = Targetcontract2->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        ba = adjustment->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        ba = bidspread->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";


        ba = askspread->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        ba = bidspread1->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        ba = askspread1->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        ba = threshold->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        ba = threshhold1->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        char state[4];
        snprintf(state, sizeof(state), "%d", AutoHedge->checkState());
        strData += state;
        strData += ",";

        ba = hedginginstrument1->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        ba = slippage1->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        ba = radio1->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        ba = hedginginstrument2->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        ba = slippage2->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        ba = radio3->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        ba = stoptime->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        ba = Strategyname->text().toLatin1(); // must
        strData += ba.data();
        strData += ",";

        return strData;
    }


    double DecimlPart(double x)
    {
        return x - (int)x;
    }

    int IntPart(double x)
    {
        return (int)x;
    }

#define ZERO 0
#define ONE 1

    double Power(double radix, int index)
    {
        double result = 1.0;

        if (index == ZERO && radix != ZERO)
        {
            result = ONE;
        }
        else if (radix == ZERO)
        {
            result = ZERO;
        }
        else if (index < ZERO)
        {
            for (; index < ZERO; index++)
            {
                result = result * radix;
            }
            result = 1.0 / result;
        }
        else if (index > ZERO)
        {
            for (; index > ZERO; index--)
            {
                result = result * radix;
            }
        }
        return result;
    }
    
    double Rurd(double x, int exaval)
    {
        double num = x;//保存原数

        x = DecimlPart(x);//取小数部分

        x = x * Power((double)10, exaval);//乘以10的精确值次方

        x = DecimlPart(x);//取小数部分

        x = x * 10;//乘10

        if ((int)x > 4)//判断
        {
            x = num * Power((double)10, exaval);
            x = (int)x + 1;
            x = x / Power((double)10, exaval);
        }
        else
        {
            x = num * Power((double)10, exaval);
            x = (int)x;
            x = x / Power((double)10, exaval);
        }

        return x;
    }

    
    void future_platform::addbuytrategy()
    {
        QMessageBox box;
        
        if (m_vec_account.size() <= 0)
        {
            box.setText("请至少登录一个交易账号");
            box.exec();
            return;
        }

        QDir* photo = new QDir;
        QString conf_path = QCoreApplication::applicationDirPath() + "/stratery/";
        bool exist = photo->exists(conf_path);
        if (exist)
        {
            
        }
        else
        {
            //创建photo文件夹
            photo->mkdir(conf_path);
        }
       

        QString fileName = Strategyname->text();
        conf_path = conf_path + fileName;
        QFile file(conf_path);
        if (file.exists())
        {
            QFile::remove(conf_path);
        }

        char state[4];
        snprintf(state, sizeof(state), "%d", AutoHedge->checkState());
        box.setText(state);
       // box.exec();

        QByteArray ba;
        string str = gettrategystr();
        
        str +=  "0";
        str += ",";
        if (2 == AutoHedge->checkState())
        {
            ba = account_check_->currentText().toLatin1(); // must
            str += ba.data();
        }
        
        str += ",";
        if (2 == AutoHedge->checkState())
        {
            ba = Hedgingaccount1->currentText().toLatin1(); // must
            str += ba.data();
        }
        
        str += ",";

        if (2 == AutoHedge->checkState())
        {
            ba = Hedgingaccount2->currentText().toLatin1(); // must
            str += ba.data();
        }
        
        str += ",";

        //存在打开，不存在创建
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        //写入内容,这里需要转码，否则报错。

        //写入QByteArray格式字符串
        
        //box.setText(strData.c_str());
        // box.exec();

        QString res = str.c_str();
        //QTextStream out(&file);//获取转码文件
       // out.setCodec("utf-8");//转码
       // out << res;//写入
        file.write(res.toUtf8());
        //关闭文件
        file.close();
        
        md_server::get_instance()->get_md_spi()->AddStratery(
            Strategyname->text().toLatin1().data(),
            Targetcontract1->text().toLatin1().data(),
            Targetcontract2->text().toLatin1().data(),
            product_market_edit_->text().toLatin1().data(), 
            str);

        QString contract = product_market_edit_->text();
        if (contract.isEmpty()) return;
        ba = contract.toLatin1();
        string strTmp = ba.data();
        md_server::get_instance()->get_md_spi()->
            req_sub_market_data(strTmp);
        //Sleep(2000);
         contract = Targetcontract1->text();
        if (contract.isEmpty()) return;
        ba = contract.toLatin1();
         strTmp = ba.data();
        md_server::get_instance()->get_md_spi()->
            req_sub_market_data(strTmp);

         contract = Targetcontract2->text();
        if (contract.isEmpty()) return;
        ba = contract.toLatin1();
         strTmp = ba.data();
        md_server::get_instance()->get_md_spi()->
            req_sub_market_data(strTmp);

         contract = hedginginstrument1->text();
        if (contract.isEmpty()) return;
        ba = contract.toLatin1();
         strTmp = ba.data();
        md_server::get_instance()->get_md_spi()->
            req_sub_market_data(strTmp);

         contract = hedginginstrument2->text();
        if (contract.isEmpty()) return;
        ba = contract.toLatin1();
         strTmp = ba.data();
        md_server::get_instance()->get_md_spi()->
            req_sub_market_data(strTmp);

       // Sleep(2000);

        TapAPIQuoteWhole& contract1 = md_server::get_instance()->get_md_spi()->m_strategy1.m_targetcontract1;
        TapAPIQuoteWhole& contract2 = md_server::get_instance()->get_md_spi()->m_strategy1.m_targetcontract2;
        
        double BIDASKSPREAD1 = contract1.QAskPrice[0] - contract1.QBidPrice[0];
        double MID1 = (contract1.QAskPrice[0] + contract1.QBidPrice[0]) / 2;

        double BIDASKSPREAD2 = contract2.QAskPrice[0] - contract2.QBidPrice[0];
        double MID2 = (contract2.QAskPrice[0] + contract2.QBidPrice[0]) / 2;
        

        ba = bidspread->text().toLatin1(); // must
        string strbidspread = ba.data();

        ba = askspread->text().toLatin1(); // must
        string straskspread = ba.data();
        

        ba = bidspread1->text().toLatin1(); // must
        string strbidspread1 = ba.data();

        ba = askspread1->text().toLatin1(); // must
        string straskspread1 = ba.data();

        ba = threshold->text().toLatin1(); // must
        string strthreshold = ba.data();

        ba = threshhold1->text().toLatin1(); // must
        string strthreshold1 = ba.data();

        ba = adjustment->text().toLatin1(); // must
        string stradjustment = ba.data();

        double FAIRVALUE = MID1* MID2 + atof(stradjustment.c_str());
        char buf[64];
        snprintf(buf, sizeof(buf), "%f", md_server::get_instance()->get_md_spi()->m_info.CommodityTickSize);
        double HKEXCIN2112;
        double HKEXCIN2113;

        int i, flag = 0, wei = 0;
        md_server::get_instance()->get_md_spi()->m_info.CommodityTickSize = 0.01;
        double dtmp = md_server::get_instance()->get_md_spi()->m_info.CommodityTickSize;
        while (1)
        {
            wei++;
            dtmp = dtmp * 10;
            if (dtmp >= 1)
                break;
        }
       
        //printf("%d\n", wei);

        if (BIDASKSPREAD1 < atof(strthreshold.c_str()) && BIDASKSPREAD2 < atof(strthreshold1.c_str()))
        {
            HKEXCIN2112 = FAIRVALUE - atof(strbidspread.c_str()) * md_server::get_instance()->get_md_spi()->m_info.CommodityTickSize;
            HKEXCIN2113 = FAIRVALUE + atof(straskspread.c_str()) * md_server::get_instance()->get_md_spi()->m_info.CommodityTickSize;
        }
        else
        {
            HKEXCIN2112 = FAIRVALUE - atof(strbidspread1.c_str()) * md_server::get_instance()->get_md_spi()->m_info.CommodityTickSize;
            HKEXCIN2113 = FAIRVALUE + atof(straskspread1.c_str()) * md_server::get_instance()->get_md_spi()->m_info.CommodityTickSize;
        }

        double res1 = Rurd(HKEXCIN2112, wei);
        double res2 = Rurd(HKEXCIN2113, wei);
        

        snprintf(buf, sizeof(buf), "%f %f %d %f %f", HKEXCIN2112, HKEXCIN2113, wei, res1, res2);
        
        box.setText("添加买入策略成功");
         box.exec();
    }
     
    float future_platform::getprevalue(QString name)
    {
        for (int i = 0; i < m_vecPre.size(); i++)
        {
            if (strstr( name.toLatin1().data(), m_vecPre[i].name) != NULL)
            {
                return m_vecPre[i].value;
            }
        }
    }

    int future_platform::GetBidAndAskData(QString strData, double &bid, double &ask, double &combid, double &comask)
    {
        int idx;
        QString tmpdata = strData;
        QStringList list = strData.split(",");
        for (idx = 0; idx < md_server::get_instance()->get_md_spi()->vecStrategy.size(); idx++)
        {
            if (strstr(md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_strategyName.c_str(), list[21].toLatin1().data()) != NULL)
            {
                break;
            }
        }

        if (idx == md_server::get_instance()->get_md_spi()->vecStrategy.size())
            return 0;
        md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_update = false;
        md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_linedata = tmpdata.toLatin1().data();
        
        TapAPIQuoteWhole& contract1 = md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_targetcontract1;
        TapAPIQuoteWhole& contract2 = md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_targetcontract2;
        QByteArray ba;
        double BIDASKSPREAD1 = contract1.QAskPrice[0] - contract1.QBidPrice[0];
        double MID1 = (contract1.QAskPrice[0] + contract1.QBidPrice[0]) / 2;

        double BIDASKSPREAD2 = contract2.QAskPrice[0] - contract2.QBidPrice[0];
        double MID2 = (contract2.QAskPrice[0] + contract2.QBidPrice[0]) / 2;
        combid = md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_productinfo.QBidPrice[0];
        comask = md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_productinfo.QAskPrice[0];

        ba = list[7].toLatin1(); // must
        string strbidspread = ba.data();

        ba = list[8].toLatin1(); // must
        string straskspread = ba.data();


        ba = list[9].toLatin1(); // must
        string strbidspread1 = ba.data();

        ba = list[10].toLatin1(); // must
        string straskspread1 = ba.data();

        ba = list[11].toLatin1(); // must
        string strthreshold = ba.data();

        ba = list[12].toLatin1(); // must
        string strthreshold1 = ba.data();

        ba = list[6].toLatin1(); // must
        string stradjustment = ba.data();
        
        double FAIRVALUE = MID1 * MID2 + atof(stradjustment.c_str());
        char buf[64];
        snprintf(buf, sizeof(buf), "%f", md_server::get_instance()->get_md_spi()->m_info.CommodityTickSize);
        double HKEXCIN2112;
        double HKEXCIN2113;

        int i, flag = 0, wei = 0;
        
        //md_server::get_instance()->get_md_spi()->m_info.CommodityTickSize = getprevalue(list[0]);

        md_server::get_instance()->get_md_spi()->m_info.CommodityTickSize = 0.01;

        double dtmp = md_server::get_instance()->get_md_spi()->m_info.CommodityTickSize;
        while (1)
        {
            wei++;
            dtmp = dtmp * 10;
            if (dtmp >= 1)
                break;
        }

        //printf("%d\n", wei);

        if (BIDASKSPREAD1 < atof(strthreshold.c_str()) && BIDASKSPREAD2 < atof(strthreshold1.c_str()))
        {
            HKEXCIN2112 = FAIRVALUE - atof(strbidspread.c_str()) * md_server::get_instance()->get_md_spi()->m_info.CommodityTickSize;
            HKEXCIN2113 = FAIRVALUE + atof(straskspread.c_str()) * md_server::get_instance()->get_md_spi()->m_info.CommodityTickSize;
        }
        else
        {
            HKEXCIN2112 = FAIRVALUE - atof(strbidspread1.c_str()) * md_server::get_instance()->get_md_spi()->m_info.CommodityTickSize;
            HKEXCIN2113 = FAIRVALUE + atof(straskspread1.c_str()) * md_server::get_instance()->get_md_spi()->m_info.CommodityTickSize;
        }

        bid = Rurd(HKEXCIN2112, wei);
        ask = Rurd(HKEXCIN2113, wei);


       
        return 0;
    }

    void future_platform::slotActionOpen()
    {
#if 1
        QMessageBox box;
       
        QTableWidgetItem* item;
       
        
        int ret;
        int idx;
        item = staterytableWidget->item(staterytableWidget->currentRow(), 1);
        
        for (idx = 0; idx < md_server::get_instance()->get_md_spi()->vecStrategy.size(); idx++)
        {
            string str = item->text().toLatin1().data();
            if (item != NULL && str == md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_strategyName)
            {
                break;
            }
        }

        if (idx == md_server::get_instance()->get_md_spi()->vecStrategy.size())
            return;

        item = staterytableWidget->item(staterytableWidget->currentRow(), 2);
        string contract = item->text().toLatin1().data();

        item = staterytableWidget->item(staterytableWidget->currentRow(), 3);
        string price = item->text().toLatin1().data();

        QString str = md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_linedata.c_str();
        QStringList list = str.split(",");
        box.setText(list[23]);
        
        if (trader_server::get_instance()->get_trader_spi()->userid == list[23])
        // trader_server::get_instance()->get_trader_spi()->m_mapCancelOrder["11"] = "11";
          ret = trader_server::get_instance()->get_trader_spi()->order_create(list[23].toLatin1().data(), contract, atof(price.c_str()), atoi(list[2].toLatin1().data()), TAPI_SIDE_BUY);
        else if (tradesvr1->get_trader_spi()->userid == list[23])
            //tradesvr1->get_trader_spi()->m_mapCancelOrder["11"] = "11";
            tradesvr1->get_trader_spi()->order_create(list[23].toLatin1().data(), contract, atof(price.c_str()), atoi(list[2].toLatin1().data()), TAPI_SIDE_BUY);

        if (ret)
        {
            //QMessageBox::information(staterylist, QString().fromLocal8Bit("创建挂单"), QString().fromLocal8Bit("创建挂单失败"));

        }
        else
        {
           
            //QMessageBox::information(staterylist, QString().fromLocal8Bit("创建挂单"), QString().fromLocal8Bit("创建挂单成功"));
        }
#endif
        //box.setText(item->text());
      //  box.exec();
        
        //box.setText(item->text());
       // box.exec();


       
        //box.setText(item->text());
        //box.exec();
#if 0
            QMap<QString, QString>::Iterator it = trader_server::get_instance()->get_trader_spi()->m_mapCancelOrder.begin();
            if (it == trader_server::get_instance()->get_trader_spi()->m_mapCancelOrder.end())
            {
                ActionOpen(i);
            }

            it = trader_server::get_instance()->get_trader_spi()->m_mapCreateOrder.begin();
            if (it != trader_server::get_instance()->get_trader_spi()->m_mapCreateOrder.end())
            {
                trader_server::get_instance()->get_trader_spi()->order_withdraw(it.key().toLatin1().data());
            }

       
       
            QMap<QString, QString>::Iterator it = tradesvr1->get_trader_spi()->m_mapCancelOrder.begin();
            if (it == tradesvr1->get_trader_spi()->m_mapCancelOrder.end())
            {
                ActionOpen(i);
            }

            it = tradesvr1->get_trader_spi()->m_mapCreateOrder.begin();
            //if (it != tradesvr1->get_trader_spi()->m_mapCreateOrder.end()
              //  && md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_cancelflg == false)
            if (it != tradesvr1->get_trader_spi()->m_mapCreateOrder.end())
            {
                //md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_cancelflg = false;
                tradesvr1->get_trader_spi()->order_withdraw(it.key().toLatin1().data());
            }
#endif
            //tradesvr1->get_trader_spi()->order_withdraw("11");
        
       
        m_isopen = true;

        
    }

    void future_platform::tableOpenMenuRequested(const QPoint& pos)
    {
        
        
        stratery_widget_menu->exec(QCursor::pos());
    }

    void GetFileName(QString path, QStringList &string_list)
    {
        QDir dir(path);
        if (!dir.exists())
         return;

        //查看路径中后缀为.cfg格式的文件
        QStringList filters;
        filters << QString("*");
        dir.setFilter(QDir::Files | QDir::NoSymLinks); //设置类型过滤器，只为文件格式
        dir.setNameFilters(filters);  //设置文件名称过滤器，只为filters格式

        //统计cfg格式的文件个数
        int dir_count = dir.count();
        if (dir_count <= 0)
        return;

        //  //测试
        //  //文件路径及名称
        //  QFile outFile( "filename.txt");
        //  //看能否打开
        //  if(!outFile.open(QIODevice::WriteOnly | QIODevice::Append))  
        //      return ;  
        //  QTextStream ts(&outFile);

        //存储文件名称
        
        for (int i = 0; i < dir_count; i++)
        {
            QString file_name = path + dir[i];  //文件名称
            //ts<<file_name<<"\r\n"<<"\r\n";
            string_list.append(file_name);

           
        }

        QMessageBox box;
        char tmp[4];
        snprintf(tmp, sizeof(tmp), "%d", string_list.size());
        box.setText(tmp);
        //box.setText(item->text());
       // box.exec();
    }

    void future_platform::show_stratery()
    {
        staterylist = new QWidget();
        QStringList string_list;
        GetFileName(QCoreApplication::applicationDirPath() + "/stratery/", string_list);
        QVBoxLayout* vmainlayout = new QVBoxLayout(staterylist);
        QWidget* widget = new QWidget();
        vmainlayout->addWidget(widget);

        QHBoxLayout* mainlayout = new QHBoxLayout();
        staterytableWidget = new QTableWidget();
        staterytableWidget->setColumnCount(13);
        QStringList header;

        string key;
        key = "md_info/userid";
        QString userid = common::get_config_value(key).toString();


        //header << QStringLiteral("产品") << QStringLiteral("账号");
        staterytableWidget->setHorizontalHeaderLabels(QStringList() << tr("status") << tr("Strategyname")
            << tr("product") << tr("theo bid") << tr("mkt bid") << tr("mkt ask") << tr("theo ask") << tr("adjustment")
            << tr("strategy") << tr("contract1") << tr("contract2") << tr("STOP TIME") << tr("Auto-Hedge"));
        int line = 0;
        const int SIZE = 1;
        string filename[SIZE] = { "CINZ1SGXT+1" };


        QHeaderView* headerview = staterytableWidget->horizontalHeader();
        for (int i = 0; i < string_list.size(); i++)
        {
            QString name = string_list[i];
            QFile file(name);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                qDebug() << "Can't open the file!" << endl;
                file.close();
                continue;
            }
            QTextCodec* codec = QTextCodec::codecForName("utf-8");//设置文件编码格式
            QByteArray data = file.readLine();
            double bid;
            double ask;
            double combid;
            double comask;
            
            QString str = codec->toUnicode(data);//转码;
            QStringList list = str.split(",");

            if (md_server::get_instance()->get_md_spi()->vecStrategy.size() == 0)
            {
                md_server::get_instance()->get_md_spi()->AddStratery(
                    list[21].toLatin1().data(),
                    list[4].toLatin1().data(),
                    list[5].toLatin1().data(),
                    list[0].toLatin1().data(),
                    data.data());

                md_server::get_instance()->get_md_spi()->
                    req_sub_market_data(list[0].toLatin1().data());
                //Sleep(2000);

                md_server::get_instance()->get_md_spi()->
                    req_sub_market_data(list[4].toLatin1().data());

                md_server::get_instance()->get_md_spi()->
                    req_sub_market_data(list[5].toLatin1().data());
            }

            Sleep(1000);
            GetBidAndAskData(data, bid, ask, combid, comask);
            staterytableWidget->setRowCount(line + 1);
            int col = 0;
            staterytableWidget->setItem(line, col++, new QTableWidgetItem("stop"));
            staterytableWidget->setItem(line, col++, new QTableWidgetItem(list[21]));
            staterytableWidget->setItem(line, col++, new QTableWidgetItem(list[0]));
            char buf[16];
            snprintf(buf, sizeof(buf), "%.2f", bid);
            

            staterytableWidget->setItem(line, col++, new QTableWidgetItem(buf));
            
            snprintf(buf, sizeof(buf), "%.2f", combid);
            staterytableWidget->setItem(line, col++, new QTableWidgetItem(buf));
            snprintf(buf, sizeof(buf), "%.2f", comask);
            staterytableWidget->setItem(line, col++, new QTableWidgetItem(buf));
            
            snprintf(buf, sizeof(buf), "%.2f", ask);
            staterytableWidget->setItem(line, col++, new QTableWidgetItem(buf));
          
            staterytableWidget->setItem(line, col++, new QTableWidgetItem(list[6]));
            if (list[1] == "0")
                staterytableWidget->setItem(line, col++, new QTableWidgetItem("单边做市1"));
            else if (list[1] == "1")
                staterytableWidget->setItem(line, col++, new QTableWidgetItem("单边做市2"));
            else if (list[1] == "2")
                staterytableWidget->setItem(line, col++, new QTableWidgetItem("双边做市1"));
            else if (list[1] == "3")
                staterytableWidget->setItem(line, col++, new QTableWidgetItem("双边做市1"));
            
            staterytableWidget->setItem(line, col++, new QTableWidgetItem(list[4]));
            staterytableWidget->setItem(line, col++, new QTableWidgetItem(list[5]));
            staterytableWidget->setItem(line, col++, new QTableWidgetItem(list[20]));
            staterytableWidget->setItem(line, col++, new QTableWidgetItem(list[13]));


            line++;
            file.close();



        }

        staterytableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        staterytableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        staterytableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);


        // connect(tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(tableItemClicked(int, int)));
        stratery_widget_menu = new QMenu(staterytableWidget);
        strateryaction = new QAction("开启", this);
       connect(strateryaction, SIGNAL(triggered()), this, SLOT(slotActionOpen()));
       stratery_widget_menu->addAction(strateryaction);
       connect(staterytableWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tableOpenMenuRequested(QPoint)));

        //last--;

        staterytableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        staterytableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

        //headerview->setSectionResizeMode(QHeaderView::Stretch);
        mainlayout->addWidget(staterytableWidget);
        staterytableWidget->scrollToBottom();
        // mainlayout->addWidget(tableWidget);
        mainlayout->setMargin(10);
        mainlayout->setSpacing(5);
        widget->setLayout(mainlayout);
        staterylist->setWindowTitle("策略信息");
        //newwin->setFixedSize(1200, 800);
        staterylist->resize(1200, 800);
        staterylist->setAttribute(Qt::WA_QuitOnClose, false);
       
       
        staterylist->show();
    }

    void future_platform::addselltrategy()
    {
        QString fileName = Strategyname->text();
        
        QFile file(fileName);
        if (file.exists())
        {
            QFile::remove(fileName);
        }

        //存在打开，不存在创建
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        //写入内容,这里需要转码，否则报错。

        QByteArray ba;
        string str = gettrategystr();

        str += "1";
        str += ",";

        ba = account_check_->currentText().toLatin1(); // must
        str += ba.data();
        str += ",";

        ba = Hedgingaccount1->currentText().toLatin1(); // must
        str += ba.data();
        str += ",";

        ba = Hedgingaccount2->currentText().toLatin1(); // must
        str += ba.data();
        str += ",";

        //写入QByteArray格式字符串
        QMessageBox box;
        //box.setText(strData.c_str());
        // box.exec();
        file.write(gettrategystr().c_str());
        //关闭文件
        file.close();
    }

    void future_platform::savetrategy()
    {
        QString fileName = QFileDialog::getSaveFileName(this,QString::fromLocal8Bit("文件另存为"),"",tr("Config Files (*.txt)"));

        QFile file(fileName);
        if (file.exists())
        {
            QFile::remove(fileName);
        }

        //存在打开，不存在创建
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        //写入内容,这里需要转码，否则报错。
        
        //写入QByteArray格式字符串
        QMessageBox box;
        //box.setText(strData.c_str());
       // box.exec();
        file.write(gettrategystr().c_str());
        //关闭文件
        file.close();
    }

    void future_platform::loadtrategy()
    {
        QString fileName = QFileDialog::getOpenFileName();
        
        TapAPIContract stContract;
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) 
        {
            qDebug() << "Can't open the file!" << endl;
        }

        QByteArray line = file.readLine();
        QString str(line);

        QStringList list = str.split(",");
        
        
        product_market_edit_->setText(list[0]);
        
        market_check_->setCurrentText(list[1]);
        
        buy_market_num_->setText(list[2]);

        sell_market_num_->setText(list[3]);
        
        

        Targetcontract1->setText(list[4]);

        Targetcontract2->setText(list[5]);

        adjustment->setText(list[6]);
        
        

       bidspread->setText(list[7]);
        


        askspread->setText(list[8]);
        
        bidspread1->setText(list[9]);
        

        askspread1->setText(list[10]);
        

        threshold->setText(list[11]);
        

        threshhold1->setText(list[12]);
        
        

        int state = list[13].toInt();
        AutoHedge->setCheckState((Qt::CheckState)state);
        
#if 1
        hedginginstrument1->setText(list[14]);
        
        

        slippage1->setText(list[15]);
        

        radio1->setText(list[16]);

        hedginginstrument2->setText(list[17]); // must
        
        

        slippage2->setText(list[18]);
        
        radio3->setText(list[19]);
        

        stoptime->setText(list[20]);
        

        Strategyname->setText(list[21]);
        
#endif

        //关闭文件
        file.close();

    }

    void future_platform::addstrategy()
    {

        wstrategy = new QWidget;
        wstrategy->setAttribute(Qt::WA_QuitOnClose, false);
        QLineEdit edit;
        QGridLayout gLayout;
        QLabel* aselect_label = new QLabel("账号");
        account_check_ = new QComboBox();
        gLayout.addWidget(aselect_label, 0, 1);
        gLayout.addWidget(account_check_, 0, 2);
        QStringList strList;
        for (int i = 0; i < m_vec_account.size(); i++)
        {
            strList << m_vec_account[i];
        }
        account_check_->addItems(strList);


        QLabel* product_label = new QLabel("产品");
        product_market_edit_ = new QLineEdit();
        gLayout.addWidget(product_label, 1, 1);
        gLayout.addWidget(product_market_edit_, 1, 2);

        QLabel* aselect_label1 = new QLabel("做市");
        market_check_ = new QComboBox();
        gLayout.addWidget(aselect_label1, 2, 1);
        gLayout.addWidget(market_check_, 2, 2);
        QStringList marketList;
        
        marketList << "单边做市1";
        marketList << "单边做市2";
        marketList << "双边做市1";
        marketList << "双边做市2";
        market_check_->addItems(marketList);

        QPushButton* save = new QPushButton("保存");
        QPushButton* load = new QPushButton("加载");

        gLayout.addWidget(save, 3, 1);
        gLayout.addWidget(load, 3, 2);

        QLabel* num_label = new QLabel("目标合约1");
        Targetcontract1 = new QLineEdit();
        gLayout.addWidget(num_label, 5, 1);
        gLayout.addWidget(Targetcontract1, 5, 2);

        num_label = new QLabel("目标合约2");
        Targetcontract2 = new QLineEdit();
        gLayout.addWidget(num_label, 5, 3);
        gLayout.addWidget(Targetcontract2, 5, 4);

        num_label = new QLabel("买入数");
        buy_market_num_ = new QLineEdit();
        gLayout.addWidget(num_label, 4, 1);
        gLayout.addWidget(buy_market_num_, 4, 2);

       num_label = new QLabel("卖出数");
       sell_market_num_ = new QLineEdit();
        gLayout.addWidget(num_label, 4, 3);
        gLayout.addWidget(sell_market_num_, 4, 4);

        num_label = new QLabel("adjustment");
        adjustment = new QLineEdit();
        gLayout.addWidget(num_label, 6, 1);
        gLayout.addWidget(adjustment, 6, 2);


        num_label = new QLabel("bid spread");
        bidspread = new QLineEdit();
        gLayout.addWidget(num_label, 7, 1);
        gLayout.addWidget(bidspread, 7, 2);

        num_label = new QLabel("ask spread");
        askspread = new QLineEdit();
        gLayout.addWidget(num_label, 7, 3);
        gLayout.addWidget(askspread, 7, 4);

        num_label = new QLabel("bid spread1");
        bidspread1 = new QLineEdit();
        gLayout.addWidget(num_label, 8, 1);
        gLayout.addWidget(bidspread1, 8, 2);

        num_label = new QLabel("ask spread1");
        askspread1 = new QLineEdit();
        gLayout.addWidget(num_label, 8, 3);
        gLayout.addWidget(askspread1, 8, 4);

        num_label = new QLabel("threshold1");
        threshold = new QLineEdit();
        gLayout.addWidget(num_label, 9, 1);
        gLayout.addWidget(threshold, 9, 2);

        num_label = new QLabel("threshold2");
        threshhold1 = new QLineEdit();
        gLayout.addWidget(num_label, 9, 3);
        gLayout.addWidget(threshhold1, 9, 4);

        num_label = new QLabel("Auto Hedge");
        AutoHedge = new QCheckBox();
        gLayout.addWidget(num_label, 10, 1);
        gLayout.addWidget(AutoHedge, 10, 2);

        num_label = new QLabel("hedging instrument1");
        hedginginstrument1 = new QLineEdit();
        gLayout.addWidget(num_label, 11, 1);
        gLayout.addWidget(hedginginstrument1, 11, 2);

        num_label = new QLabel("对冲账户1");
        Hedgingaccount1 = new QComboBox();
        gLayout.addWidget(num_label, 11, 3);
        gLayout.addWidget(Hedgingaccount1, 11, 4);
        
        Hedgingaccount1->addItems(strList);

        num_label = new QLabel("slippage1");
        slippage1 = new QLineEdit();
        gLayout.addWidget(num_label, 12, 1);
        gLayout.addWidget(slippage1, 12, 2);

        num_label = new QLabel("ratio1");
        radio1 = new QLineEdit();
        gLayout.addWidget(num_label, 13, 1);
        gLayout.addWidget(radio1, 13, 2);

        num_label = new QLabel("hedging instrument2");
        hedginginstrument2 = new QLineEdit();
        gLayout.addWidget(num_label, 14, 1);
        gLayout.addWidget(hedginginstrument2, 14, 2);

        num_label = new QLabel("对冲账户2");
        Hedgingaccount2 = new QComboBox();
        gLayout.addWidget(num_label, 14, 3);
        gLayout.addWidget(Hedgingaccount2, 14, 4);

        Hedgingaccount2->addItems(strList);

        num_label = new QLabel("slippage2");
        slippage2 = new QLineEdit();
        gLayout.addWidget(num_label, 15, 1);
        gLayout.addWidget(slippage2, 15, 2);

        num_label = new QLabel("ratio2");
        radio3 = new QLineEdit();
        gLayout.addWidget(num_label, 16, 1);
        gLayout.addWidget(radio3, 16, 2);

        num_label = new QLabel("停止时间");
        stoptime = new QLineEdit();
        gLayout.addWidget(num_label, 17, 1);
        gLayout.addWidget(stoptime, 17, 2);

        num_label = new QLabel("策略名");
        Strategyname = new QLineEdit();
        gLayout.addWidget(num_label, 17, 3);
        gLayout.addWidget(Strategyname, 17, 4);

       // QLineEdit* slippage1;
        //QLineEdit* slippage2;

        //QLineEdit* hedginginstrument2;
        

        QPushButton* buy = new QPushButton("添加买入策略");
        QPushButton* sell = new QPushButton("添加卖出策略");


        gLayout.addWidget(buy, 18, 1);
        gLayout.addWidget(sell, 18, 2);

        connect(save, SIGNAL(clicked()),
            this, SLOT(savetrategy()));

        connect(load, SIGNAL(clicked()),
            this, SLOT(loadtrategy()));

        connect(buy, SIGNAL(clicked()), this, SLOT(addbuytrategy()));
        connect(sell, SIGNAL(clicked()), this, SLOT(addselltrategy()));

        wstrategy->setLayout(&gLayout);
        wstrategy->setWindowTitle("做市策略");
        //connect(select_check_, SIGNAL(currentIndexChanged(const QString)), this, SLOT(check_select(const QString)));

        wstrategy->setAttribute(Qt::WA_QuitOnClose, false);
        wstrategy->show();

        // QMessageBox box;
        // box.setText("Another ways to use MessageBox");
         //box.exec();
      // QString log = "test";
       //slot_write_log(log);
    }


    void future_platform::transaction_account()
    {
        
        w = new QWidget;
        w->setAttribute(Qt::WA_QuitOnClose, false);
        QLineEdit edit;
        QGridLayout gLayout;
        QLabel* aselect_label = new QLabel("账号");
        aselect_check_ = new QComboBox();
        gLayout.addWidget(aselect_label, 0, 1);
        gLayout.addWidget(aselect_check_, 0, 2);
        QStringList strList;
        for (int i = 0; i < m_vec_account.size(); i++)
        {
            strList << m_vec_account[i];
        }
        
        aselect_check_->addItems(strList);
        QLabel* product_label = new QLabel("产品");
        product_line_edit_ = new QLineEdit();
        gLayout.addWidget(product_label, 1, 1);
        gLayout.addWidget(product_line_edit_, 1, 2);

        
        QLabel *price_label = new QLabel("价格");
        price_line_edit_ = new QLineEdit();
        gLayout.addWidget(price_label, 2, 1);
        gLayout.addWidget(price_line_edit_, 2, 2);

        QLabel* num_label = new QLabel("数量");
        num_line_edit_ = new QLineEdit();
        gLayout.addWidget(num_label, 3, 1);
        gLayout.addWidget(num_line_edit_, 3, 2);

        QLabel* select_label = new QLabel("选项");
        QComboBox* select_check_ = new QComboBox();
        gLayout.addWidget(select_label, 4, 1);
        gLayout.addWidget(select_check_, 4, 2);
        QStringList strList1;
        strList1 << "普通交易" << "庄家订单";
        select_check_->addItems(strList1);

        QPushButton *buy = new QPushButton("买入");
        QPushButton* sell = new QPushButton("卖出");

        
        gLayout.addWidget(buy, 5, 1);
        gLayout.addWidget(sell, 5, 2);

        connect(buy, SIGNAL(clicked()), this, SLOT(buy_order()));
        connect(sell, SIGNAL(clicked()), this, SLOT(sell_order()));

        w->setLayout(&gLayout);
        w->setWindowTitle("交易指示");
        connect(select_check_, SIGNAL(currentIndexChanged(const QString)), this, SLOT(check_select(const QString)));
        
        w->setAttribute(Qt::WA_QuitOnClose, false);
        w->show();



        // QMessageBox box;
        // box.setText("Another ways to use MessageBox");
         //box.exec();
      // QString log = "test";
       //slot_write_log(log);
    }

    void future_platform::slot_connnet_dialog()
    {
        QString contract = contract_line_edit_->text();
        if (contract.isEmpty()) return;
        QByteArray ba = contract.toLatin1();
        string strTmp = ba.data();
        
        if (operator_btn_->text() == "连接") {
            
            //md_server::get_instance()->get_md_spi()->
              //  req_sub_market_data(contract.toStdString());
            md_server::get_instance()->get_md_spi()->
                req_sub_market_data(strTmp);
            string key = "md_info/contractid";


            common::set_config_value(key, strTmp);
            operator_btn_->setText("停止");
            contract_line_edit_->setEnabled(false);
        } else {
            md_server::get_instance()->get_md_spi()->
                req_unsub_market_data(strTmp);
            operator_btn_->setText("连接");
            contract_line_edit_->setEnabled(true);
            quote_line_edit_->setText(QString::number(0, 10, 0));
            quote_diff_label_->setText(QString::number(0, 10, 0));
        }
    }
    
    void future_platform::slot_order_open()
    {
        if (order_open_btn_->text() == "挂单") {
            
            QString qaccount = (t_user_line_edit_->text().split("/"))[1];
            QByteArray ba = qaccount.toLatin1();
            //string account = qaccount.toStdString();
            //string contract = contract_line_edit_->text().toStdString();
            string account = ba.data();
            ba = contract_line_edit_->text().toLatin1();
            string contract = ba.data();
            double price = dot_line_edit_->text().toDouble() * 0.01 +
                quote_line_edit_->text().toDouble();

            trader_server::get_instance()->get_trader_spi()->order_open(
                account, contract, price);
        } 
        else {
            //trader_server::get_instance()->get_trader_spi()->order_withdraw();
        }
    }

    void future_platform::slot_order_close()
    {
        QString qaccount = (t_user_line_edit_->text().split("/"))[1];

		QByteArray ba = qaccount.toLatin1();
       // string account = qaccount.toStdString();
		string account = ba.data();
        //string contract = contract_line_edit_->text().toStdString();
		ba = contract_line_edit_->text().toLatin1();
		string contract = ba.data();
        trader_server::get_instance()->get_trader_spi()->order_close(
            account, contract);
    }

    void future_platform::slot_write_log(QString str)
    {
        QDateTime now = QDateTime::currentDateTime();
        QString time_str = now.toString("hh:mm:ss");
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss.zzz");
        QString log = QObject::tr("%1  %2").arg(current_date).arg(str);
       // log_text_edit_->append(log);
        logtableWidget->setRowCount(m_table_row);
        logtableWidget->setItem(m_table_row-1, 0, new QTableWidgetItem(log));
        m_table_row++;
        logtableWidget->scrollToBottom();
        QByteArray cdata = log.toLocal8Bit();
        string strdata = cdata;
        //APP_LOG(applog::LOG_INFO) << log.toLatin1().data();
        APP_LOG(applog::LOG_INFO) << strdata.c_str();
#if 1

        if (NULL != newwin && true == newwin->isVisible())
        {
            QVector<TapAPIOrderInfo>& vecOrder = trader_server::get_instance()->get_trader_spi()->m_add_order;
            for (int i = 0; i < vecOrder.size(); i++)
            {
                slot_add_order(&vecOrder[i]);
            }

            QVector<TapAPIOrderInfo> vecSwap;
            vecSwap.swap(vecOrder);
        }

        if (NULL != fillwin && true == fillwin->isVisible())
        {
            EnterCriticalSection(&cs);
            QVector<TapAPIFillInfo>& vecOrder = trader_server::get_instance()->get_trader_spi()->m_add_fill;

            QVector<TapAPIFillInfo>::iterator itr = vecOrder.begin();
            while (itr != vecOrder.end())
            {
                slot_add_trans(itr);
                itr = vecOrder.erase(itr);
            }
            LeaveCriticalSection(&cs);
        }
#endif
        if (NULL != newwin && true == newwin->isVisible())
        {
            QVector<TapAPIOrderInfo>& vecOrder = tradesvr1->get_trader_spi()->m_add_order;
            int size = vecOrder.size();
            for (int i = 0; i < size; i++)
            {
                slot_add_order(&vecOrder[i]);
            }

            QVector<TapAPIOrderInfo> vecSwap;
            vecSwap.swap(vecOrder);
        }
       
        if (NULL != fillwin && true == fillwin->isVisible())
        {
            EnterCriticalSection(&cs);
            QVector<TapAPIFillInfo>& vecOrder = tradesvr1->get_trader_spi()->m_add_fill;

            QVector<TapAPIFillInfo>::iterator itr = vecOrder.begin();
            while (itr != vecOrder.end())
            {
                slot_add_trans(itr);
                 itr = vecOrder.erase(itr);
            }
            LeaveCriticalSection(&cs);
        }

        //slot_add_order
    }

    void future_platform::slot_quote_changed(QString last_price)
    {
        quote_line_edit_->setText(last_price);

        QString log = QObject::tr("%1 %2").arg("当前价格").arg(last_price);
        slot_write_log(log);
    }

    void future_platform::slot_state_changed(TapAPIOrderInfo* _t1)
    {
       
        slot_write_log("testttttt");
    }

#if 1
    void future_platform::slot_add_trans(TapAPIFillInfo *info1)
    {
        //last--;
        
        int col = 0;
        TapAPIFillInfo& info = *info1;
        QVector<TapAPIFillInfo>& map = trader_server::get_instance()->GetFillMap();
        QVector<TapAPIFillInfo>& map1 = tradesvr1->GetFillMap();

        int line = m_fill_row;
        m_fill_row++;
        filltableWidget->setRowCount(line + 1);
        col = 0;
        filltableWidget->setItem(line, col++, new QTableWidgetItem(info.AccountNo));
        filltableWidget->setItem(line, col++, new QTableWidgetItem(info.ExchangeNo));
        QString str;
        str.sprintf("%c", info.CommodityType);
        filltableWidget->setItem(line, col++, new QTableWidgetItem(str));
        filltableWidget->setItem(line, col++, new QTableWidgetItem(info.CommodityNo));

        filltableWidget->setItem(line, col++, new QTableWidgetItem(info.ContractNo));

        filltableWidget->setItem(line, col++, new QTableWidgetItem(info.StrikePrice));
        str.sprintf("%c", info.CallOrPutFlag);
        filltableWidget->setItem(line, col++, new QTableWidgetItem(str));
        str.sprintf("%c", info.MatchSide);
        filltableWidget->setItem(line, col++, new QTableWidgetItem(str));
        filltableWidget->setItem(line, col++, new QTableWidgetItem(info.OrderNo));
        filltableWidget->setItem(line, col++, new QTableWidgetItem(info.MatchDateTime));

        str.sprintf("%f", info.MatchPrice);
        filltableWidget->setItem(line, col++, new QTableWidgetItem(str));

        str.sprintf("%d", info.MatchQty);
        filltableWidget->setItem(line, col++, new QTableWidgetItem(str));        
        filltableWidget->scrollToBottom();
    }

    void future_platform::GetPreInfo()
    {
        FILE* fp;
        QString conf_path = QCoreApplication::applicationDirPath() + "/precision.txt";
        if ((fp = fopen(conf_path.toLatin1().data(), "r")) == NULL)
        {
            printf("打开文件%s错误\n", "accounts.txt");
            return;
        }

        char buff[32];
        while (!feof(fp))
        {
            memset(buff, 0, sizeof(buff));
            fgets(buff, sizeof(buff), fp);
            char* pos = strstr(buff, "\r");
            if (pos != NULL)
            {
                *pos = '\0';
            }

            pos = strstr(buff, "\n");
            if (pos != NULL)
            {
                *pos = '\0';
            }

            StPrecision pre;
            
            char tmpdata[32] = { 0 };
            pos = strstr(buff, " ");
            if (pos != NULL)
            {
                
                strncpy(pre.name, buff, pos - buff);
                strcpy(tmpdata, pos + 1);
                pre.value = atof(tmpdata);
                m_vecPre.push_back(pre);
            }

            QMessageBox box;
            //box.setText(tmp);
            
            //box.setText(item->text());
            //box.exec();
           
        }
    }

    void future_platform::ActionOpen(int row)
    {
        QMessageBox box;

        QTableWidgetItem* item;


        int ret;
        int idx;
        item = staterytableWidget->item(row, 1);

        for (idx = 0; idx < md_server::get_instance()->get_md_spi()->vecStrategy.size(); idx++)
        {
            string str = item->text().toLatin1().data();
            if (item != NULL && str == md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_strategyName)
            {
                break;
            }
        }

        if (idx == md_server::get_instance()->get_md_spi()->vecStrategy.size())
            return;

        item = staterytableWidget->item(row, 2);
        string contract = item->text().toLatin1().data();

        item = staterytableWidget->item(row, 3);
        string price = item->text().toLatin1().data();

        QString str = md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_linedata.c_str();
        QStringList list = str.split(",");
        box.setText(list[23]);

        if (trader_server::get_instance()->get_trader_spi()->userid == list[23])
            ret = trader_server::get_instance()->get_trader_spi()->order_create(list[23].toLatin1().data(), contract, atof(price.c_str()), atoi(list[2].toLatin1().data()), TAPI_SIDE_BUY);
        else if (tradesvr1->get_trader_spi()->userid == list[23])
            tradesvr1->get_trader_spi()->order_create(list[23].toLatin1().data(), contract, atof(price.c_str()), atoi(list[2].toLatin1().data()), TAPI_SIDE_BUY);

        if (ret)
        {
           // QMessageBox::information(staterylist, QString().fromLocal8Bit("创建挂单"), QString().fromLocal8Bit("创建挂单失败"));

        }
        else
        {
           // QMessageBox::information(staterylist, QString().fromLocal8Bit("创建挂单"), QString().fromLocal8Bit("创建挂单成功"));
        }
        //box.setText(item->text());
      //  box.exec();

        //box.setText(item->text());
       // box.exec();



        //box.setText(item->text());
        //box.exec();

    }
    
    void future_platform::slot_update_stratery()
    {
        if (NULL != staterylist && true == staterylist->isVisible())
        {
            int idx = 0;

            QString str = md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_linedata.c_str();
            QString tmpdata = md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_linedata.c_str();
            int rowcount = staterytableWidget->rowCount();
            QTableWidgetItem* item;
            QStringList list = str.split(",");
            char buf[16];

            for (int i = 0; i < rowcount; i++)
            {
                item = staterytableWidget->item(i, 1);
                if (item != NULL && item->text() == list[21])
                {
                    double bid;
                    double ask;
                    double combid;
                    double comask;
                    GetBidAndAskData(tmpdata, bid, ask, combid, comask);
                    int ret;
                    if (true == m_isopen)
                    {
                        if (trader_server::get_instance()->get_trader_spi()->userid == list[23])
                        {
                            QMap<QString, QString>::Iterator it = trader_server::get_instance()->get_trader_spi()->m_mapCancelOrder.begin();
                            if (it == trader_server::get_instance()->get_trader_spi()->m_mapCancelOrder.end())
                            {
                                ActionOpen(i);
                            }

                            it = trader_server::get_instance()->get_trader_spi()->m_mapCreateOrder.begin();
                            if (it != trader_server::get_instance()->get_trader_spi()->m_mapCreateOrder.end() 
                                && false == md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_cancelflg)
                            {
                                md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_cancelflg = true;
                                trader_server::get_instance()->get_trader_spi()->order_withdraw(it.key().toLatin1().data());
                            }

                        }
                        else if (tradesvr1->get_trader_spi()->userid == list[23])
                        {
                            QMap<QString, QString>::Iterator it = tradesvr1->get_trader_spi()->m_mapCancelOrder.begin();
                            if (it == tradesvr1->get_trader_spi()->m_mapCancelOrder.end())
                            {
                                ActionOpen(i);
                            }

                            it = tradesvr1->get_trader_spi()->m_mapCreateOrder.begin();
                            //if (it != tradesvr1->get_trader_spi()->m_mapCreateOrder.end()
                              //  && md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_cancelflg == false)
                            if (it != tradesvr1->get_trader_spi()->m_mapCreateOrder.end()
                                && false == md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_cancelflg)
                            {
                                md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_cancelflg = true;
                                tradesvr1->get_trader_spi()->order_withdraw(it.key().toLatin1().data());
                            }
                            //tradesvr1->get_trader_spi()->order_withdraw("11");
                        }

                    }


                    snprintf(buf, sizeof(buf), "%.2f", bid);
                    staterytableWidget->setItem(i, 3, new QTableWidgetItem(buf));
                    snprintf(buf, sizeof(buf), "%.2f", ask);
                    staterytableWidget->setItem(i, 6, new QTableWidgetItem(buf));

                    snprintf(buf, sizeof(buf), "%.2f", md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_productinfo.QBidPrice[0]);
                    staterytableWidget->setItem(i, 4, new QTableWidgetItem(buf));
                    snprintf(buf, sizeof(buf), "%.2f", md_server::get_instance()->get_md_spi()->vecStrategy[idx].m_productinfo.QAskPrice[0]);
                    staterytableWidget->setItem(i, 5, new QTableWidgetItem(buf));
                    break;
                }

            }

           
        }

    }

    void future_platform::slot_add_order(TapAPIOrderInfo *info1)
    {
       
            TapAPIOrderInfo &info = *info1;
            QMap<QString, TapAPIOrderInfo>& map = trader_server::get_instance()->GetOrderMap();
            QMap<QString, TapAPIOrderInfo>& map1 = tradesvr1->GetOrderMap();
            int rowcount = ordertableWidget->rowCount();
            QTableWidgetItem* item;
            
            for (int i = 0; i < rowcount; i++)
            {
                item = ordertableWidget->item(i, 10);
                if (item != NULL && item->text() == info1->OrderNo)
                {
                    ordertableWidget->removeRow(i);
                }
                
            }

            rowcount = ordertableWidget->rowCount();
            //int line = map.size() + map1.size();
            int line = rowcount;
            m_order_row++;
            //line--;
           // TapAPIOrderInfo& info = it.value();
            string Contract, Contract2, OrderState;
            Contract = info.ContractNo;
            Contract2 = info.ContractNo2;
            if (Contract2.empty()) {
            }
            else {
                Contract = Contract + "/" + Contract2;
            }
            switch (info.OrderState) {

            case '0':
                OrderState = "SUMBITTED";
                break;
            case '1':
                OrderState = "ACCEPTED";
                break;
            case '4':
                OrderState = "QUEUED";
                break;
            case '5':
                OrderState = "PARTFINISHED";
                break;
            case '6':
                OrderState = "FINISHED";
                break;
            case '7':
                OrderState = "CANCELLING";
                break;
            case '8':
                OrderState = "MODIFYING";
                break;
            case '9':
                OrderState = "CANCELLED";
                break;
            case 'A':
                OrderState = "LEFTDELETED";
                break;
            case 'B':
                OrderState = "FAILED";
                break;
            default:
                OrderState = info.OrderState;
            }

            ordertableWidget->setContextMenuPolicy(Qt::CustomContextMenu);


            ordertableWidget->setRowCount(line + 1);
            int col = 0;
            ordertableWidget->setItem(line, col++, new QTableWidgetItem(info.AccountNo));
            ordertableWidget->setItem(line, col++, new QTableWidgetItem(info.ExchangeNo));
            QString str;
            str.sprintf("%c", info.CommodityType);
            ordertableWidget->setItem(line, col++, new QTableWidgetItem(str));
            ordertableWidget->setItem(line, col++, new QTableWidgetItem(info.CommodityNo));

            ordertableWidget->setItem(line, col++, new QTableWidgetItem(Contract.c_str()));

            ordertableWidget->setItem(line, col++, new QTableWidgetItem(info.StrikePrice));
            str.sprintf("%c", info.CallOrPutFlag);

            ordertableWidget->setItem(line, col++, new QTableWidgetItem(str));
            str.sprintf("%c", info.OrderSide);
            ordertableWidget->setItem(line, col++, new QTableWidgetItem(str));
            str.sprintf("%f", info.OrderPrice);
            ordertableWidget->setItem(line, col++, new QTableWidgetItem(str));
            str.sprintf("%d", info.OrderQty);
            ordertableWidget->setItem(line, col++, new QTableWidgetItem(str));
            ordertableWidget->setItem(line, col++, new QTableWidgetItem(info.OrderNo));
            char* pos = strchr(info.OrderInsertTime, '.');

            string strFullTime;
            if (pos != NULL)
            {

                char time[20] = { 0 };
                strncpy(time, info.OrderInsertTime, pos - info.OrderInsertTime);
                GetTime(time, strFullTime);
            }
            //tableWidget->setItem(line, col++, new QTableWidgetItem(info.OrderInsertTime));
            ordertableWidget->setItem(line, col++, new QTableWidgetItem(strFullTime.c_str()));

            pos = strchr(info.OrderUpdateTime, '.');


            if (pos != NULL)
            {
                char time[20] = { 0 };
                strncpy(time, info.OrderUpdateTime, pos - info.OrderUpdateTime);
                GetTime(time, strFullTime);
            }

            ordertableWidget->setItem(line, col++, new QTableWidgetItem(strFullTime.c_str()));
            ordertableWidget->setItem(line, col++, new QTableWidgetItem(OrderState.c_str()));
            str.sprintf("%d", info.OrderMatchQty);
            ordertableWidget->setItem(line, col++, new QTableWidgetItem(str));
            ordertableWidget->scrollToBottom();
        
    }
#endif
    void future_platform::slot_close_position(QString commodity_no, QString contract_no)
    {
        contract_line_edit_->setText(commodity_no+contract_no);
        order_open_btn_->setEnabled(false);
        order_close_btn_->setHidden(false);
    }

    void future_platform::slot_withdraw_order(QString order_no)
    {
        order_open_btn_->setText("弃单");
    }

    void future_platform::slot_quote_reconnect()
    {
        QString contract = contract_line_edit_->text();
        if (contract.isEmpty()) return;
        if (operator_btn_->text() == "停止") {
            md_server::get_instance()->get_md_spi()->
                req_sub_market_data(contract.toStdString());
        }
    }

    void future_platform::init()
    {
        string key = "md_info/ip";
        QString ip = common::get_config_value(key).toString();
        key = "md_info/userid";
        QString userid = common::get_config_value(key).toString();
        q_user_line_edit_->setText(ip + "/" + userid);

        key = "md_info/contractid";
        QString md_contractid = common::get_config_value(key).toString();
        contract_line_edit_->setText(md_contractid);

        key = "trader_info/ip";
        ip = common::get_config_value(key).toString();
        key = "trader_info/userid";
        userid = common::get_config_value(key).toString();
        t_user_line_edit_->setText(ip + "/" + userid);

        dot_line_edit_->setText(QString::number(0, 10, 0));

        order_close_btn_->setHidden(true);
        connect(btn3_, SIGNAL(clicked()),
            this, SLOT(show_account()));

        connect(q_set_btn_, SIGNAL(clicked()),
            this, SLOT(slot_open_q_dialog()));

        connect(operator_btn_, SIGNAL(clicked()),
            this, SLOT(slot_connnet_dialog()));

        connect(t_set_btn_, SIGNAL(clicked()),
            this, SLOT(slot_open_t_dialog()));

        connect(order_open_btn_, SIGNAL(clicked()),
            this, SLOT(slot_order_open()));

        connect(order_close_btn_, SIGNAL(clicked()),
            this, SLOT(slot_order_close()));

       
    }
}