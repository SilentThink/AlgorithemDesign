// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <vector>
#include <stack>
#include "tree.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_initializeButton_clicked();
    void on_executeButton_clicked();
    void on_onestepButton_clicked();
    void updateUIWithCurrentNode(node* currentNode);

private:
    Ui::MainWindow *ui;
    node* currentNode;
    // 其他成员变量和函数声明
};

#endif // MAINWINDOW_H
