#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "turingmachine.h"

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tb_origintape->setRowCount(2);
    ui->tb_worktape->setRowCount(2);
    ui->tb_resultTape->setRowCount(2);
    // 设置水平滚动条总是可见
    ui->tb_origintape->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->tb_origintape->verticalHeader()->setVisible(false);
    ui->tb_origintape->horizontalHeader()->setVisible(false);

    ui->tb_worktape->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->tb_worktape->verticalHeader()->setVisible(false);
    ui->tb_worktape->horizontalHeader()->setVisible(false);

    ui->tb_resultTape->verticalHeader()->setVisible(false);
    ui->tb_resultTape->horizontalHeader()->setVisible(false);

    ui->tb_origintape->setColumnCount(1);
    ui->tb_worktape->setColumnCount(1);
    ui->tb_resultTape->setColumnCount(1);

    QTableWidgetItem *item;
    item = new QTableWidgetItem("^");
    ui->tb_origintape->setItem(1, 0, item);
    item = new QTableWidgetItem("^");
    ui->tb_worktape->setItem(1, 0, item);
    item = new QTableWidgetItem("^");
    ui->tb_resultTape->setItem(1, 0, item);

    ui->le_originCurIdx->setText("0");
    ui->le_workCurIdx->setText("0");
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
输入初始纸带
初始化输入纸带
初始化状态
*/
void MainWindow::on_btn_input_clicked()
{
    ui->le_curState->setText("START");
    ui->le_nextState->setText("INIT");
    // 逐个获取输入数据
    QStringList items = ui->le_input->text().split(' ', Qt::SkipEmptyParts);
    if (ui->tb_origintape->columnCount() < items.size()) {
        // 如果列数小于分割后的字符串数量，增加列数
        ui->tb_origintape->setColumnCount(items.size());
    }
    if (ui->tb_origintape) {
        // 将分割后的字符串添加到表格中，每个字符串占据一列
        for (int i = 0; i < items.size(); ++i) {
            QTableWidgetItem *item = new QTableWidgetItem(items[i]);
            ui->tb_origintape->setItem(0, i, item);
        }
    }

    for(int i=3;i<items.size();++i){
        TuringMachine::getInstance().tapeElem[i-3] = items[i].toInt();
        qDebug() << TuringMachine::getInstance().tapeElem[i-3];
    }
    TuringMachine::getInstance().ElemSize = items.size()-3;
    TuringMachine::getInstance().originIdx=0;
    TuringMachine::getInstance().workIdx=0;
}

// 下一步
void MainWindow::on_btn_step_clicked()
{
    QString state = TuringMachine::getInstance().curState;
    if(state == "READ_LOW"){
        READ_LOW();
    }
    else if(state=="COMPARE_HIGH"){
        COMPARE_HIGH();
    }
    else if(state == "CALMID") {
        CALMID();
    }
    else if(state == "FINDMID") {
        FINDMID();
    }
    else if(state == "CMPTARGET") {
        CMPTARGET();
    }
    else if(state == "UpdateHigh") {
        UpdateHigh();
    }
    else if(state == "UpdateLow") {
        UpdateLow();
    }
    else if(state=="CALL"){
        CALL();
    }
    else if(state=="SUCCESS"){
        SUCCESS();
        TuringMachine::getInstance().halt = true;
        QMessageBox::information(this, "halt", "图灵机成功停机!");
    }
    else if(state=="RETURN"){
        RETURN();
    }
    else if(state=="STOP"){
        STOP();
    }
    ui->le_originCurIdx->setText(QString::number(TuringMachine::getInstance().originIdx));
    ui->le_workCurIdx->setText(QString::number(TuringMachine::getInstance().workIdx));
    ui->le_stepNum->setText(QString::number(++(TuringMachine::getInstance().steps)));
    ui->le_gridNum->setText(QString::number(TuringMachine::getInstance().gridNum));
}

// 一键执行
void MainWindow::on_btn_runAll_clicked()
{
    while(TuringMachine::getInstance().halt == false)
    {
        on_btn_step_clicked();
    }
}

// 初始化原始纸带：对原始序列进行排序
void MainWindow::on_btn_initTape_clicked()
{
    QTableWidgetItem *item;
    ui->le_curState->setText("READ_LOW");
    ui->le_nextState->setText("COMPARE_HIGH");
    TuringMachine::getInstance().sortTapeElem();
    for(int i=0;i<TuringMachine::getInstance().ElemSize;++i) {
        qDebug() << TuringMachine::getInstance().tapeElem[i];
        item = new QTableWidgetItem(QString::number(TuringMachine::getInstance().tapeElem[i]));
        ui->tb_origintape->setItem(0, i+3, item);
    }

    TuringMachine::getInstance().curState = "READ_LOW";
}
//读取下界
void MainWindow::READ_LOW(){
    if(TuringMachine::getInstance().originIdx!=0){
        TuringMachine::getInstance().originIdx--;
        ui->le_curState->setText("READ_LOW");
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx+1,new QTableWidgetItem(""));
    }
    else{
        ui->le_curState->setText("READ_LOW");
        TuringMachine::getInstance().low=ui->tb_origintape->item(0,0)->text().toInt();
        TuringMachine::getInstance().originIdx++;
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx-1,new QTableWidgetItem(""));
        ui->le_nextState->setText("COMPARE_HIGH");
        TuringMachine::getInstance().curState = "COMPARE_HIGH";
    }
}
//比较上下界大小
void MainWindow::COMPARE_HIGH(){
    ui->le_curState->setText("COMPARE_HIGH");
    TuringMachine::getInstance().high=ui->tb_origintape->item(0,1)->text().toInt();
    if(TuringMachine::getInstance().low<=TuringMachine::getInstance().high){
        ui->le_nextState->setText("CALMID");
        TuringMachine::getInstance().curState = "CALMID";
    }
    else{
        ui->le_nextState->setText("STOP");
        TuringMachine::getInstance().curState = "STOP";
    }
}


// 计算中位数位置
void MainWindow::CALMID()
{
    ui->le_curState->setText("CALMID");
    int leftIdx = ui->tb_origintape->item(0,0)->text().toInt();
    int rightIdx = ui->tb_origintape->item(0,1)->text().toInt();
    int midIdx = (leftIdx+rightIdx)/2;
    TuringMachine::getInstance().x=midIdx;
    ui->tb_worktape->setColumnCount(ui->tb_worktape->columnCount()+1);
    ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx++,new QTableWidgetItem(QString::number(midIdx)));
    ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
    ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem(""));
    QString str="low: " + QString::number(TuringMachine::getInstance().low) + " high: " + QString::number(TuringMachine::getInstance().high) + " mid: " + QString::number(TuringMachine::getInstance().x);
    TuringMachine::getInstance().stack.push_back(str);
    // 获取QTextEdit的文本文档
    QTextDocument *document = ui->textEdit->document();
    QTextCursor cursor(document);
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(str+"\n");
    ui->textEdit->ensureCursorVisible();
    TuringMachine::getInstance().gridNum++;
    ui->le_gridNum->setText(QString::number(TuringMachine::getInstance().gridNum));
    ui->le_nextState->setText("FINDMID");
    TuringMachine::getInstance().curState = "FINDMID";
}

// 查找原纸带的中位数
void MainWindow::FINDMID()
{
    ui->le_curState->setText("FINDMID");
    int mid = ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx-1)->text().toInt();
    if(TuringMachine::getInstance().originIdx-3 <mid)
    {
        TuringMachine::getInstance().originIdx++;
        ui->le_nextState->setText("FINDMID");
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx-1,new QTableWidgetItem(""));
    }
    else if(TuringMachine::getInstance().originIdx-3 >mid)
    {
        TuringMachine::getInstance().originIdx--;
        ui->le_nextState->setText("FINDMID");
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx+1,new QTableWidgetItem(""));
    }
    else
    {
        TuringMachine::getInstance().midValue=ui->tb_origintape->item(0,TuringMachine::getInstance().originIdx)->text().toInt();
        ui->le_nextState->setText("CMPTARGET");
        TuringMachine::getInstance().curState = "CMPTARGET";
    }
}

void MainWindow::CMPTARGET()
{
    ui->le_curState->setText("CMPTARGET");
    // 如果当前工作纸带指针没指向target(idx=2)
    if(TuringMachine::getInstance().originIdx != 2)
    {
        TuringMachine::getInstance().originIdx--;
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx+1,new QTableWidgetItem(""));
        return;
    }
    else
    {
        int target = ui->tb_origintape->item(0,TuringMachine::getInstance().originIdx)->text().toInt();
        if(TuringMachine::getInstance().midValue>target)
        {
            ui->le_nextState->setText("UpdateHigh");
            TuringMachine::getInstance().curState = "UpdateHigh";
        }
        else if(TuringMachine::getInstance().midValue < target)
        {
            ui->le_nextState->setText("UpdateLow");
            TuringMachine::getInstance().curState = "UpdateLow";
        }
        else
        {
            ui->le_nextState->setText("SUCCESS");
            TuringMachine::getInstance().curState = "SUCCESS";
        }
    }
}

void MainWindow::UpdateHigh()
{
    ui->le_curState->setText("UpdateHigh");
    if(TuringMachine::getInstance().originIdx == 1)
    {
        ui->tb_origintape->setItem(0,TuringMachine::getInstance().originIdx,new QTableWidgetItem(QString::number(TuringMachine::getInstance().x-1)));
        ui->le_nextState->setText("CALL");
        TuringMachine::getInstance().curState = "CALL";
    }
    else
    {
        ui->le_nextState->setText("UpdateHigh");
        TuringMachine::getInstance().originIdx--;
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx+1,new QTableWidgetItem(""));
    }
}

void MainWindow::UpdateLow()
{
    ui->le_curState->setText("UpdateLow");
    if(TuringMachine::getInstance().originIdx == 0)
    {
        ui->tb_origintape->setItem(0,TuringMachine::getInstance().originIdx,new QTableWidgetItem(QString::number(TuringMachine::getInstance().x+1)));
        ui->le_nextState->setText("CALL");
        TuringMachine::getInstance().curState = "CALL";
    }
    else
    {
        ui->le_nextState->setText("UpdateLow");
        TuringMachine::getInstance().originIdx--;
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx+1,new QTableWidgetItem(""));
    }
}
//递归调用
void MainWindow::CALL(){
    ui->le_nextState->setText("CALL");
    ui->le_nextState->setText("READ_LOW");
    TuringMachine::getInstance().curState = "READ_LOW";
}
//成功查找
void MainWindow::SUCCESS(){
    ui->le_curState->setText("SUCCESS");
    ui->tb_resultTape->setItem(0,0,new QTableWidgetItem(QString::number(TuringMachine::getInstance().x)));
    TuringMachine::getInstance().outIdx++;
    ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx,new QTableWidgetItem("^"));
    ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx+1,new QTableWidgetItem(""));
    ui->le_nextState->setText("halt");
}
void MainWindow::STOP(){
    ui->le_curState->setText("STOP");
    if(TuringMachine::getInstance().outIdx==0){
        ui->tb_resultTape->setItem(0,TuringMachine::getInstance().outIdx,new QTableWidgetItem("-1"));
        ui->tb_resultTape->setColumnCount(ui->tb_resultTape->columnCount()+1);
        TuringMachine::getInstance().outIdx++;
        ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx,new QTableWidgetItem("^"));
        ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx-1,new QTableWidgetItem(""));
    }
    ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx--,new QTableWidgetItem(""));
    ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
    ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
    TuringMachine::getInstance().gridNum--;
    TuringMachine::getInstance().stack.pop_back();
    QTextDocument *document = ui->textEdit->document();
    int totalLines = document->blockCount();
    if (totalLines > 0) {
        QTextCursor cursor(document);
        cursor.movePosition(QTextCursor::End);
        cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor);
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
    }
    ui->le_curState->setText("RETURN");
    TuringMachine::getInstance().curState = "RETURN";
}
void MainWindow::RETURN(){
    ui->le_curState->setText("RETURN");
    if(!TuringMachine::getInstance().stack.isEmpty()){
        ui->le_nextState->setText("STOP");
        TuringMachine::getInstance().curState = "STOP";
    }
    else{
        ui->le_nextState->setText("halt");
        TuringMachine::getInstance().halt = true;
        QMessageBox::information(this, "halt", "图灵机成功停机!");
    }
}





void MainWindow::on_comboBox_activated(int index)
{
    // 根据 comboBox 的当前索引设置 textEdit 的内容
    QString text;
    switch (index) {
    case 0:
        text = "BinarySearch(Iterative)\n"
               "请输入 需要搜索的数 搜索的起始下标 搜索的终止下标 搜索数序列\n"
               "e.g. 5 0 4 1 2 3 4 5\n"
               "在1~5序列中搜索5\n";
        break;
    case 1:
        text = "BinarySearch(Recursive)\n"
               "请输入 搜索的起始下标 搜索的终止下标 需要搜索的数 搜索数序列\n"
               "e.g. 0 9 5 0 1 2 3 4 6 7 8 9 10\n"
               "在0~10序列中搜索5\n"
               "当前递归栈：\n";
        break;
    case 2:
        text = "Dynamic Program";
        break;
    default:
        text = "...";
        break;
    }
    ui->textEdit->setText(text);
}

