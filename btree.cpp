#pragma once
#include <queue>
#include<qstring.h>
using namespace std;

template <typename T>
struct BTreeNode
{
    int num;
    T *K;
    BTreeNode<T> *parent;
    BTreeNode<T> **A;
    BTreeNode(int n, int m, BTreeNode<T>  *p)
    {
        num = n;
        parent = p;
        K = new T[m + 1];
        A = new BTreeNode *[m + 1];
        for (int i = 0; i <= m; i++)
            A[i] = NULL;
    }
    ~BTreeNode()
    {
        delete[] K; K = NULL;
        delete[] A; A = NULL;
    }
};

template <typename T>
struct Triple
{
    BTreeNode<T> * node;
    int i;
    bool tag;
    Triple(BTreeNode<T> *nd, int pos, bool t)
    {
        node = nd; i = pos; tag = t;
    }
};

template <typename T>
class BTree
{
public:
    BTree(int m, BTreeNode<T> * root);
    ~BTree();
    Triple<T> Search(const T& x);
    bool Insert(const T& x);
    bool Delete(const T& x);
    void InsertKey(BTreeNode<T> *p, T k, BTreeNode<T> *a, int i);
    void SpliteNode(BTreeNode<T> *p, T *k, BTreeNode<T> **a, int i);
    void RightAdjust(BTreeNode<T> *p, BTreeNode<T> *q, int i);
    void LeftAdjust(BTreeNode<T> *p, BTreeNode<T> *q, int i);
    void LeftCompress(BTreeNode<T> *p, int i);
    void RightCompress(BTreeNode<T> *p, int i);
    void MergeNode(BTreeNode<T> *p, BTreeNode<T> *q, BTreeNode<T> *pR, int i);
    QString SaveBTree();

private:
    int m_m;
    BTreeNode<T> *m_pRoot;
};

template<typename T>
BTree<T>::BTree(int m, BTreeNode<T> * root)
{
    m_m = m;
    m_pRoot = root;
}

template<typename T>
BTree<T>::~BTree()
{
    if (m_pRoot != NULL)
    {
        queue<BTreeNode<T> *> nodeQueue;
        nodeQueue.push(m_pRoot);
        while (nodeQueue.size())
        {
            BTreeNode<T> * p = nodeQueue.front();
            if (p->A[0] != NULL)
            {
                for (int i = 0; i <= p->num; i++)
                    nodeQueue.push(p->A[i]);
            }
            nodeQueue.pop();
            delete p;
            p = NULL;
        }
    }
}

template <typename T>
Triple<T> BTree<T>::Search(const T &x)
{
    int i = 0;
    BTreeNode<T> *p = m_pRoot, *q = NULL;

    while (p != NULL)
    {

        int n = p->num;
        for (i = 0; i < n && x >= p->K[i + 1]; i++)
            ;
        if (x == p->K[i])
            return Triple<T>(p, i, true);
        q = p;
        p = p->A[i];
    }
    return Triple<T>(q, i, false);
}

template <typename T>
bool BTree<T>::Insert(const T &x){

    if (m_pRoot == NULL)
    {
        m_pRoot = new BTreeNode<T>(1, m_m, NULL);
        m_pRoot->K[1] = x;
        return true;
    }

    Triple<T> triple = Search(x);
    if (triple.tag == true)
        return false;

    BTreeNode<T> *p = triple.node, *q;

    BTreeNode<T> *a = NULL;
    T k = x;
    int i = triple.i;

    while (1)
    {
        if (p->num < m_m - 1)
        {
            InsertKey(p, k, a, i);
            return true;
        }
        SpliteNode(p, &k, &a, i);
        if (p->parent != NULL)
        {
            q = p->parent;
            for (i = 0; i < q->num && x >= q->K[i + 1]; i++)
                ;
            p = q;
        }
        else
        {

            m_pRoot = new BTreeNode<T>(1, m_m, NULL);
            m_pRoot->K[1] = k;
            m_pRoot->A[0] = p;
            m_pRoot->A[1] = a;
            p->parent = a->parent = m_pRoot;
            return true;
        }
    }
}

template <typename T>
void BTree<T>::InsertKey(BTreeNode<T> *p, T k, BTreeNode<T> *a, int i)
{
    for (int j = p->num; j > i; j--)
    {
        p->K[j + 1] = p->K[j];
        p->A[j + 1] = p->A[j];
    }
    p->num++;
    p->K[i + 1] = k;
    p->A[i + 1] = a;
    if (a != NULL)
        a->parent = p;
}

template <typename T>
void BTree<T>::SpliteNode(BTreeNode<T> *p, T *k, BTreeNode<T> **a, int i)
{
    InsertKey(p, *k, *a, i);
    int mid = (m_m + 1) / 2;
    int size = (m_m & 1) ? mid : mid + 1;

    BTreeNode<T> *q = new BTreeNode<T>(0, m_m, p->parent);
    q->A[0] = p->A[mid];
    for (int j = 1; j < size; j++)
    {
        q->K[j] = p->K[mid + j];
        q->A[j] = p->A[mid + j];
    }

    if (q->A[0] != NULL)
    {
        for (int j = 0; j < size; j++)
            q->A[j]->parent = q;
    }

    q->num = m_m - mid;
    p->num = mid - 1;

    *k = p->K[mid];
    *a = q;
}


template <typename T>
bool BTree<T>::Delete(const T& x)
{
    Triple<T> triple = Search(x);
    if (triple.tag == false)
        return false;
    BTreeNode<T> *p = triple.node, *q;
    int i = triple.i;

    if (p->A[i] != NULL)
    {
        q = p->A[i];
        while (q->A[0] != NULL)
            q = q->A[0];
        p->K[i] = q->K[1];
        LeftCompress(q, 1);
        p = q;
    }
    else
        LeftCompress(p, i);
    int mid = (m_m + 1) / 2;
    while (1)
    {
        if (p == m_pRoot || p->num >= mid - 1)
            break;
        else
        {
            q = p->parent;
            for (i = 0; i <= q->num && q->A[i] != p; i++)
                ;
            if (i == 0)
                RightAdjust(p, q, i);
            else
                LeftAdjust(p, q, i);
            p = q;
        }
    }
    if (m_pRoot->num == 0)
    {
        p = m_pRoot->A[0];
        delete m_pRoot;
        m_pRoot = p;
        if (m_pRoot != NULL)
            m_pRoot->parent = NULL;
    }
    return true;
}

template <typename T>
void BTree<T>::RightAdjust(BTreeNode<T> *p, BTreeNode<T> *q, int i)
{
    BTreeNode<T> *pR = q->A[i + 1];
    if (pR->num >= (m_m + 1) / 2)
    {

        p->num++;
        p->K[p->num] = q->K[i + 1];
        p->A[p->num] = pR->A[0];
        if (p->A[p->num] != NULL)
            p->A[p->num]->parent = p;

        q->K[i + 1] = pR->K[1];
        pR->A[0] = pR->A[1];
        LeftCompress(pR, 1);
    }
    else
        MergeNode(p, q, pR, i + 1);
}

template <typename T>
void BTree<T>::LeftAdjust(BTreeNode<T> *p, BTreeNode<T> *q, int i)
{
    BTreeNode<T> *pL = q->A[i - 1];
    if (pL->num >= (m_m + 1) / 2)
    {

        RightCompress(p, 1);
        p->A[1] = p->A[0];
        p->K[1] = q->K[i];
        p->A[0] = pL->A[pL->num];
        if (p->A[0] != NULL)
            p->A[0]->parent = p;

        q->K[i] = pL->K[pL->num];

        pL->num--;
    }
    else
    {

        BTreeNode<T> *pR = p;
        p = pL;
        MergeNode(p, q, pR, i);
    }
}

template <typename T>
void BTree<T>::LeftCompress(BTreeNode<T> *p, int i)
{
    int n = p->num;
    for (int j = i; j < n; j++)
    {
        p->K[j] = p->K[j + 1];
        p->A[j] = p->A[j + 1];
    }
    p->num--;
}

template <typename T>
void BTree<T>::RightCompress(BTreeNode<T> *p, int i)
{
    for (int j = p->num; j >= i; j--)
    {
        p->K[j + 1] = p->K[j];
        p->A[j + 1] = p->A[j];
    }
    p->num++;
}

template <typename T>
void BTree<T>::MergeNode(BTreeNode<T> *p, BTreeNode<T> *q, BTreeNode<T> *pR, int i)
{
    int n = p->num + 1;
    p->K[n] = q->K[i];
    p->A[n] = pR->A[0];
    for (int j = 1; j <= pR->num; j++)
    {
        p->K[n + j] = pR->K[j];
        p->A[n + j] = pR->A[j];
    }
    if (p->A[0])
    {
        for (int j = 0; j <= pR->num; j++)
            p->A[n + j]->parent = p;
    }
    LeftCompress(q, i);
    p->num = p->num + pR->num + 1;
    delete pR;
    pR = NULL;
}

template <typename T>
QString BTree<T>::SaveBTree()
{
    QString nodes;
    if (m_pRoot != NULL)
    {
        queue<BTreeNode<T> *> nodeQueue;
        nodeQueue.push(m_pRoot);
        while (nodeQueue.size())
        {
            BTreeNode<T> * p = nodeQueue.front();
            if (p->A[0] != NULL)
            {
                nodeQueue.push(p->A[0]);
                for (int i = 1; i <= p->num; i++)
                {
                    nodeQueue.push(p->A[i]);
                    nodes.push_back(QString("%1+").arg(p->K[i]));
                }
            }
            else
            {
                for (int i = 1; i <= p->num; i++)
                    nodes.push_back(QString("%1+").arg(p->K[i]));
            }

            nodes .push_back('/');
            nodeQueue.pop();
        }
    }
    return nodes;
}
