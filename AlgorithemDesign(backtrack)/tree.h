#ifndef TREE_H
#define TREE_H
#include <vector>
#include<iostream>
#include <QLineEdit>
#include <stack>
#include <queue>
using namespace std;
struct item{
    int value;
    int weight;
};
struct node {
    vector<int> itemsnode; // 选择的商品编号
    int cw;        // 当前总重量
    int cv;         // 当前总价值
    int cur_layyer; //节点在树中的层数
    int situation;  //1是choose状态，0是not_choose状态,2是backtrack状态
    struct node* choose;
    struct node* not_choose;
    int lastBranch;
};

inline int capacity;//背包容量
inline int num;//商品数量；
inline int bestv=0;//目前最优解总价值
inline int steps=0;
inline vector<int> bestitems;//目前最优解
inline vector<item> items;
inline node* root = new node();//解空间树的根节点
inline queue<node*> sta;
inline vector<node*> outputsta;
inline int curbestv;
inline QString a;//是否输出overweight状态

void init();
void output_items();
void buildTree(node* root, int index, int currentWeight, int currentTotalValue, const vector<int>& chosenItems);
void output_bestitems();
void clear_tree();
void output_currentnode(node* cur);
void one_step(node* node,int cur_state);
void dfsTraversal(node* current);
#endif // TREE_H
