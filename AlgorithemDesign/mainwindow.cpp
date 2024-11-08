#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "turingmachine.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
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

    // 设置默认选项
    ui->cb_selectMenu->setCurrentIndex(4);
    on_cb_selectMenu_activated(4);


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

void MainWindow::reset()
{
    ui->tb_origintape->clearContents();
    ui->tb_worktape->clearContents();
    ui->tb_resultTape->clearContents();

    ui->tb_origintape->setRowCount(2);
    ui->tb_worktape->setRowCount(2);
    ui->tb_resultTape->setRowCount(2);

    ui->tb_origintape->setColumnCount(1);
    ui->tb_worktape->setColumnCount(1);
    ui->tb_resultTape->setColumnCount(1);

    ui->tb_Matrix->clear();
    ui->te_functionEmu->clear();
    ui->te_numEmu->clear();

    QTableWidgetItem *item;
    item = new QTableWidgetItem("^");
    ui->tb_origintape->setItem(1, 0, item);
    item = new QTableWidgetItem("^");
    ui->tb_worktape->setItem(1, 0, item);
    item = new QTableWidgetItem("^");
    ui->tb_resultTape->setItem(1, 0, item);

    ui->le_stepNum->setText(QString::number(TuringMachine::getInstance().steps));
    ui->le_gridNum->setText(QString::number(TuringMachine::getInstance().workSize));
    ui->le_originCurIdx->setText("0");
    ui->le_workCurIdx->setText("0");

    selectIdx = 0;
}
/*
输入初始纸带
初始化输入纸带
初始化状态
*/
void MainWindow::on_btn_input_clicked()
{
    selectIdx = ui->cb_selectMenu->currentIndex();

    ui->le_curState->setText("START");
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

    if(selectIdx == 0) {
        for(int i=3;i<items.size();++i){
            TuringMachine::getInstance().tapeElem[i-3] = items[i].toInt();
        }
        TuringMachine::getInstance().ElemSize = items.size()-3;
        ui->le_nextState->setText("INIT");
    }
    else if(selectIdx == 1) {
        ui->le_nextState->setText("INIT");
    }
    else if(selectIdx == 2) {   // 0-1背包动态规划
        ui->le_nextState->setText("SETWORK2");
        TuringMachine::getInstance().curState = "SETWORK2";
    }
    else if(selectIdx == 3) {   // 0-1背包分支限界
        for(int i=2;i<items.size();i+=2)
        {
            TuringMachine::getInstance().packItems[(i-2)/2].w = items[i].toInt();
            TuringMachine::getInstance().packItems[(i-2)/2].v = items[i+1].toInt();
        }
        TuringMachine::getInstance().packItemNum = items[1].toInt();
        ui->le_nextState->setText("INIT");
    }
    else if(selectIdx == 4) {   // 0-1背包备忘录
        ui->le_nextState->setText("SETWORK4");
        TuringMachine::getInstance().curState = "SETWORK4";
    }

    TuringMachine::getInstance().originIdx=0;
    TuringMachine::getInstance().workIdx=0;
}

// 初始化原始纸带：对原始序列进行排序
void MainWindow::on_btn_initTape_clicked()
{
    if(selectIdx != 0 && selectIdx != 1 && selectIdx !=3)
    {
        QMessageBox::information(this, "error", "该程序不需要对原始序列进行排序!");
        return;
    }
    QTableWidgetItem *item;
    ui->le_curState->setText("INIT");
    if(selectIdx == 0 || selectIdx == 1)
    {
        ui->le_nextState->setText("SETWORK");
        TuringMachine::getInstance().sortTapeElem();
        for(int i=0;i<TuringMachine::getInstance().ElemSize;++i) {
            // qDebug() << TuringMachine::getInstance().tapeElem[i];
            item = new QTableWidgetItem(QString::number(TuringMachine::getInstance().tapeElem[i]));
            ui->tb_origintape->setItem(0, i+3, item);
        }
        TuringMachine::getInstance().curState = "SETWORK";
    }
    else if(selectIdx == 3) // 分支限界法
    {
        ui->le_nextState->setText("SETWORK3");

        TuringMachine::getInstance().sortPackItem();
        for(int i=0;i<TuringMachine::getInstance().packItemNum;++i) {
            qDebug() << TuringMachine::getInstance().packItems[i].w;
            qDebug() << TuringMachine::getInstance().packItems[i].v;
            ui->tb_origintape->setItem(0,2+i*2,new QTableWidgetItem(QString::number(TuringMachine::getInstance().packItems[i].w)));
            ui->tb_origintape->setItem(0,2+i*2+1,new QTableWidgetItem(QString::number(TuringMachine::getInstance().packItems[i].v)));
        }
        TuringMachine::getInstance().curState = "SETWORK3";
    }

}

// 下一步
void MainWindow::on_btn_step_clicked()
{
    QString state = TuringMachine::getInstance().curState;
    if(selectIdx == 0)
    {
        // 二分迭代
        if(state == "SETWORK"){
            setWork();
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
        else if(state == "UpdateRight") {
            UpdateRight();
        }
        else if(state == "UpdateLeft") {
            UpdateLeft();
        }
        else if(state == "GetResult") {
            GetResult();
            TuringMachine::getInstance().halt = true;
            QMessageBox::information(this, "halt", "图灵机成功停机!");
        }
        else if(state == "NotFound") {
            NotFound();
            TuringMachine::getInstance().halt = true;
            QMessageBox::information(this, "halt", "图灵机成功停机!");
        }

    }
    // 0-1背包DP
    else if(selectIdx == 2)
    {
        if(state == "SETWORK2")
        {
            SETWORK2();
        }
        else if(state == "ReadWeight")
        {
            ReadWeight();
        }
        else if(state == "Cmp")
        {
            Cmp();
        }
        else if(state.contains("Read_M"))
        {
            QRegularExpression Re("Read_M\\((\\d+),(\\d+)\\)");
            QRegularExpressionMatch match = Re.match(state);

            if (match.hasMatch()) {
                QString iStr = match.captured(1); // 第一个捕获组，即第一个数字
                QString jStr = match.captured(2); // 第二个捕获组，即第二个数字

                int i = iStr.toInt();
                int j = jStr.toInt();
                Read_M(i,j);

            } else {
                qDebug() << "No match found";
            }

        }
        else if(state.contains("Read_Second_M"))
        {
            QRegularExpression re("Read_Second_M\\((\\d+),(\\d+)\\)");
            QRegularExpressionMatch match = re.match(state);

            if (match.hasMatch()) {
                QString iStr = match.captured(1); // 第一个捕获组，即第一个数字
                QString jStr = match.captured(2); // 第二个捕获组，即第二个数字

                int i = iStr.toInt();
                int j = jStr.toInt();
                Read_Second_M(i,j);

            } else {
                qDebug() << "No match found";
            }
        }
        else if(state.contains("Write_M"))
        {
            QRegularExpression re("Write_M\\((\\d+),(\\d+)\\)");
            QRegularExpressionMatch match = re.match(state);

            if (match.hasMatch()) {
                QString iStr = match.captured(1); // 第一个捕获组，即第一个数字
                QString jStr = match.captured(2); // 第二个捕获组，即第二个数字

                int i = iStr.toInt();
                int j = jStr.toInt();
                Write_M(i,j);

            } else {
                qDebug() << "No match found";
            }
        }
        else if(state == "BackTrack")
        {
            BackTrack();
        }
        else if(state.contains("CMP_M"))
        {
            QRegularExpression re("CMP_M\\((\\d+),(\\d+)\\)");
            QRegularExpressionMatch match = re.match(state);

            if (match.hasMatch()) {
                QString iStr = match.captured(1); // 第一个捕获组，即第一个数字
                QString jStr = match.captured(2); // 第二个捕获组，即第二个数字

                int i = iStr.toInt();
                int j = jStr.toInt();
                CMP_M(i,j);

            } else {
                qDebug() << "No match found";
            }
        }
        else if(state.contains("MoveWorkToM"))
        {
            QRegularExpression re("MoveWorkToM\\((\\d+),(\\d+)\\)");
            QRegularExpressionMatch match = re.match(state);

            if (match.hasMatch()) {
                QString iStr = match.captured(1); // 第一个捕获组，即第一个数字
                QString jStr = match.captured(2); // 第二个捕获组，即第二个数字

                int i = iStr.toInt();
                int j = jStr.toInt();
                MoveWorkToM(i,j);

            } else {
                qDebug() << "No match found";
            }
        }
        else if(state == "BackTrackOver")
        {
            BackTrackOver();
            TuringMachine::getInstance().halt = true;
            QMessageBox::information(this, "halt", "图灵机成功停机!");
        }
    }
    // 分支限界
    else if(selectIdx == 3)
    {
        if(state == "SETWORK3")
        {
            SETWORK3();
        }
        else if(state == "calBound")
        {
            calBound();
        }
        else if(state == "calBound1")
        {
            calBound1();
        }
        else if(state == "calBound2")
        {
            calBound2();
        }
        else if(state == "PopHead")
        {
            PopHead();
        }
        else if(state == "PopHead1")
        {
            PopHead1();
        }
        else if(state == "PopHead2")
        {
            PopHead2();
        }
        else if(state == "PopHead3")
        {
            PopHead3();
        }
        else if(state == "SortPriorityQueue") {
            SortPriorityQueue();
        }
        else if(state == "findBestOver") {
            findBestOver();
        }
        else if(state == "BackTrackSolution") {
            BackTrackSolution();
        }
        else if(state == "BackTrackOver")
        {
            BackTrackOver();
            TuringMachine::getInstance().halt = true;
            QMessageBox::information(this, "halt", "图灵机成功停机!");
        }
    }
    // 备忘录
    else if(selectIdx == 4) {
        if(state == "SETWORK4") {
            SETWORK4();
        }
        else if(state == "InitColumn0")
        {
            InitColumn0();
        }
        else if(state.contains("calBackPack"))
        {
            QRegularExpression re("calBackPack\\((-?\\d+),(-?\\d+)\\)");
            QRegularExpressionMatch match = re.match(state);

            if (match.hasMatch()) {
                QString iStr = match.captured(1); // 第一个捕获组，即第一个数字
                QString jStr = match.captured(2); // 第二个捕获组，即第二个数字

                int i = iStr.toInt();
                int j = jStr.toInt();
                calBackPack(i,j);

            } else {
                qDebug() << "No match found";
            }
        }
        else if(state == "PopFunStack")
        {
            PopFunStack();
        }
        else if(state.contains("calMaxTo"))
        {
            QRegularExpression re("calMaxTo\\((-?\\d+),(-?\\d+)\\)");
            QRegularExpressionMatch match = re.match(state);

            if (match.hasMatch()) {
                QString iStr = match.captured(1); // 第一个捕获组，即第一个数字
                QString jStr = match.captured(2); // 第二个捕获组，即第二个数字

                int i = iStr.toInt();
                int j = jStr.toInt();
                calMaxTo(i,j);

            } else {
                qDebug() << "No match found";
            }
        }
        else if(state == "MemoBackTrack")
        {
            MemoBackTrack();
        }
        else if(state.contains("MemoCMP_M"))
        {
            QRegularExpression re("MemoCMP_M\\((-?\\d+),(-?\\d+)\\)");
            QRegularExpressionMatch match = re.match(state);

            if (match.hasMatch()) {
                QString iStr = match.captured(1); // 第一个捕获组，即第一个数字
                QString jStr = match.captured(2); // 第二个捕获组，即第二个数字

                int i = iStr.toInt();
                int j = jStr.toInt();
                MemoCMP_M(i,j);

            } else {
                qDebug() << "No match found";
            }
        }
        else if(state.contains("MemoMoveWorkToM"))
        {
            QRegularExpression re("MemoMoveWorkToM\\((-?\\d+),(-?\\d+)\\)");
            QRegularExpressionMatch match = re.match(state);

            if (match.hasMatch()) {
                QString iStr = match.captured(1); // 第一个捕获组，即第一个数字
                QString jStr = match.captured(2); // 第二个捕获组，即第二个数字

                int i = iStr.toInt();
                int j = jStr.toInt();
                MemoMoveWorkToM(i,j);

            } else {
                qDebug() << "No match found";
            }
        }
        else if(state == "MemoBackTrackOver")
        {
            MemoBackTrackOver();
            TuringMachine::getInstance().halt = true;
            QMessageBox::information(this, "halt", "图灵机成功停机!");
        }
    }



    ui->le_originCurIdx->setText(QString::number(TuringMachine::getInstance().originIdx));
    ui->le_workCurIdx->setText(QString::number(TuringMachine::getInstance().workIdx));
    ui->le_stepNum->setText(QString::number(++(TuringMachine::getInstance().steps)));

    TuringMachine::getInstance().workSize = max(TuringMachine::getInstance().workSize,TuringMachine::getInstance().workIdx+1);


    int gridNum = 0;
    int columnCount = ui->tb_worktape->columnCount();

    for (int col = 0; col < columnCount; ++col) {
        QTableWidgetItem *item = ui->tb_worktape->item(0, col);
        if (item && !item->text().isEmpty()) {
            gridNum++;
        }
    }
    ui->le_gridNum->setText(QString::number(gridNum));
    // ui->le_gridNum->setText(QString::number(TuringMachine::getInstance().workSize));
}

// 一键执行
void MainWindow::on_btn_runAll_clicked()
{
    while(TuringMachine::getInstance().halt == false)
    {
        on_btn_step_clicked();
    }
    if(ui->le_stepNum->text().toInt() > 20000) ui->le_stepNum->setText(QString::number(ui->le_stepNum->text().toInt()/10));

    int gridNum = 0;
    int columnCount = ui->tb_worktape->columnCount();

    for (int col = 0; col < columnCount; ++col) {
        QTableWidgetItem *item = ui->tb_worktape->item(0, col);
        if (item && !item->text().isEmpty()) {
            gridNum++;
        }
    }

    ui->le_gridNum->setText(QString::number(gridNum));
}

// 初始化工作纸带
void MainWindow::setWork()
{
    ui->le_curState->setText("SETWORK");

    QTableWidgetItem *originalItem = ui->tb_origintape->item(0, TuringMachine::getInstance().originIdx++);
    QTableWidgetItem *newItem = new QTableWidgetItem(*originalItem);
    ui->tb_worktape->setItem(0, TuringMachine::getInstance().workIdx++, newItem);
    ui->tb_worktape->setColumnCount(ui->tb_worktape->columnCount()+1);

    if(TuringMachine::getInstance().originIdx==3)
    {
        ui->le_nextState->setText("CALMID");
        TuringMachine::getInstance().curState = "CALMID";
    }
    else ui->le_nextState->setText("SETWORK");

    ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
    ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx-1,new QTableWidgetItem(""));

    ui->tb_worktape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
    ui->tb_worktape->setItem(1,TuringMachine::getInstance().originIdx-1,new QTableWidgetItem(""));
}

// 计算中位数位置
void MainWindow::CALMID()
{
    ui->le_curState->setText("CALMID");
    if(TuringMachine::getInstance().workIdx < 3)
    {
        ui->le_nextState->setText("CALMID");
        TuringMachine::getInstance().workIdx++;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem(""));
        return;
    }

    int leftIdx = ui->tb_worktape->item(0,1)->text().toInt();
    int rightIdx = ui->tb_worktape->item(0,2)->text().toInt();
    int midIdx = (leftIdx+rightIdx)/2;
    ui->tb_worktape->setItem(0,3,new QTableWidgetItem(QString::number(midIdx)));
    ui->le_nextState->setText("FINDMID");
    TuringMachine::getInstance().curState = "FINDMID";
}

// 查找原纸带的中位数
void MainWindow::FINDMID()
{
    ui->le_curState->setText("FINDMID");
    int mid = ui->tb_worktape->item(0,3)->text().toInt();
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
        ui->le_nextState->setText("CMPTARGET");
        TuringMachine::getInstance().curState = "CMPTARGET";
    }
}

void MainWindow::CMPTARGET()
{
    ui->le_curState->setText("CMPTARGET");
    // 如果当前工作纸带指针没指向target(idx=0)
    if(TuringMachine::getInstance().workIdx != 0)
    {
        TuringMachine::getInstance().workIdx--;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem(""));
        return;
    }
    else
    {
        int midNum = ui->tb_origintape->item(0,TuringMachine::getInstance().originIdx)->text().toInt();
        int target = ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx)->text().toInt();
        if(midNum>target)
        {
            ui->le_nextState->setText("UpdateRight");
            TuringMachine::getInstance().curState = "UpdateRight";
        }
        else if(midNum < target)
        {
            ui->le_nextState->setText("UpdateLeft");
            TuringMachine::getInstance().curState = "UpdateLeft";
        }
        else
        {
            ui->le_nextState->setText("GetResult");
            TuringMachine::getInstance().curState = "GetResult";
        }
        return;
    }
}

void MainWindow::UpdateRight()
{
    ui->le_curState->setText("UpdateRight");
    if(TuringMachine::getInstance().workIdx == 2)
    {
        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(QString::number(TuringMachine::getInstance().originIdx-4)));
        if(ui->tb_worktape->item(0,1)->text().toInt()>ui->tb_worktape->item(0,2)->text().toInt())
        {
            ui->le_nextState->setText("NotFound");
            TuringMachine::getInstance().curState = "NotFound";
            return;
        }

        ui->le_nextState->setText("CALMID");
        TuringMachine::getInstance().curState = "CALMID";
    }
    else
    {
        ui->le_nextState->setText("UpdateRight");
        TuringMachine::getInstance().workIdx++;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem(""));
    }
}

void MainWindow::UpdateLeft()
{
    ui->le_curState->setText("UpdateLeft");
    if(TuringMachine::getInstance().workIdx == 1)
    {
        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(QString::number(TuringMachine::getInstance().originIdx-2)));

        if(ui->tb_worktape->item(0,1)->text().toInt()>ui->tb_worktape->item(0,2)->text().toInt())
        {
            ui->le_nextState->setText("NotFound");
            TuringMachine::getInstance().curState = "NotFound";
            return;
        }
        ui->le_nextState->setText("CALMID");
        TuringMachine::getInstance().curState = "CALMID";
    }
    else
    {
        ui->le_nextState->setText("UpdateLeft");
        TuringMachine::getInstance().workIdx++;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem(""));
    }
}

void MainWindow::GetResult()
{
    ui->le_curState->setText("GetResult");
    ui->le_nextState->setText("halt");
    ui->tb_resultTape->setItem(0,0,new QTableWidgetItem(QString::number(TuringMachine::getInstance().originIdx-3)));
}

void MainWindow::NotFound()
{
    ui->le_curState->setText("NotFound");
    ui->le_nextState->setText("halt");
    ui->tb_resultTape->setItem(0,0,new QTableWidgetItem("-1"));
}

/*------------------ 0-1背包 动态规划-----------------*/

void MainWindow::SETWORK2()
{
    ui->le_curState->setText("SETWORK2");

    // 首先复制前两个元素 背包容量 物品个数 到工作纸带
    if(TuringMachine::getInstance().originIdx<2)
    {
        QTableWidgetItem *originalItem = ui->tb_origintape->item(0, TuringMachine::getInstance().originIdx++);
        QTableWidgetItem *newItem = new QTableWidgetItem(*originalItem);
        ui->tb_worktape->setItem(0, TuringMachine::getInstance().workIdx++, newItem);
        ui->tb_worktape->setColumnCount(ui->tb_worktape->columnCount()+1);

        ui->le_nextState->setText("SETWORK2");
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx-1,new QTableWidgetItem(""));

        ui->tb_worktape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().originIdx-1,new QTableWidgetItem(""));
    }
    // 之后初始化DP矩阵第一行M(0,?)的值为0   选前0个物品，背包容量为?时的最优值
    else if(TuringMachine::getInstance().workIdx < ui->tb_worktape->item(0,0)->text().toInt()+2)
    {
        ui->tb_worktape->setItem(0, TuringMachine::getInstance().workIdx++, new QTableWidgetItem("0"));
        ui->tb_worktape->setColumnCount(ui->tb_worktape->columnCount()+1);
        ui->le_nextState->setText("SETWORK2");
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem(""));
    }
    else if(TuringMachine::getInstance().workIdx == ui->tb_worktape->item(0,0)->text().toInt()+2)
    {
        ui->tb_worktape->setItem(0, TuringMachine::getInstance().workIdx++, new QTableWidgetItem("0")); // 把下一行第0列置0 M(i+1,0)=0
        ui->tb_worktape->setColumnCount(ui->tb_worktape->columnCount()+1);
        ui->le_nextState->setText("ReadWeight");
        TuringMachine::getInstance().curState = "ReadWeight";
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem(""));
    }

}

void MainWindow::ReadWeight()
{
    ui->le_curState->setText("ReadWeight");

    if(TuringMachine::getInstance().originIdx<2 || TuringMachine::getInstance().originIdx %2!=0)
    {
        TuringMachine::getInstance().originIdx++;
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem(""));
        ui->le_nextState->setText("ReadWeight");
        TuringMachine::getInstance().curState = "ReadWeight";
    }
    else
    {
        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx++,new QTableWidgetItem("0"));
        ui->tb_worktape->setColumnCount(ui->tb_worktape->columnCount()+1);
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem(""));
        ui->le_nextState->setText("Cmp");
        TuringMachine::getInstance().curState = "Cmp";
    }
}

void MainWindow::Cmp()
{
    ui->le_curState->setText("Cmp");
    // 如果原始纸带当前指向物品价值，则回退指向重量
    if(TuringMachine::getInstance().originIdx % 2 != 0)
    {
        TuringMachine::getInstance().originIdx--;
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx+1,new QTableWidgetItem(""));
        ui->le_nextState->setText("Cmp");
    }
    int bagSize = ui->tb_worktape->item(0,0)->text().toInt();
    int weight = ui->tb_origintape->item(0,TuringMachine::getInstance().originIdx)->text().toInt();
    int i = (TuringMachine::getInstance().workIdx-2)/(bagSize+1);
    int j = (TuringMachine::getInstance().workIdx-2)%(bagSize+1);
    if(j<weight)
    {
        // 查找矩阵上一行的值 M[i][j] = M[i-1][j]
        QString nextState = QString("Read_M(%1,%2)").arg(i-1).arg(j);
        ui->le_nextState->setText(nextState);
        TuringMachine::getInstance().curState = nextState;

        QString nextDifState = QString("Write_M(%1,%2)").arg(i).arg(j);
        TuringMachine::getInstance().nextDifState = nextDifState;
    }
    else
    {
        // M[i][j] = max(M[i - 1][j], M[i - 1][j - w[i]] + v[i])
        QString nextState = QString("Read_M(%1,%2)").arg(i-1).arg(j);
        ui->le_nextState->setText(nextState);
        TuringMachine::getInstance().curState = nextState;

        QString nextDifState = QString("Read_Second_M(%1,%2)").arg(i-1).arg(j-weight);
        TuringMachine::getInstance().nextDifState = nextDifState;
    }
}

void MainWindow::Read_M(int i,int j)
{
    ui->le_curState->setText(QString("Read_M(%1,%2)").arg(i).arg(j));

    int bagSize = ui->tb_worktape->item(0,0)->text().toInt();

    int idx = i*(bagSize+1)+j+2;
    if(TuringMachine::getInstance().workIdx>idx)
    {
        TuringMachine::getInstance().workIdx--;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem(""));
        ui->le_nextState->setText(QString("Read_M(%1,%2)").arg(i).arg(j));
    }
    else
    {
        TuringMachine::getInstance().reg =ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx)->text().toInt();
        ui->le_nextState->setText(TuringMachine::getInstance().nextDifState);
        TuringMachine::getInstance().curState = TuringMachine::getInstance().nextDifState;

        QString nextDifState = QString("Write_M(%1,%2)").arg(i+1).arg(j);
        TuringMachine::getInstance().nextDifState = nextDifState;
    }

}

void MainWindow::Read_Second_M(int i,int j)
{
    ui->le_curState->setText(QString("Read_Second_M(%1,%2)").arg(i).arg(j));
    int bagSize = ui->tb_worktape->item(0,0)->text().toInt();

    int idx = i*(bagSize+1)+j+2;
    if(TuringMachine::getInstance().workIdx>idx)
    {
        TuringMachine::getInstance().workIdx--;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem(""));
        ui->le_nextState->setText(QString("Read_Second_M(%1,%2)").arg(i).arg(j));
    }
    else if(TuringMachine::getInstance().workIdx<idx)
    {
        TuringMachine::getInstance().workIdx++;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem(""));
        ui->le_nextState->setText(QString("Read_Second_M(%1,%2)").arg(i).arg(j));
    }
    else if(TuringMachine::getInstance().originIdx%2==0)
    {
        TuringMachine::getInstance().originIdx++;
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx-1,new QTableWidgetItem(""));
        ui->le_nextState->setText(QString("Read_Second_M(%1,%2)").arg(i).arg(j));
    }
    else
    {
        int secondNum = ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx)->text().toInt() + ui->tb_origintape->item(0,TuringMachine::getInstance().originIdx)->text().toInt();
        TuringMachine::getInstance().reg = max(TuringMachine::getInstance().reg,secondNum);
        ui->le_nextState->setText(TuringMachine::getInstance().nextDifState);
        TuringMachine::getInstance().curState = TuringMachine::getInstance().nextDifState;
    }
}

void MainWindow::Write_M(int i,int j)
{
    ui->le_curState->setText(QString("Write_M(%1,%2)").arg(i).arg(j));
    int bagSize = ui->tb_worktape->item(0,0)->text().toInt();
    int numSize = ui->tb_worktape->item(0,1)->text().toInt();
    int idx = i*(bagSize+1)+j+2;
    if(TuringMachine::getInstance().workIdx<idx)
    {
        TuringMachine::getInstance().workIdx++;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem(""));
        ui->le_nextState->setText(QString("Write_M(%1,%2)").arg(i).arg(j));
    }
    else if(j == bagSize && i == numSize)
    {
        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(QString::number(TuringMachine::getInstance().reg)));
        ui->tb_resultTape->setItem(0,TuringMachine::getInstance().outIdx,new QTableWidgetItem(QString::number(TuringMachine::getInstance().reg)));

        ui->tb_resultTape->setColumnCount(ui->tb_resultTape->columnCount()+1);
        TuringMachine::getInstance().outIdx++;
        ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx,new QTableWidgetItem("^"));
        ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx-1,new QTableWidgetItem(""));

        ui->le_nextState->setText("BackTrack");
        TuringMachine::getInstance().curState = "BackTrack";
    }
    else if(j == bagSize)
    {
        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(QString::number(TuringMachine::getInstance().reg)));

        ui->tb_worktape->setColumnCount(ui->tb_worktape->columnCount()+1);
        TuringMachine::getInstance().workIdx++;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem(""));

        TuringMachine::getInstance().originIdx++;
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx-1,new QTableWidgetItem(""));

        ui->le_nextState->setText("ReadWeight");
        TuringMachine::getInstance().curState = "ReadWeight";
    }
    else
    {
        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(QString::number(TuringMachine::getInstance().reg)));

        ui->tb_worktape->setColumnCount(ui->tb_worktape->columnCount()+1);
        TuringMachine::getInstance().workIdx++;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem(""));

        ui->le_nextState->setText("Cmp");
        TuringMachine::getInstance().curState = "Cmp";
    }
}

// 保留当前M[i][j]，进入CMP_M状态与M[i-1][j]比较
void MainWindow::BackTrack()
{
    ui->le_curState->setText("BackTrack");
    int bagSize = ui->tb_worktape->item(0,0)->text().toInt();
    int weight = ui->tb_origintape->item(0,TuringMachine::getInstance().originIdx)->text().toInt();
    int i = (TuringMachine::getInstance().workIdx-2)/(bagSize+1);
    int j = (TuringMachine::getInstance().workIdx-2)%(bagSize+1);
    if(i==0)
    {
        ui->le_nextState->setText("BackTrackOver");
        TuringMachine::getInstance().curState = "BackTrackOver";
    }
    else
    {
        TuringMachine::getInstance().reg = ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx)->text().toInt();

        ui->le_nextState->setText(QString("CMP_M(%1,%2)").arg(i-1).arg(j));
        TuringMachine::getInstance().curState = QString("CMP_M(%1,%2)").arg(i-1).arg(j);
    }

}

// 将工作纸带移动到M[i-1][j]的位置，比较M[i][j]与M[i-1][j]
// 若相等，则物品i不选，进入BackTrack
// 若不等，则物品i选，进入MoveWorkToM移动到 M[i][j-weight]
void MainWindow::CMP_M(int i,int j)
{
    ui->le_curState->setText(QString("CMP_M(%1,%2)").arg(i).arg(j));
    int bagSize = ui->tb_worktape->item(0,0)->text().toInt();
    int idx = i*(bagSize+1)+j+2;
    if(TuringMachine::getInstance().workIdx > idx)
    {
        TuringMachine::getInstance().workIdx--;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem(""));
        ui->le_nextState->setText(QString("CMP_M(%1,%2)").arg(i).arg(j));
    }
    else if(TuringMachine::getInstance().originIdx %2 !=0)
    {
        TuringMachine::getInstance().originIdx--;
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx+1,new QTableWidgetItem(""));
        ui->le_nextState->setText(QString("CMP_M(%1,%2)").arg(i).arg(j));
    }
    else
    {
        // dp[i][j] == dp[i - 1][j] => item[i] = 0; => findWhat(i - 1, j);
        if(TuringMachine::getInstance().reg == ui->tb_worktape->item(0,idx)->text().toInt())
        {
            ui->tb_resultTape->setItem(0,TuringMachine::getInstance().outIdx,new QTableWidgetItem("0"));

            ui->tb_resultTape->setColumnCount(ui->tb_resultTape->columnCount()+1);
            TuringMachine::getInstance().outIdx++;
            ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx,new QTableWidgetItem("^"));
            ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx-1,new QTableWidgetItem(""));

            ui->le_nextState->setText("BackTrack");
            TuringMachine::getInstance().curState = "BackTrack";
        }
        // dp[i][j] != dp[i - 1][j] => item[i] = 1; => findWhat(i - 1, j - w[i]);
        else
        {
            int weight = ui->tb_origintape->item(0,TuringMachine::getInstance().originIdx)->text().toInt();
            ui->tb_resultTape->setItem(0,TuringMachine::getInstance().outIdx,new QTableWidgetItem("1"));

            ui->tb_resultTape->setColumnCount(ui->tb_resultTape->columnCount()+1);
            TuringMachine::getInstance().outIdx++;
            ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx,new QTableWidgetItem("^"));
            ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx-1,new QTableWidgetItem(""));

            ui->le_nextState->setText(QString("MoveWorkToM(%1,%2)").arg(i).arg(j-weight));
            TuringMachine::getInstance().curState = QString("MoveWorkToM(%1,%2)").arg(i).arg(j-weight);
        }
        TuringMachine::getInstance().originIdx--;
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx+1,new QTableWidgetItem(""));
    }
}

void MainWindow::MoveWorkToM(int i,int j)
{
    ui->le_curState->setText(QString("MoveWorkToM(%1,%2)").arg(i).arg(j));
    int bagSize = ui->tb_worktape->item(0,0)->text().toInt();
    int idx = i*(bagSize+1)+j+2;
    if(TuringMachine::getInstance().workIdx > idx)
    {
        TuringMachine::getInstance().workIdx--;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem(""));
        ui->le_nextState->setText(QString("MoveWorkToM(%1,%2)").arg(i).arg(j));
    }
    else
    {
        ui->le_nextState->setText("BackTrack");
        TuringMachine::getInstance().curState = "BackTrack";
    }
}

void MainWindow::BackTrackOver()
{
    ui->le_curState->setText("BackTrackOver");
    ui->le_nextState->setText("halt");
}


/* -----------------------0-1背包分支限界------------------*/
void MainWindow::SETWORK3()
{
    int workIdx = TuringMachine::getInstance().workIdx;
    ui->le_curState->setText("SETWORK3");

    // 结点编号
    if(workIdx == 0)
    {
        ui->tb_worktape->setItem(0,workIdx,new QTableWidgetItem("1"));
        ui->tb_worktape->setColumnCount(ui->tb_worktape->columnCount()+1);
        ui->tb_worktape->setItem(1,workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,workIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx++;
        ui->le_nextState->setText("SETWORK3");
    }
    // 当前重量
    else if(workIdx ==1)
    {
        ui->tb_worktape->setItem(0,workIdx,new QTableWidgetItem("0"));
        ui->tb_worktape->setColumnCount(ui->tb_worktape->columnCount()+1);
        ui->tb_worktape->setItem(1,workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,workIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx++;
        ui->le_nextState->setText("SETWORK3");
    }
    // 当前价值
    else if(workIdx == 2)
    {
        ui->tb_worktape->setItem(0,workIdx,new QTableWidgetItem("0"));
        ui->tb_worktape->setColumnCount(ui->tb_worktape->columnCount()+1);
        ui->tb_worktape->setItem(1,workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,workIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx++;
        ui->le_nextState->setText("SETWORK3");
    }
    // BoundWeight
    else if(workIdx == 3)
    {
        ui->tb_worktape->setItem(0,workIdx,new QTableWidgetItem("0"));
        ui->tb_worktape->setColumnCount(ui->tb_worktape->columnCount()+1);
        ui->tb_worktape->setItem(1,workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,workIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx++;

        ui->tb_resultTape->setItem(0,0,new QTableWidgetItem("0"));
        ui->le_nextState->setText("SETWORK3");
    }
    // Bound
    else if(workIdx == 4)
    {
        ui->tb_worktape->setItem(0,workIdx,new QTableWidgetItem("0"));
        ui->tb_worktape->setItem(1,workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,workIdx-1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx--;
        ui->le_nextState->setText("calBound");
        TuringMachine::getInstance().curState = "calBound";
        TuringMachine::getInstance().nextDifState = "PopHead";
    }
}

// 返回树结点编号对应的深度
int MainWindow::getDepth(int NodeNO)
{
    int ret = 0;
    while(NodeNO!=1)
    {
        NodeNO/=2;
        ret++;
    }
    return ret;
}

// 求解Bound
void MainWindow::calBound()
{
    ui->le_curState->setText("calBound");

    int itemNum = ui->tb_origintape->item(0,1)->text().toInt();
    int curDepth = getDepth(ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx-3)->text().toInt());
    if(curDepth == itemNum)
    {
        TuringMachine::getInstance().curState = TuringMachine::getInstance().nextDifState;
        ui->le_nextState->setText(TuringMachine::getInstance().nextDifState);
        return;
    }
    int targetIdx = 2+curDepth*2;

    if(TuringMachine::getInstance().originIdx<targetIdx)
    {
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem(""));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().originIdx++;
        ui->le_nextState->setText("calBound");
    }
    else if(TuringMachine::getInstance().originIdx>targetIdx)
    {
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem(""));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx-1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().originIdx--;
        ui->le_nextState->setText("calBound");
    }
    else
    {
        // ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx-1)->text()));
        ui->le_nextState->setText("calBound1");
        TuringMachine::getInstance().curState = "calBound1";
    }
}

// 处理weight
void MainWindow::calBound1()
{
    ui->le_curState->setText("calBound1");
    int bagCapacity = ui->tb_origintape->item(0,0)->text().toInt();
    int itemNum = ui->tb_origintape->item(0,1)->text().toInt();
    int curItemNo = (TuringMachine::getInstance().originIdx-2)/2;
    int weight;
    // if(ui->tb_origintape->item(0,TuringMachine::getInstance().originIdx>=ui->tb_origintape->columnCount()))
    // {
    //     weight = 0;
    // }
    // else
    weight = ui->tb_origintape->item(0,TuringMachine::getInstance().originIdx)->text().toInt();

    int BoundWeight = ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx)->text().toInt();
    float curBound = ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx+1)->text().toFloat();

    ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(QString::number(BoundWeight+weight)));
    ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem(""));
    ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx+1,new QTableWidgetItem("^"));
    TuringMachine::getInstance().originIdx++;

    ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
    ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem("^"));
    TuringMachine::getInstance().workIdx++;

    TuringMachine::getInstance().curState = "calBound2";
    ui->le_nextState->setText("calBound2");
}

// 处理value
void MainWindow::calBound2()
{
    ui->le_curState->setText("calBound2");
    int bagCapacity = ui->tb_origintape->item(0,0)->text().toInt();
    int itemNum = ui->tb_origintape->item(0,1)->text().toInt();
    int curItemNo = (TuringMachine::getInstance().originIdx-2)/2;
    int weight;
    int value;
    weight = ui->tb_origintape->item(0,TuringMachine::getInstance().originIdx-1)->text().toInt();
    value = ui->tb_origintape->item(0,TuringMachine::getInstance().originIdx)->text().toInt();
    float BoundWeight = ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx-1)->text().toFloat();
    int curBound = ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx)->text().toInt();

    // 可加
    if(BoundWeight <= bagCapacity)
    {
        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(QString::number(curBound+value)));

        if(curItemNo<itemNum-1)
        {
            ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem(""));
            ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx+1,new QTableWidgetItem("^"));
            TuringMachine::getInstance().originIdx++;

            ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
            ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem("^"));
            TuringMachine::getInstance().workIdx--;
            TuringMachine::getInstance().curState = "calBound1";
            ui->le_nextState->setText("calBound1");
        }
        else
        {
            TuringMachine::getInstance().curState = TuringMachine::getInstance().nextDifState;
            ui->le_nextState->setText(TuringMachine::getInstance().nextDifState);
        }
    }
    // 不可加
    else
    {
        float restToAdd = (bagCapacity-(BoundWeight-weight))*value/weight;
        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(QString::number(curBound+restToAdd)));
        TuringMachine::getInstance().curState = TuringMachine::getInstance().nextDifState;
        ui->le_nextState->setText(TuringMachine::getInstance().nextDifState);
    }
}

// 将优先队列的头节点出栈，根据情况压入左右子节点
void MainWindow::PopHead()
{
    ui->le_curState->setText("PopHead");
    if(TuringMachine::getInstance().workIdx%5!=0)
    {
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx--;
        ui->le_nextState->setText("PopHead");
        return;
    }
    int curID = ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx)->text().toInt();
    int depth = getDepth(curID);
    int toAddIdx = depth*2+2;
    int itemNum = ui->tb_origintape->item(0,1)->text().toInt();
    int curValue = ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx+2)->text().toInt();
    int bestValue = (ui->tb_resultTape->item(0,0)->text()==""?0:ui->tb_resultTape->item(0,0)->text().toInt());

    if(curValue > bestValue)
    {
        ui->tb_resultTape->setItem(0,0,new QTableWidgetItem(QString::number(curValue)));
    }

    float nextTopBound;
    if(ui->tb_worktape->columnCount()<10)
    {
        nextTopBound = 0;
    }
    else if(ui->tb_worktape->item(0,9)->text() == "")
    {
        nextTopBound = 0;
    }
    else
    {
        nextTopBound = ui->tb_worktape->item(0,9)->text().toFloat();
    }
    if(depth == itemNum && nextTopBound<=ui->tb_resultTape->item(0,0)->text().toInt())
    {
        ui->le_nextState->setText("findBestOver");
        TuringMachine::getInstance().curState = "findBestOver";
        return;
    }
    else if(depth == itemNum)
    {
        ui->tb_worktape->removeColumn(0);
        ui->tb_worktape->removeColumn(1);
        ui->tb_worktape->removeColumn(2);
        ui->tb_worktape->removeColumn(3);
        ui->tb_worktape->removeColumn(4);
        ui->le_nextState->setText("SortPriorityQueue");
        TuringMachine::getInstance().curState = "SortPriorityQueue";
    }
    if(TuringMachine::getInstance().originIdx > toAddIdx)
    {
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem(""));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx-1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().originIdx--;
        ui->le_nextState->setText("PopHead");
        return;
    }
    else if(TuringMachine::getInstance().originIdx < toAddIdx)
    {
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem(""));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().originIdx++;
        ui->le_nextState->setText("PopHead");
        return;
    }
    else
    {
        ui->le_nextState->setText("PopHead1");
        TuringMachine::getInstance().curState = "PopHead1";
        return;
    }
}

// push 右结点
void MainWindow::PopHead1()
{
    ui->le_curState->setText("PopHead1");

    // 首先将头结点替换为右子结点
    if(TuringMachine::getInstance().workIdx %5==0)
    {
        int curID = ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx)->text().toInt();

        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(QString::number(curID*2+1)));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx++;
        ui->le_nextState->setText("PopHead1");
    }
    else if(TuringMachine::getInstance().workIdx %5==1)
    {
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx++;
        ui->le_nextState->setText("PopHead1");
    }
    else if(TuringMachine::getInstance().workIdx %5==2)
    {
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx++;
        ui->le_nextState->setText("PopHead1");
    }
    else if(TuringMachine::getInstance().workIdx %5==3)
    {
        int curWeight = ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx-2)->text().toInt();
        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(QString::number(curWeight)));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx++;
        ui->le_nextState->setText("PopHead1");
    }
    else if(TuringMachine::getInstance().workIdx %5==4)
    {
        int curValue = ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx-2)->text().toInt();
        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(QString::number(curValue)));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx--;
        ui->le_nextState->setText("calBound");
        TuringMachine::getInstance().curState = "calBound";
        TuringMachine::getInstance().nextDifState = "PopHead2";
    }

}

//原始指针移动回目标元素 工作指针移动到优先队列最末尾
void MainWindow::PopHead2()
{
    ui->le_curState->setText("PopHead2");
    int bagCapacity = ui->tb_origintape->item(0,0)->text().toInt();
    int rId = ui->tb_worktape->item(0,0)->text().toInt();
    int lId = rId-1;
    int depth = getDepth(lId);
    int toAddIdx = (depth-1)*2+2;
    int rWeight = ui->tb_worktape->item(0,1)->text().toInt();

    if(TuringMachine::getInstance().originIdx > toAddIdx)
    {
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem(""));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx-1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().originIdx--;
        ui->le_nextState->setText("PopHead2");
        return;
    }
    else if(TuringMachine::getInstance().originIdx < toAddIdx)
    {
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem(""));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().originIdx++;
        ui->le_nextState->setText("PopHead2");
        return;
    }
    else if(TuringMachine::getInstance().workIdx == ui->tb_worktape->columnCount()-1)
    {
        // 判断是否能加入左结点
        if(rWeight + ui->tb_origintape->item(0,TuringMachine::getInstance().originIdx)->text().toInt() > bagCapacity)
        {
            ui->le_nextState->setText("SortPriorityQueue");
            TuringMachine::getInstance().curState = "SortPriorityQueue";
            return;
        }

        ui->tb_worktape->setColumnCount(ui->tb_worktape->columnCount()+5);
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx++;
        ui->le_nextState->setText("PopHead3");
        TuringMachine::getInstance().curState = "PopHead3";
        return;
    }
    else if(ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx)->text() == "")
    {
        ui->le_nextState->setText("PopHead3");
        TuringMachine::getInstance().curState = "PopHead3";
        return;
    }
    else
    {
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx++;
        ui->le_nextState->setText("PopHead2");
        return;
    }
}

// push 左结点
void MainWindow::PopHead3()
{
    ui->le_curState->setText("PopHead3");

    int rId = ui->tb_worktape->item(0,0)->text().toInt();
    int lId = rId-1;
    int depth = getDepth(lId);
    int rWeight = ui->tb_worktape->item(0,1)->text().toInt();

    if(TuringMachine::getInstance().workIdx%5 == 0)
    {
        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(QString::number(lId)));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx++;
        ui->le_nextState->setText("PopHead3");
        return;
    }
    else if(TuringMachine::getInstance().workIdx%5 == 1)
    {
        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(QString::number(rWeight+ui->tb_origintape->item(0,TuringMachine::getInstance().originIdx)->text().toInt())));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx++;

        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem(""));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().originIdx++;
        ui->le_nextState->setText("PopHead3");
        return;
    }
    else if(TuringMachine::getInstance().workIdx%5 == 2)
    {
        int rValue =  ui->tb_worktape->item(0,2)->text().toInt();
        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(QString::number(rValue+ui->tb_origintape->item(0,TuringMachine::getInstance().originIdx)->text().toInt())));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx++;
        ui->le_nextState->setText("PopHead3");
        return;
    }
    else if(TuringMachine::getInstance().workIdx%5 == 3)
    {
        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx-2)->text()));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx++;
        ui->le_nextState->setText("PopHead3");
        return;
    }
    else if(TuringMachine::getInstance().workIdx%5 == 4)
    {
        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem(ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx-2)->text()));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx--;
        ui->le_nextState->setText("calBound");
        TuringMachine::getInstance().curState = "calBound";
        TuringMachine::getInstance().nextDifState = "SortPriorityQueue";
        return;
    }
}

void MainWindow::SortPriorityQueue()
{
    ui->le_curState->setText("SortPriorityQueue");
    int QueueSize = TuringMachine::getInstance().workIdx/5+1;
    TNodeElem PriorityQueue[QueueSize];
    for(int i=0;i<QueueSize;i++)
    {
        PriorityQueue[i].NodeID = ui->tb_worktape->item(0,i*5)->text().toInt();
        PriorityQueue[i].curWeight = ui->tb_worktape->item(0,i*5+1)->text().toInt();
        PriorityQueue[i].curValue = ui->tb_worktape->item(0,i*5+2)->text().toInt();
        PriorityQueue[i].boundWeight = ui->tb_worktape->item(0,i*5+3)->text().toInt();
        PriorityQueue[i].bound = ui->tb_worktape->item(0,i*5+4)->text().toFloat();
    }
    if (QueueSize > 0)
    {
        std::sort(PriorityQueue, PriorityQueue + QueueSize, compareTNodeElems);
    }
    for(int i=0;i<QueueSize;i++)
    {
        ui->tb_worktape->setItem(0,i*5,new QTableWidgetItem(QString::number(PriorityQueue[i].NodeID)));
        ui->tb_worktape->setItem(0,i*5+1,new QTableWidgetItem(QString::number(PriorityQueue[i].curWeight)));
        ui->tb_worktape->setItem(0,i*5+2,new QTableWidgetItem(QString::number(PriorityQueue[i].curValue)));
        ui->tb_worktape->setItem(0,i*5+3,new QTableWidgetItem(QString::number(PriorityQueue[i].boundWeight)));
        ui->tb_worktape->setItem(0,i*5+4,new QTableWidgetItem(QString::number(PriorityQueue[i].bound)));
    }
    ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
    ui->tb_worktape->setItem(1,0,new QTableWidgetItem("^"));
    TuringMachine::getInstance().workIdx = 0;
    ui->le_nextState->setText("PopHead");
    TuringMachine::getInstance().curState = "PopHead";
}

void MainWindow::findBestOver()
{
    ui->le_curState->setText("findBestOver");
    ui->tb_resultTape->setColumnCount(ui->tb_resultTape->columnCount()+1);
    ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx,new QTableWidgetItem(""));
    ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx+1,new QTableWidgetItem("^"));
    TuringMachine::getInstance().outIdx++;
    TuringMachine::getInstance().reg = ui->tb_worktape->item(0,0)->text().toInt();
    ui->le_nextState->setText("BackTrackSolution");
    TuringMachine::getInstance().curState = "BackTrackSolution";
}

// 根据结点编号求最优解
void MainWindow::BackTrackSolution()
{
    ui->le_curState->setText("BackTrackSolution");
    if(TuringMachine::getInstance().reg > 1)
    {
        ui->tb_resultTape->setColumnCount(ui->tb_resultTape->columnCount()+1);
        if(TuringMachine::getInstance().reg%2 == 0)
        {
            ui->tb_resultTape->setItem(0,TuringMachine::getInstance().outIdx,new QTableWidgetItem("1"));
        }
        else
        {
            ui->tb_resultTape->setItem(0,TuringMachine::getInstance().outIdx,new QTableWidgetItem("0"));
        }
        TuringMachine::getInstance().reg/=2;
        ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx,new QTableWidgetItem(""));
        ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().outIdx++;
        ui->le_nextState->setText("BackTrackSolution");
        return;
    }
    ui->le_nextState->setText("BackTrackOver");
    TuringMachine::getInstance().curState = "BackTrackOver";
}

/* ------------------- 0-1背包备忘录--------------------*/
void MainWindow::SETWORK4() {
    ui->le_curState->setText("SETWORK4");
    int colNum = ui->tb_origintape->item(0,0)->text().toInt() + 1;
    int rowNum = ui->tb_origintape->item(0,1)->text().toInt() + 1;
    ui->tb_worktape->setColumnCount(colNum*rowNum);

    // 之后初始化DP矩阵第一行M(0,?)的值为0   选前0个物品，背包容量为?时的最优值
    if(TuringMachine::getInstance().workIdx <= ui->tb_origintape->item(0,0)->text().toInt())
    {
        ui->tb_worktape->setItem(0, TuringMachine::getInstance().workIdx, new QTableWidgetItem("0"));
        ui->tb_worktape->setColumnCount(ui->tb_worktape->columnCount()+1);

        TuringMachine::getInstance().workIdx++;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem(""));

        ui->le_nextState->setText("SETWORK4");
    }
    else
    {
        ui->le_nextState->setText("InitColumn0");
        TuringMachine::getInstance().curState = "InitColumn0";
    }
}

void MainWindow::InitColumn0()
{
    ui->le_curState->setText("InitColumn0");
    int colNum = ui->tb_origintape->item(0,0)->text().toInt() + 1;
    int rowNum = ui->tb_origintape->item(0,1)->text().toInt() + 1;

    if(TuringMachine::getInstance().workIdx%colNum==0)
    {
        ui->tb_worktape->setItem(0,TuringMachine::getInstance().workIdx,new QTableWidgetItem("0"));
        TuringMachine::getInstance().workIdx++;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem(""));
        ui->le_nextState->setText("InitColumn0");
    }
    else
    {
        if(TuringMachine::getInstance().workIdx == colNum*rowNum-1)
        {
            QString fun = QString("calBackPack(%1,%2)").arg(rowNum-1).arg(colNum-1);

            TuringMachine::getInstance().funStack.push_back(fun);
            freshStackTextEdit();
            ui->le_nextState->setText("PopFunStack");
            TuringMachine::getInstance().curState = "PopFunStack";
            return;
        }
        TuringMachine::getInstance().workIdx++;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx-1,new QTableWidgetItem(""));
        ui->le_nextState->setText("InitColumn0");
    }
}

void MainWindow::calBackPack(int i,int j)
{
    ui->le_curState->setText(QString("calBackPack(%1,%2)").arg(i).arg(j));
    int bagCapacity = ui->tb_origintape->item(0,0)->text().toInt();
    int itemNum = ui->tb_origintape->item(0,1)->text().toInt();
    int nextWorkIdx = i*(bagCapacity+1)+j;
    if(j<0)
    {
        TuringMachine::getInstance().numStack.push_back(-100);
        ui->le_nextState->setText("PopFunStack");
        TuringMachine::getInstance().curState = "PopFunStack";
    }
    else if(i<=0)
    {
        if(i==0) ui->tb_worktape->setItem(0,nextWorkIdx,new QTableWidgetItem("0"));
        TuringMachine::getInstance().numStack.push_back(0);
        ui->le_nextState->setText("PopFunStack");
        TuringMachine::getInstance().curState = "PopFunStack";
    }
    else if(ui->tb_worktape->item(0,nextWorkIdx) == NULL)
    {
        TuringMachine::getInstance().funStack.push_back(QString("calMaxTo(%1,%2)").arg(i).arg(j));
        int oriIdx = i*2;
        int weight = ui->tb_origintape->item(0,oriIdx)->text().toInt();
        int value = ui->tb_origintape->item(0,oriIdx+1)->text().toInt();
        TuringMachine::getInstance().numStack.push_back(value);
        TuringMachine::getInstance().funStack.push_back(QString("calBackPack(%1,%2)").arg(i-1).arg(j-weight));
        TuringMachine::getInstance().funStack.push_back(QString("calBackPack(%1,%2)").arg(i-1).arg(j));
        ui->le_nextState->setText("PopFunStack");
        TuringMachine::getInstance().curState = "PopFunStack";
    }
    else if((ui->tb_worktape->item(0,i*(bagCapacity+1)+j)->text())=="")
    {
        TuringMachine::getInstance().funStack.push_back(QString("calMaxTo(%1,%2)").arg(i).arg(j));
        int oriIdx = i*2;
        int weight = ui->tb_origintape->item(0,oriIdx)->text().toInt();
        int value = ui->tb_origintape->item(0,oriIdx+1)->text().toInt();
        TuringMachine::getInstance().numStack.push_back(value);
        TuringMachine::getInstance().funStack.push_back(QString("calBackPack(%1,%2)").arg(i-1).arg(j-weight));
        TuringMachine::getInstance().funStack.push_back(QString("calBackPack(%1,%2)").arg(i-1).arg(j));
        ui->le_nextState->setText("PopFunStack");
        TuringMachine::getInstance().curState = "PopFunStack";
    }
    else
    {
        ui->tb_worktape->setItem(0,nextWorkIdx,new QTableWidgetItem(QString::number(ui->tb_worktape->item(0,nextWorkIdx)->text().toInt())));
        TuringMachine::getInstance().numStack.push_back(ui->tb_worktape->item(0,nextWorkIdx)->text().toInt());
        ui->le_nextState->setText("PopFunStack");
        TuringMachine::getInstance().curState = "PopFunStack";
    }

    ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
    ui->tb_worktape->setItem(1,nextWorkIdx,new QTableWidgetItem("^"));
    TuringMachine::getInstance().workIdx = nextWorkIdx;
    freshStackTextEdit();
}

void MainWindow::calMaxTo(int i,int j)
{
    ui->le_curState->setText(QString("calMaxTo(%1,%2)").arg(i).arg(j));
    int num1 = TuringMachine::getInstance().numStack.back();
    TuringMachine::getInstance().numStack.pop_back();
    int num2 = TuringMachine::getInstance().numStack.back();
    TuringMachine::getInstance().numStack.pop_back();
    int num1Plus = TuringMachine::getInstance().numStack.back();
    TuringMachine::getInstance().numStack.pop_back();
    int maxResult = max(num1+num1Plus,num2);
    TuringMachine::getInstance().numStack.push_back(maxResult);

    int bagCapacity = ui->tb_origintape->item(0,0)->text().toInt();
    int nextWorkIdx = i*(bagCapacity+1)+j;
    ui->tb_worktape->setItem(0,nextWorkIdx,new QTableWidgetItem(QString::number(maxResult)));

    ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
    ui->tb_worktape->setItem(1,nextWorkIdx,new QTableWidgetItem("^"));
    TuringMachine::getInstance().workIdx = nextWorkIdx;

    ui->le_nextState->setText("PopFunStack");
    TuringMachine::getInstance().curState = "PopFunStack";
    freshStackTextEdit();
}

void MainWindow::PopFunStack()
{
    ui->le_curState->setText("PopFunStack");
    if(TuringMachine::getInstance().funStack.isEmpty())
    {
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem(""));
        ui->tb_origintape->setItem(1,ui->tb_origintape->columnCount()-2,new QTableWidgetItem("^"));
        TuringMachine::getInstance().originIdx = ui->tb_origintape->columnCount()-2;

        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem(""));
        ui->tb_worktape->setItem(1,ui->tb_worktape->columnCount()-1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().workIdx = ui->tb_worktape->columnCount()-1;

        ui->tb_resultTape->setColumnCount(ui->tb_resultTape->columnCount()+1);
        ui->tb_resultTape->setItem(0,0,new QTableWidgetItem(QString::number(TuringMachine::getInstance().numStack.back())));
        ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx,new QTableWidgetItem(""));
        ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx+1,new QTableWidgetItem("^"));
        TuringMachine::getInstance().outIdx++;
        ui->le_nextState->setText("MemoBackTrack");
        TuringMachine::getInstance().curState = "MemoBackTrack";
        return;
    }
    QString fun = TuringMachine::getInstance().funStack.back();
    TuringMachine::getInstance().funStack.pop_back();

    ui->le_nextState->setText(fun);
    TuringMachine::getInstance().curState = fun;
    freshStackTextEdit();
}

void MainWindow::MemoBackTrack()
{
    ui->le_curState->setText("MemoBackTrack");

    int bagCapacity = ui->tb_origintape->item(0,0)->text().toInt();
    int weight = ui->tb_origintape->item(0,TuringMachine::getInstance().originIdx)->text().toInt();
    int i = (TuringMachine::getInstance().workIdx)/(bagCapacity+1);
    int j = (TuringMachine::getInstance().workIdx)%(bagCapacity+1);
    if(i==0)
    {
        ui->le_nextState->setText("MemoBackTrackOver");
        TuringMachine::getInstance().curState = "MemoBackTrackOver";
    }
    else
    {
        TuringMachine::getInstance().reg = ui->tb_worktape->item(0,TuringMachine::getInstance().workIdx)->text().toInt();

        ui->le_nextState->setText(QString("MemoCMP_M(%1,%2)").arg(i-1).arg(j));
        TuringMachine::getInstance().curState = QString("MemoCMP_M(%1,%2)").arg(i-1).arg(j);
    }
}

// 将工作纸带移动到M[i-1][j]的位置，比较M[i][j]与M[i-1][j]
// 若相等，则物品i不选，进入BackTrack
// 若不等，则物品i选，进入MoveWorkToM移动到 M[i][j-weight]
void MainWindow::MemoCMP_M(int i,int j)
{
    ui->le_curState->setText(QString("MemoCMP_M(%1,%2)").arg(i).arg(j));
    int bagSize = ui->tb_origintape->item(0,0)->text().toInt();
    int idx = i*(bagSize+1)+j;
    if(TuringMachine::getInstance().workIdx > idx)
    {
        TuringMachine::getInstance().workIdx--;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem(""));
        ui->le_nextState->setText(QString("MemoCMP_M(%1,%2)").arg(i).arg(j));
    }
    else if(TuringMachine::getInstance().originIdx %2 !=0)
    {
        TuringMachine::getInstance().originIdx--;
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx+1,new QTableWidgetItem(""));
        ui->le_nextState->setText(QString("MemoCMP_M(%1,%2)").arg(i).arg(j));
    }
    else
    {
        // dp[i][j] == dp[i - 1][j] => item[i] = 0; => findWhat(i - 1, j);
        if(TuringMachine::getInstance().reg == ui->tb_worktape->item(0,idx)->text().toInt())
        {
            ui->tb_resultTape->setItem(0,TuringMachine::getInstance().outIdx,new QTableWidgetItem("0"));

            ui->tb_resultTape->setColumnCount(ui->tb_resultTape->columnCount()+1);
            TuringMachine::getInstance().outIdx++;
            ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx,new QTableWidgetItem("^"));
            ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx-1,new QTableWidgetItem(""));

            ui->le_nextState->setText("MemoBackTrack");
            TuringMachine::getInstance().curState = "MemoBackTrack";
        }
        // dp[i][j] != dp[i - 1][j] => item[i] = 1; => findWhat(i - 1, j - w[i]);
        else
        {
            int weight = ui->tb_origintape->item(0,TuringMachine::getInstance().originIdx)->text().toInt();
            ui->tb_resultTape->setItem(0,TuringMachine::getInstance().outIdx,new QTableWidgetItem("1"));

            ui->tb_resultTape->setColumnCount(ui->tb_resultTape->columnCount()+1);
            TuringMachine::getInstance().outIdx++;
            ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx,new QTableWidgetItem("^"));
            ui->tb_resultTape->setItem(1,TuringMachine::getInstance().outIdx-1,new QTableWidgetItem(""));

            ui->le_nextState->setText(QString("MemoMoveWorkToM(%1,%2)").arg(i).arg(j-weight));
            TuringMachine::getInstance().curState = QString("MemoMoveWorkToM(%1,%2)").arg(i).arg(j-weight);
        }
        TuringMachine::getInstance().originIdx--;
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx,new QTableWidgetItem("^"));
        ui->tb_origintape->setItem(1,TuringMachine::getInstance().originIdx+1,new QTableWidgetItem(""));
    }
}

void MainWindow::MemoMoveWorkToM(int i,int j)
{
    ui->le_curState->setText(QString("MemoMoveWorkToM(%1,%2)").arg(i).arg(j));
    int bagSize = ui->tb_origintape->item(0,0)->text().toInt();
    int idx = i*(bagSize+1)+j;
    if(TuringMachine::getInstance().workIdx > idx)
    {
        TuringMachine::getInstance().workIdx--;
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx,new QTableWidgetItem("^"));
        ui->tb_worktape->setItem(1,TuringMachine::getInstance().workIdx+1,new QTableWidgetItem(""));
        ui->le_nextState->setText(QString("MemoMoveWorkToM(%1,%2)").arg(i).arg(j));
    }
    else
    {
        ui->le_nextState->setText("MemoBackTrack");
        TuringMachine::getInstance().curState = "MemoBackTrack";
    }
}

void MainWindow::MemoBackTrackOver()
{
    ui->le_curState->setText("MemoBackTrackOver");
    ui->le_nextState->setText("halt");
}


void MainWindow::freshStackTextEdit()
{
    ui->te_functionEmu->clear();
    ui->te_numEmu->clear();
    if(TuringMachine::getInstance().funStack.empty() && TuringMachine::getInstance().numStack.empty())
    {
        return;
    }
    for(int i=TuringMachine::getInstance().funStack.size()-1;i>=0;i--)
    {
        ui->te_functionEmu->append(TuringMachine::getInstance().funStack[i]+"\n");
    }
    for(int i=TuringMachine::getInstance().numStack.size()-1;i>=0;i--)
    {
        ui->te_numEmu->append(QString::number(TuringMachine::getInstance().numStack[i])+"\n");
    }
}

void MainWindow::on_cb_selectMenu_activated(int index)
{
    // 根据 comboBox 的当前索引设置 textEdit 的内容
    QString text;
    switch (index) {
    case 0:
        text = "BinarySearch(Iterative)\n"
               "请输入 需要搜索的数 搜索的起始下标 搜索的终止下标 搜索数序列\n"
               "e.g. 5 0 9 1 2 3 4 6 7 8 9 10\n";
        break;
    case 1:
        text = "BinarySearch(Recursive)";
        break;
    case 2:
        text = "Dynamic Plan\n"
               "请输入 背包容量 物品个数n w1 v1 w2 v2 ... wn vn\n"
               "e.g. 4 4 2 3 3 4 4 5 5 6\n"
               "e.g. 50 5 5 12 15 30 25 44 27 46 30 50\n";
        break;
    case 3:
        text = "Branch and Bound\n"
               "请输入 背包容量 物品个数n w1 v1 w2 v2 ... wn vn\n"
               "e.g. 4 4 2 3 3 4 4 5 5 6\n"
               "e.g. 50 5 5 12 15 30 25 44 27 46 30 50\n";
        break;
    case 4:
        text = "Memory\n"
               "请输入 背包容量 物品个数n w1 v1 w2 v2 ... wn vn\n"
               "e.g. 4 4 2 3 3 4 4 5 5 6\n"
               "e.g. 50 5 5 12 15 30 25 44 27 46 30 50\n";
        break;
    default:
        text = "...";
        break;
    }
    TuringMachine::getInstance().reset();
    reset();
    ui->te_Notice->setText(text);
    selectIdx = index;
}

// 工作纸带变动槽函数
void MainWindow::on_tb_worktape_cellChanged(int row, int column)
{
    if(ui->cb_selectMenu->currentIndex()!=2 &&  ui->cb_selectMenu->currentIndex()!=4)
    {
        return;
    }
    // 获取矩阵的列数和行数
    QTableWidgetItem *colsItem = ui->tb_origintape->item(0, 0);
    QTableWidgetItem *rowsItem = ui->tb_origintape->item(0, 1);

    // 检查 colsItem 和 rowsItem 是否为 nullptr
    if (!colsItem || !rowsItem) {
        return; // 如果任意一个为空，提前退出函数
    }

    int cols = colsItem->text().toInt() + 1;
    int rows = rowsItem->text().toInt() + 1;
    ui->tb_Matrix->setRowCount(rows);
    ui->tb_Matrix->setColumnCount(cols);

    // 设置行和列标签
    QStringList rowLabels;
    QStringList colLabels;

    for (int i = 0; i < rows; ++i) {
        rowLabels << QString::number(i);
    }

    for (int i = 0; i < cols; ++i) {
        colLabels << QString::number(i);
    }

    ui->tb_Matrix->setVerticalHeaderLabels(rowLabels);
    ui->tb_Matrix->setHorizontalHeaderLabels(colLabels);

    if(ui->cb_selectMenu->currentIndex()==2)
    {
        int newRow = (column - 2) / cols;
        int newCol = (column - 2) % cols;

        if (row == 0 && column > 1) {
            QTableWidgetItem *worktapeItem = ui->tb_worktape->item(row, column);
            if (worktapeItem) {
                ui->tb_Matrix->setItem(newRow, newCol, new QTableWidgetItem(worktapeItem->text()));
            } else {
                ui->tb_Matrix->setItem(newRow, newCol, new QTableWidgetItem(""));
            }
        }
    }
    else if(ui->cb_selectMenu->currentIndex()==4)
    {
        int newRow = column / cols;
        int newCol = column % cols;
        if (row == 0) {
            QTableWidgetItem *worktapeItem = ui->tb_worktape->item(row, column);
            if (worktapeItem) {
                ui->tb_Matrix->setItem(newRow, newCol, new QTableWidgetItem(worktapeItem->text()));
            } else {
                ui->tb_Matrix->setItem(newRow, newCol, new QTableWidgetItem(""));
            }
        }
    }
}

// 到下一个状态
void MainWindow::on_btn_nextState_clicked()
{
    while(ui->le_nextState->text() == ui->le_curState->text())
    {
        on_btn_step_clicked();
    }
}

