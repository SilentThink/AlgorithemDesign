#ifndef TURINGMACHINE_H
#define TURINGMACHINE_H
#include "const.h"

using namespace std;
class Rule;

typedef struct PackItem
{
    int w;
    int v;
}PackItem;

class TuringMachine
{
public:
    // 获取单例实例
    static TuringMachine& getInstance()
    {
        static TuringMachine instance;
        return instance;
    }

    // 禁用拷贝构造函数和赋值运算符
    TuringMachine(const TuringMachine&) = delete;
    TuringMachine& operator=(const TuringMachine&) = delete;

public:
    // 私有构造函数
    TuringMachine();
    void reset();

    static bool comparePackItems(const PackItem &a, const PackItem &b)
    {
        double ratioA = static_cast<double>(a.v) / a.w;
        double ratioB = static_cast<double>(b.v) / b.w;

        // Sort in descending order based on v/w ratio
        return ratioA > ratioB;
    }

    // 二分搜索序列排序函数
    void sortTapeElem();

    // 背包物品排序函数
    void sortPackItem();

    Rule* rules;
    QString curState;
    QString nextDifState;   // 下一个变化的状态，不同于nextState
    int tapeElem[100];
    int ElemSize=0;

    PackItem packItems[100];
    int packItemNum = 0;

    int originIdx = 0;
    int workIdx = 0;
    int outIdx = 0;
    int originSize=0;
    int workSize=0;
    int outSize=0;

    int reg;

    QVector<QString> funStack;
    QVector<int> numStack;

    int steps = 0;
    bool halt = false;
};


class Rule
{
public:
    Rule();

private:
    QString currentState;
    int read;
    QString nextState;
    QString move;
    int write;
};

#endif // TURINGMACHINE_H
