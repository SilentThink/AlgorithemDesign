#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "const.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

typedef struct TNodeElem
{
    int NodeID;
    int curWeight;
    int curValue;
    int boundWeight;
    float bound;
}TNodeElem;

static bool compareTNodeElems(const TNodeElem &a, const TNodeElem &b)
{
    return a.bound > b.bound;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setWork();
    void CALMID();
    void FINDMID();
    void CMPTARGET();
    void UpdateRight();
    void UpdateLeft();
    void GetResult();
    void NotFound();

    // 0-1背包动态规划
    void SETWORK2();
    void ReadWeight();
    void Cmp();
    void Read_M(int i,int j);
    void Read_Second_M(int i,int j);
    void Write_M(int i,int j);
    void BackTrack();
    void CMP_M(int i,int j);
    void MoveWorkToM(int i,int j);
    void BackTrackOver();

    // 0-1背包分支限界
    int getDepth(int NodeNO);

    void SETWORK3();
    void calBound();
    void calBound1();
    void calBound2();
    void PopHead();
    void PopHead1();
    void PopHead2();
    void PopHead3();
    void SortPriorityQueue();
    void findBestOver();
    void BackTrackSolution();

    // 0-1背包备忘录
    void freshStackTextEdit();

    void SETWORK4();
    void InitColumn0();
    void calBackPack(int i,int j);
    void PopFunStack();
    void calMaxTo(int i,int j);
    void MemoBackTrack();
    void MemoCMP_M(int i,int j);
    void MemoMoveWorkToM(int i,int j);
    void MemoBackTrackOver();

    // 重置函数
    void reset();

private slots:
    void on_btn_input_clicked();

    void on_btn_step_clicked();

    void on_btn_initTape_clicked();

    void on_btn_runAll_clicked();


    void on_cb_selectMenu_activated(int index);

    void on_tb_worktape_cellChanged(int row, int column);

    void on_btn_nextState_clicked();

public:
    int selectIdx = 0;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
