#ifndef __FUTURE_PLATFORM_H__
#define __FUTURE_PLATFORM_H__

#ifdef WIN32
#ifndef _DEBUG
#define WIN32_LEAN_AND_MEAN
#endif
#endif
#include <QtWidgets/QDialog>
#include <string>
#include <vector>
#include <QtWidgets/QtWidgets>
#include "set_dialog.h"
#include "iTapTradeAPIDataType.h"
#include <QVector>
typedef struct
{
    std::string name;
    float value;
}StPrecision;

namespace future
{
    class CWndDlg : public QWidget
    {
    protected:
        void closeEvent(QCloseEvent* event);
    };

    //struct TapAPIFillInfo;
    //struct TapAPIOrderInfo;
    class trader_server;
    
    class future_platform : public QMainWindow
    {
        Q_OBJECT

    public:
        future_platform(QWidget *parent = 0);
        ~future_platform();
        void changeEvent(QEvent* event);
    private slots:
        void slot_close();
        void slot_open_q_dialog();
        void slot_open_t_dialog();
        void slot_connnet_dialog();
        void slot_order_open();
        void slot_order_close();
        void show_account();
        void transaction_account();
        int check_select(const QString& text);
        void slotActionCancel();
        void slotActionOpen();
        void slotActionStop();
        void buy_order();
        void sell_order();
        void tableContextMenuRequested(const QPoint& pos);
        void tableOpenMenuRequested(const QPoint& pos);
        void manual_create_mm_order();
        void manual_cancel_mm_order();
        void Cancel();
        void tableItemClicked(int row, int column);
        void show_order();
        void show_fill();
        void slot_write_log(QString str);
        void slot_quote_changed(QString last_price);
        void slot_state_changed(ITapTrade::TapAPIOrderInfo* _t1);
        void slot_close_position(QString commodity_no, QString contract_no);
        void slot_withdraw_order(QString order_no);
        void slot_quote_reconnect();
        void verify_code();
        void show_log();
        void slotLogout();
        void slot_add_trans(ITapTrade::TapAPIFillInfo *info);
        void logoutMenuRequested(const QPoint& pos);
        void slot_add_order(ITapTrade::TapAPIOrderInfo *info);
        void addstrategy();
        void savetrategy();
        void loadtrategy();
        void iconActived(QSystemTrayIcon::ActivationReason reason);
        void addbuytrategy();
        void addselltrategy();
        void show_stratery();
        void mousePressEvent(QMouseEvent *event)
        {
            this->windowPos = this->pos();           // 获得部件当前位置
            this->mousePos = event->globalPos();     // 获得鼠标位置
            this->dPos = mousePos - windowPos;       // 移动后部件所在的位置
        }

        void mouseMoveEvent(QMouseEvent *event)
        {
            this->move(event->globalPos() - this->dPos);
        }
    public:
        void start_server();
        std::string gettrategystr();
        int GetBidAndAskData(QString strData, double& bid, double& ask, double& combid, double& comask);
        void slot_update_stratery(int idx);
        void update_strategy_status_stopped(int idx);
        void ActionOpen(int row);
        void GetPreInfo();
        float getprevalue(QString name);
    public:
        int ismd_login;
        int istrade_login;
        int firstcreate;
        //Ui::future_platformClass ui;
        QWidget* top_wgt_;
        QLabel* title_label_;
        QPushButton* btn3_;
        QPushButton*btn_;
        QPushButton* btn1_;
        QPushButton* btn2_;
        QPushButton* strategy;
        QPushButton* strategylist;
        QPushButton* logbtn;
        QWidget* body_wgt_;
        //1 line
        QLabel* q_user_label_;
        QLineEdit* q_user_line_edit_;
        QPushButton* q_set_btn_;
        QWidget* w;
        QWidget* w1;
        QWidget* w2;
        QWidget* w_cancel_order;

        QWidget* wstrategy;

        //2 line
        QLabel* contract_label_;
        QLineEdit* contract_line_edit_;
        QPushButton* operator_btn_;
        QMenu* table_widget_menu;
        QMenu* logout_widget_menu;
        QMenu* stratery_widget_menu;
        //3 line
        QLabel* quote_label_;
        QLineEdit* quote_line_edit_;
        QLabel* quote_diff_label_;

        //4 line
        QLabel* t_user_label_;
        QLineEdit* t_user_line_edit_;
        QPushButton* t_set_btn_;

        QLineEdit* price_line_edit_;
        QLineEdit* num_line_edit_;

        QLineEdit* sell_price_line_edit_;
        QLineEdit* buy_price_line_edit_;
        QLineEdit* sell_num_line_edit_;
        QLineEdit* buy_num_line_edit_;
        QLineEdit* product_line_edit_;
        QLineEdit* verify_line_edit_;
        QLineEdit* sell_product_line_edit_;
        QLineEdit* hkex_mm_buy_orderno_edit_;
        QLineEdit* hkex_mm_sell_orderno_edit_;

        QComboBox* aselect_check_;
        QComboBox* sell_select_check_;

        QComboBox* account_check_;

        QComboBox* Hedgingaccount1;
        QComboBox* Hedgingaccount2;

        QComboBox* market_check_;
        QLineEdit* product_market_edit_;
        QLineEdit* sell_market_num_;
        QLineEdit* buy_market_num_;
        QLineEdit* adjustment;
        QLineEdit* bidspread;
        QLineEdit* askspread;

        QLineEdit* radio1;
        QLineEdit* radio2;

        QLineEdit* radio3;
        QLineEdit* radio4;

        QLineEdit* Strategyname;

         QLineEdit* stoptime;

        QLineEdit* bidspread1;
        QLineEdit* askspread1;

        QLineEdit* threshold;
        QLineEdit* threshhold1;
        
        QLineEdit *hedginginstrument1;
        QLineEdit* hedginginstrument2;

        QCheckBox* AutoHedge;

        QLineEdit* slippage1;
        QLineEdit* slippage2;

        QLineEdit*  Targetcontract1;
        QLineEdit* Targetcontract2;

        QGridLayout *sell_gLayout;
        QComboBox* sell_type_check1_;

        //5 line
        QLabel* dot_label_;
        QLineEdit* dot_line_edit_;
        QPushButton* order_open_btn_;

        //6 line
        QLabel* deal_price_label_;
        QLineEdit* deal_price_line_edit_;
        QPushButton* order_close_btn_;

        QWidget* bottom_wgt_;
        QLabel* log_label_;
        QTextBrowser* log_text_edit_;

        //set_dialog
        set_dialog q_set_dialog_;
        set_dialog t_set_dialog_;

        QTableWidget* logtableWidget;
        QTableWidget* tableWidget;
        QTableWidget *filltableWidget;
        QTableWidget *staterytableWidget;

        QTableWidget* ordertableWidget;
        QDesktopWidget* m_pDeskdop;
        QPoint windowPos;
        QPoint mousePos;
        QPoint dPos;
        QWidget* newwin;
        QVector <QString> m_vec_account;
        trader_server *tradesvr1;
        QMap <QString, QString> mapAccount;
        int m_row;
        CWndDlg* accwin;
        CWndDlg* logwin;
        int m_sellorder;
        int m_buyorder;
        QWidget* fillwin;
        QWidget *staterylist;
        QAction* logoutaction;
        QAction* action;

        QAction* strateryaction;
        QAction *stopaction;
        int m_table_row;
        int m_fill_row;
        int m_order_row;
        
        QVector <StPrecision> m_vecPre;
    public:
        void init();

    };
}
#endif // FUTURE_PLATFORM_H
