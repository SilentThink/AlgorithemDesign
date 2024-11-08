#include "tree.h"
void init(){ //初始化商品结构体数组
    vector<int> temp_weight;
    vector<int> temp_value;
    cin>>capacity;
    cin>>num;
    for(int i=0;i<num;i++){
        int temp;
        cin>>temp;
        temp_weight.push_back(temp);
    }
    for(int i=0;i<num;i++){
        int temp;
        cin>>temp;
        temp_value.push_back(temp);
    }
    for(int i=0;i<num;i++){
        item temp;
        temp.weight=temp_weight[i];
        temp.value=temp_value[i];
        items.push_back(temp);
    }
}

void output_items() {
    cout << "items' information" << endl;
    cout << "NO.\tWeight\tValue" << endl;
    for (int i = 0; i < num; ++i) {
        cout << i + 1 << "\t" << items[i].weight << "\t" << items[i].value << endl;
    }
}

void buildTree(node* root, int index, int currentWeight, int currentTotalValue, const vector<int>& chosenItems) {
    root->cw = currentWeight;
    root->cv = currentTotalValue;
    root->itemsnode = chosenItems;
    root->cur_layyer = index;

    if(currentWeight>capacity){
        root->situation=2;
        root->choose = nullptr;
        root->not_choose = nullptr;
        return;
    }

    if (index == num) {
        if (currentTotalValue > bestv) {
            bestv = currentTotalValue;
            bestitems = root->itemsnode;
        }
        root->choose = nullptr;
        root->not_choose = nullptr;
        return;
    }

    // 选择当前商品
    if (currentWeight + items[index].weight <= capacity) {
        vector<int> newChosenItems = chosenItems;
        newChosenItems.push_back(index + 1); // 添加当前商品编号
        root->choose = new node();
        root->choose->situation=1;
        buildTree(root->choose, index + 1, currentWeight + items[index].weight, currentTotalValue + items[index].value, newChosenItems);
    } else {
        root->choose = nullptr; // 如果不能选择当前商品，设置为nullptr
    }

    // 不选择当前商品
    root->not_choose = new node();
    root->not_choose->situation=0;
    buildTree(root->not_choose, index + 1, currentWeight, currentTotalValue, chosenItems);

}

void output_bestitems(){
    // 打印最优解
    cout << "Best solution:";
    for (int item : bestitems) {
        cout << item << " ";
    }
    cout << endl << "total value:" << bestv << endl;
}

void clear_tree(){
    // 清理内存
    delete root->choose;
    delete root->not_choose;
    delete root;
}
void output_currentnode(node *cur){
    if (cur == nullptr) {
        std::cout << "empty!" << std::endl;
        return;
    }
    std::cout << "information of currentnode:" << std::endl;
    std::cout << "totalweight:" << cur->cw << std::endl;
    std::cout << "totalvalue:" << cur->cv << std::endl;
    std::cout << "number of node:";
    for (size_t i = 0; i < cur->itemsnode.size(); ++i) {
        std::cout << cur->itemsnode[i];
    }
    std::cout << std::endl;
}

void one_step(node* node,int cur_state) {//根据节点内的信息，判断当前节点对应状态
    if (node == nullptr) {
        if(cur_state==2){//非叶子节点则对应backtrack状态
            cout<<"Overweight!Backtrack"<<endl;
        }
        else{//对应遇到叶子节点状态
            cout<<"Meet the leaf node!"<<endl;
        }
        return; // 基线条件，如果节点为空，则直接返回
    }

    // 标记当前访问的节点及状态
    cout << "items' information" << endl;
    cout << "NO.\tWeight\tValue" << endl;
    for (int i = 0; i < num; ++i) {
        cout << i + 1 << "\t" << items[i].weight << "\t" << items[i].value ;
        if(i+1==node->cur_layyer){
            cout<<"<---";
            if(cur_state==0){
                cout<<"not choose!";
            }
            else if(cur_state==1){
                cout<<"choose!";
            }
        }
        cout << endl;
    }

    // 递归地遍历选择子节点和不选择子节点
    if(node->choose==nullptr&&node->not_choose!=nullptr){
        one_step(node->choose,2);
    }
    else {
        one_step(node->choose,1);
    }
    one_step(node->not_choose,0);
}

void dfsTraversal(node* current) {
    if (current == nullptr) {
        return; // 基线条件，如果当前节点为空，则直接返回
    }

    // 访问当前节点，将其存入队列
    sta.push(current);

    // 遍历 choose 子节点
    if (current->choose) {
        dfsTraversal(current->choose);
    }

    // 遍历 not_choose 子节点
    if (current->not_choose) {
        dfsTraversal(current->not_choose);
    }
}
