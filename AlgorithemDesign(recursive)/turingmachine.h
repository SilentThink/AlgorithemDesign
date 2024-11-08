#ifndef TURINGMACHINE_H
#define TURINGMACHINE_H
#include "const.h"

using namespace std;
class Rule;

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

    // 排序函数
    void sortTapeElem();

    Rule* rules;
    QString curState;
    QString nextState;
    int tapeElem[100];
    int ElemSize=0;

    int originIdx = 0;
    int workIdx = 0;
    int outIdx = 0;
    int originSize=0;
    int workSize=0;
    int outSize=0;
    int low=0;
    int high=0;
    int x=0;
    int steps = 0;
    int gridNum=0;
    int midValue=0;
    QVector<QString> stack;
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
