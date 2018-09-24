#ifndef QTBTREE_H
#define QTBTREE_H

#include <QtWidgets/QMainWindow>
#include<QtWidgets/QDialog>
#include<QtGui>
#include "ui_Tree.h"
#include "ui_newbtreedlg.h"
#include "ui_insanddelui.h"
#include "btree.cpp"
#include<vector>

class DrawAll;
class InsAndDelDLG :public QDialog
{
    Q_OBJECT
public:
    InsAndDelDLG(QWidget *parent = 0);
    ~InsAndDelDLG();
    Ui::Dialog ui;
};
class NewBtreeDLG :public QDialog
{
    Q_OBJECT
public:
    NewBtreeDLG(QWidget *parent = 0);
    ~NewBtreeDLG();
    int getM();
    int getNum();
    vector<int> getKeys();
private:
    Ui::NewBtreeDLG ui;
public:
    void accept();
};
class QtBtree : public QMainWindow
{
    Q_OBJECT

public:
    bool ok = false;
    QtBtree(QWidget *parent = nullptr);
    ~QtBtree();
    NewBtreeDLG* nbdlg;
    BTree<int>* bt = nullptr;
    DrawAll* da = nullptr;
    void paintEvent(QPaintEvent* event);
private:
    Ui::QtBtreeClass ui;
    private slots:
    void NewBtree();
    void InsDel();
};
class DrawNode
{
public:
    static const int w = 20;
    static const int h = 20;
    QPainter* p;
    QPoint pt;
    QPoint parent;
    DrawNode();
    void draw();
    vector<int>keys;
    QPoint getPoint(int i);
    int getKeysNum();
};
class DrawRow//管理一行节点
{
public:
    static const int d = 15;
    QPoint pt;
    QPainter* p;
    DrawRow* parentRow = nullptr;
    DrawRow();
    vector<DrawNode> rowNodes;
    void getNode(DrawNode dn);
    void draw();

    int GetLen();

    QPoint getParentPoint(int index);
};
class DrawAll
{
public:
    static const int d = 40;
    QPoint pt;
    QPainter* p;
    DrawAll();
    vector<DrawRow> rows;
    void getRow(DrawRow dr);
    void draw();
    void Create(BTree<int>* bt);
    bool isNeedNewRow();
};

#endif // QTBTREE_H
