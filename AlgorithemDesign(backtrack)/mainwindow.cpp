// mainwindow.cpp
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <qstring.h>
#include "tree.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::on_initializeButton_clicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::on_executeButton_clicked);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::on_onestepButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_initializeButton_clicked()
{
    // 获取输入
    QLineEdit *inputEdit = findChild<QLineEdit*>("inputEdit"); // 假设控件的objectName是"inputEdit"
    if (!inputEdit) {
        QMessageBox::warning(this, "Error", "输入框未找到！");
        return;
    }
    QString input = inputEdit->text();

    // 使用空格分隔输入的字符串
    QStringList inputs = input.split(" ", Qt::SkipEmptyParts);

    // 检查输入数量
    if (inputs.size() < 3) {
        QMessageBox::warning(this, "Error", "输入信息不完整！");
        return;
    }

    // 解析背包容量和商品数量
    capacity = inputs[0].toInt();
    num = inputs[1].toInt();

    // 检查容量和商品数量是否有效
    if (capacity <= 0 || num <= 0) {
        QMessageBox::warning(this, "Error", "背包容量和商品数量必须大于0！");
        return;
    }

    // 解析每个商品的重量和价值
    items.clear(); // 清空之前的输入
    vector<int> vec_weight;
    vector<int> vec_value;
    for (int i = 2; i < num+2; i ++) {
        int weight = inputs[i].toInt();
        // 检查每个商品的重量和价值是否有效
        if (weight <= 0) {
            QMessageBox::warning(this, "Error", "商品重量必须大于0！");
            return;
        }
        vec_weight.push_back(weight);
    }
    for (int i = 2+num; i < 2+num+num; i ++) {
        int value = inputs[i].toInt();
        // 检查每个商品的重量和价值是否有效
        if ( value <= 0) {
            QMessageBox::warning(this, "Error", "商品价值必须大于0！");
            return;
        }
        vec_value.push_back(value);
    }
    for(int i=0;i<num;i++){
        item temp;
        temp.weight = vec_weight[i];
        temp.value = vec_value[i];
        items.push_back(temp);
    }

    // 将商品信息输出到QTextEdit控件
    QTextEdit *itemsEdit = findChild<QTextEdit*>("itemsEdit"); // 假设控件的objectName是"itemsEdit"
    if (!itemsEdit) {
        QMessageBox::warning(this, "Error", "商品信息输出控件未找到！");
        return;
    }
    itemsEdit->clear(); // 清空之前的输出
    itemsEdit->append(QString("商品\t重量\t价值"));
    for (int i = 0; i < num; ++i) {
        itemsEdit->append(QString("%1\t%2\t%3")
                              .arg(i + 1)
                              .arg(items[i].weight)
                              .arg(items[i].value));
    }
    while (!sta.empty()) sta.pop();
    dfsTraversal(root);
    steps=-1;
    curbestv=0;
    outputsta.clear();
}

void MainWindow::on_executeButton_clicked() {
    // 将执行结果输出到QTextEdit控件
    QTextEdit *bestitemEdit = findChild<QTextEdit*>("bestitemEdit");
    if (!bestitemEdit) {
        QMessageBox::warning(this, "Error", "结果输出控件未找到！");
        return;
    }
    bestitemEdit->clear(); // 清空之前的输出

    // 函数 buildTree 来构建解空间树并找到最优解
    // 调用这个函数来执行背包问题的算法
    buildTree(root, 0, 0, 0, {});

    // 假设 bestitems 和 bestv 是全局变量或类成员变量，存储了最优解信息
    // 输出最优解
    bestitemEdit->append(QString("选择商品编号"));
    for (int item : bestitems) {
        bestitemEdit->append(QString::number(item));
    }

    QLineEdit *bestvEdit = findChild<QLineEdit*>("bestvEdit");
    if (!bestvEdit) {
        QMessageBox::warning(this, "Error", "结果输出控件未找到！");
        return;
    }
    bestvEdit->clear(); // 清空之前的输出
    bestvEdit->setText(QString::number(bestv));
}

void MainWindow::updateUIWithCurrentNode(node* current) {
    // 确保当前节点不为空
    if (!current) {
        QMessageBox::warning(this, "Error", "当前节点为空！");
        return;
    }

    // 获取当前总重量并更新到cwEdit
    QLineEdit *cwEdit = findChild<QLineEdit*>("cwEdit"); // 假设控件的objectName是"cwEdit"
    if (cwEdit) {
        cwEdit->clear();
        cwEdit->setText(QString::number(current->cw));
    } else {
        QMessageBox::warning(this, "Error", "总重量显示控件未找到！");
    }

    // 获取当前总价值并更新到cvEdit
    QLineEdit *cvEdit = findChild<QLineEdit*>("cvEdit"); // 假设控件的objectName是"cvEdit"
    if (cvEdit) {
        cvEdit->clear();
        cvEdit->setText(QString::number(current->cv));
    } else {
        QMessageBox::warning(this, "Error", "总价值显示控件未找到！");
    }

    // 获取节点中的itemsnode并更新到outputEdit
    QLineEdit *outputEdit = findChild<QLineEdit*>("outputEdit"); // 假设控件的objectName是"outputEdit"
    if (outputEdit) {
        outputEdit->clear();
        QString itemsString;
        // 将itemsnode中的每个商品编号转换为字符串并用逗号分隔
        for (int i = 0; i < current->itemsnode.size(); ++i) {
            itemsString += QString::number(current->itemsnode[i]);
            if (i < current->itemsnode.size() - 1) { // 如果不是最后一个元素，添加逗号分隔
                itemsString += " ";
            }
        }
        outputEdit->setText(itemsString);
    } else {
        QMessageBox::warning(this, "Error", "商品编号输出控件未找到！");
    }

    //输出总步数
    QLineEdit *stepEdit = findChild<QLineEdit*>("stepEdit");
    if(stepEdit){
        stepEdit->clear();
        stepEdit->setText(QString::number(steps));
    }else {
        QMessageBox::warning(this, "Error", "总步数输出控件未找到！");
    }


    //输出状态
    QLineEdit *stateEdit = findChild<QLineEdit*>("stateEdit");
    if (stateEdit) {
        stateEdit->clear();
        if(current==root){
            stateEdit->setText(QString("Start!"));
            return;
        }
        if(current->situation==1){
            stateEdit->setText(QString("Choose!"));
        }
        else if(current->situation==0){
            QString situationText="Not choose!";
            stateEdit->setText(a + situationText);
        }
        if(current->choose==nullptr&&current->not_choose!=nullptr){
            a="OverWeight!";
        }
        else {
            a="";
        }
    } else {
        QMessageBox::warning(this, "Error", "状态输出控件未找到！");
    }

    //输出箭头
    QLineEdit *arrowEdit1= findChild<QLineEdit*>("arrowEdit1");
    if (arrowEdit1) {
        arrowEdit1->clear();

        QString spaces="0 0 ";
        for (int i = 0; i < num; ++i) {
            if(i==current->cur_layyer-1){
                spaces +="1 ";
            }
            else{
                spaces += "0 ";
            }
        }
        for (int i = 0; i < num; ++i) {
            if(i==current->cur_layyer-1){
                spaces +="1 ";
            }
            else{
                spaces += "0 ";
            }
        }
        // 将含有五个空格的字符串输出到 arrowEdit1
        arrowEdit1->setText(spaces);
    } else {
        QMessageBox::warning(this, "Error", "指针输出控件未找到！");
    }

}

void outputQueueToStaEdit(QTextEdit* staEdit) {
    if (!staEdit) {
        return; // 如果 staEdit 为空，则直接返回
    }

    staEdit->clear(); // 清空 staEdit 中的旧内容

    for (int i = outputsta.size() - 1; i >= 0; --i) {
        node* currentNode = outputsta[i];
        if (currentNode) {
            // 构建当前节点的信息字符串
            QString nodeInfo = "Node Info:";
            nodeInfo +="\n";
            nodeInfo += "Selected Items: ";
            for (int itemId : currentNode->itemsnode) {
                nodeInfo += QString::number(itemId) + " ";
            }
            nodeInfo += "\nTotal Weight (cw): " + QString::number(currentNode->cw);
            nodeInfo += "\nTotal Value (cv): " + QString::number(currentNode->cv);

            // 将格式化的字符串添加到 QTextEdit 控件中
            staEdit->append(nodeInfo);
            staEdit->append(QString(""));
        }
    }
}

int calculateCurrentNodeTotalValue(node* current) {
    if (!current) {
        return 0; // 如果当前节点为空，返回0
    }

    int totalValue = 0; // 初始化总价值为0
    for (int itemIndex : current->itemsnode) {
        // 遍历当前节点所选商品的编号
        if (itemIndex > 0 && itemIndex <= num) { // 确保商品编号在有效范围内
            totalValue += items[itemIndex - 1].value; // 累加商品价值，注意数组是从0开始的
        }
    }
    return totalValue; // 返回计算出的总价值
}

void MainWindow::on_onestepButton_clicked() {
    steps++;
    currentNode=sta.front();
    if (!currentNode) {
        QMessageBox::warning(this, "Finish", "已停机，无法继续执行！");
        return;
    }
    outputsta.push_back(currentNode);

    int temp=calculateCurrentNodeTotalValue(currentNode);
    if(curbestv<temp){
        curbestv=temp;
        QLineEdit *cbvEdit=findChild<QLineEdit*>("cbvEdit");
        if (cbvEdit) {
            cbvEdit->clear();
            cbvEdit->setText(QString::number(curbestv));
        } else {
            QMessageBox::warning(this, "Error", "当前最大价值输出控件未找到！");
        }

        QTextEdit *cbiEdit = findChild<QTextEdit*>("cbiEdit");
        if (cbiEdit) {
            // 清空之前的输出
            cbiEdit->clear();

            // 遍历当前节点选择的商品编号，并添加到输出字符串中
            QString itemsString;
            for (int itemIndex : currentNode->itemsnode) {
                itemsString += QString::number(itemIndex) + " "; // 将商品编号转换为字符串并用空格分隔
            }

            // 设置QTextEdit控件的内容
            cbiEdit->append("商品编号:\n" + itemsString);
        } else {
            QMessageBox::warning(this, "Error", "当前最佳商品序号输出控件未找到！");
        }

    }

    QTextEdit *staEdit = findChild<QTextEdit*>("staEdit"); // 假设控件的objectName是"itemsEdit"
    if (!staEdit) {
        QMessageBox::warning(this, "Error", "商品信息输出控件未找到！");
        return;
    }
    outputQueueToStaEdit(staEdit);
    updateUIWithCurrentNode(currentNode);
    sta.pop();
    if(sta.empty()){
        QLineEdit *stateEdit = findChild<QLineEdit*>("stateEdit");
        if (stateEdit) {
            stateEdit->clear();
            stateEdit->setText(QString("Halt!"));
        } else {
            QMessageBox::warning(this, "Error", "状态输出控件未找到！");
        }
    }
}

