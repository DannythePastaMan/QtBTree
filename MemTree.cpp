#include <queue>
#include<QString>
using namespace std;

template <typename temp>
struct Node
{
    int key_amount;//Cantidad de keys
    temp *Keys; //Matriz de Keys
    Node<temp> *parent;
    Node<temp> **secondary;//node secundario

    Node(int n, int m, Node<temp> *p)
    {
        key_amount = n;
        parent = p;
        Keys = new temp[m + 1];//Hay hasta m-1 palabras clave, K[0] no se usa, K[m] se usa como centinela
        secondary = new Node *[m + 1];//Mayoria de ramas, A[m] se utiliza como nodo centinela
        for (int i = 0; i <= m; i++)
        {
            secondary[i] = NULL;
        }
    }

    ~Node()
    {
        delete[] Keys;
        Keys = NULL;
        delete[] secondary;
        secondary = NULL;
    }
};


// Resultados de búsqueda de la triple definición
template <typename temp>
struct Triple
{
    Node<temp> * node;//Nodo donde se encuentra la key
    int i;//Posicion de subindice para las palabras clave
    bool tag;//Booleano para ver si la busqueda fue exitosa

    Triple(Node<temp> *nd, int pos, bool t)
    {
        node = nd;
        i = pos;
        tag = t;
    }
};

template <typename temp>
class BTree
{
public:
    BTree(int m, Node<temp> * root);
    ~BTree();
    Triple<temp> Search(const temp& x);
    bool Insert(const temp& x);
    bool Delete(const temp& x);
    void InsertKey(Node<temp> *p, temp k, Node<temp> *a, int i);
    void SpliteNode(Node<temp> *p, temp *k, Node<temp> **a, int i);
    void RightAdjust(Node<temp> *p, Node<temp> *q, int i);
    void LeftAdjust(Node<temp> *p, Node<temp> *q, int i);
    void LeftCompress(Node<temp> *p, int i);
    void RightCompress(Node<temp> *p, int i);
    void MergeNode(Node<temp> *p, Node<temp> *q, Node<temp> *pR, int i);
    QString SaveBTree();

private:
    int m_m;
    Node<temp> *m_pRoot;
};

template<typename temp>
BTree<temp>::BTree(int m, Node<temp> * root)
{
    m_m = m;
    m_pRoot = root;
}

template<typename temp>
BTree<temp>::~BTree()
{
    if (m_pRoot != NULL)
    {
        queue<Node<temp> *> nodeQueue;
        nodeQueue.push(m_pRoot);
        while (nodeQueue.size())
        {
            Node<temp> * p = nodeQueue.front();
            if (p->secondary[0] != NULL)
            {
                for (int i = 0; i <= p->key_amount; i++)
                {
                    nodeQueue.push(p->secondary[i]);
                }
            }

            nodeQueue.pop();
            delete p;
            p = NULL;
        }
    }
}

template <typename temp>
Triple<temp> BTree<temp>::Search(const temp &x)
{
    int i = 0;
    Node<temp> *p = m_pRoot;
    Node<temp> *q = NULL;

    while (p != NULL)
    {

        int n = p->key_amount;
        for (i = 0; i < n && x >= p->Keys[i + 1]; i++);
        if (x == p->Keys[i])
        {
            return Triple<temp>(p, i, true);
        }

        q = p;
        p = p->secondary[i];
    }
    return Triple<temp>(q, i, false);
}


template <typename temp>
void BTree<temp>::InsertKey(Node<temp> *p, temp k, Node<temp> *a, int i)
{
    for (int j = p->key_amount; j > i; j--)
    {
        p->Keys[j + 1] = p->Keys[j];
        p->secondary[j + 1] = p->secondary[j];
    }
    p->key_amount++;
    p->Keys[i + 1] = k;
    p->secondary[i + 1] = a;
    if (a != NULL)
    {
        a->parent = p;
    }
}

template <typename temp>
void BTree<temp>::SpliteNode(Node<temp> *p, temp *k, Node<temp> **a, int i)
{
    InsertKey(p, *k, *a, i);
    int mid = (m_m + 1) / 2;
    int size = (m_m & 1) ? mid : mid + 1;

    Node<temp> *q = new Node<temp>(0, m_m, p->parent);
    q->secondary[0] = p->secondary[mid];
    for (int j = 1; j < size; j++)
    {
        q->Keys[j] = p->Keys[mid + j];
        q->secondary[j] = p->secondary[mid + j];
    }

    if (q->Keys[0] != NULL)
    {
        for (int j = 0; j > size; j++)
            q->secondary[j]->parent = q;
    }

    q->key_amount = m_m - mid;
    p->key_amount = mid - 1;

    *k = p->Keys[mid];
    *a = q;
}

template <typename temp>
bool BTree<temp>::Insert(const temp &x){

    if (m_pRoot == NULL)
    {
        m_pRoot = new Node<temp>(1, m_m, NULL);
        m_pRoot->Keys[1] = x;
        return true;
    }

    Triple<temp> triple = Search(x);
    if (triple.tag == true)
    {
        return false;
    }

    Node<temp> *p = triple.node, *q;

    Node<temp> *a = NULL;
    temp k = x;
    int i = triple.i;

    while (1)
    {
        if (p->key_amount < m_m - 1)
        {
            InsertKey(p, k, a, i);
            return true;
        }

        SpliteNode(p, &k, &a, i);
        if (p->parent != NULL)
        {
            q = p->parent;
            for (i = 0; i < q->key_amount && x >= q->Keys[i + 1]; i++)
                ;
            p = q;
        }
        else
        {

            m_pRoot = new Node<temp>(1, m_m, NULL);
            m_pRoot->Keys[1] = k;
            m_pRoot->secondary[0] = p;
            m_pRoot->secondary[1] = a;
            p->parent = a->parent = m_pRoot;
            return true;
        }
    }
}

template <typename temp>
bool BTree<temp>::Delete(const temp& x)
{
    Triple<temp> triple = Search(x);
    if (triple.tag == false)
    {
        return false;
    }

    Node<temp> *p = triple.node, *q;
    int i = triple.i;

    if (p->secondary[i] != NULL)
    {
        q = p->secondary[i];
        while (q->secondary[0] != NULL)
        {
            q = q->secondary[0];
        }

        p->Keys[i] = q->Keys[1];
        LeftCompress(q, 1);
        p = q;
    }

    else
    {
        LeftCompress(p, i);
    }

    int mid = (m_m + 1) / 2;
    while (1)
    {
        if (p == m_pRoot || p->key_amount >= mid - 1)
        {
            break;
        }

        else
        {
            q = p->parent;
            for (i = 0; i <= q->key_amount && q->secondary[i] != p; i++);

            if (i == 0)
            {
                RightAdjust(p, q, i);
            }

            else
            {
                LeftAdjust(p, q, i);
            }

            p = q;
        }
    }
    if (m_pRoot->key_amount == 0)
    {
        p = m_pRoot->secondary[0];
        delete m_pRoot;
        m_pRoot = p;
        if (m_pRoot != NULL)
            m_pRoot->parent = NULL;
    }
    return true;
}

template <typename temp>
void BTree<temp>::RightAdjust(Node<temp> *p, Node<temp> *q, int i)
{
    Node<temp> *pR = q->secondary[i + 1];
    if (pR->key_amount >= (m_m + 1) / 2)
    {

        p->key_amount++;
        p->Keys[p->key_amount] = q->Keys[i + 1];
        p->secondary[p->key_amount] = pR->secondary[0];
        if (p->secondary[p->key_amount] != NULL)
            p->secondary[p->key_amount]->parent = p;

        q->Keys[i + 1] = pR->Keys[1];
        pR->secondary[0] = pR->secondary[1];
        LeftCompress(pR, 1);
    }
    else
    {
        MergeNode(p, q, pR, i + 1);
    }
}

template <typename temp>
void BTree<temp>::LeftAdjust(Node<temp> *p, Node<temp> *q, int i)
{
    Node<temp> *pL = q->secondary[i - 1];
    if (pL->key_amount >= (m_m + 1) / 2)
    {

        RightCompress(p, 1);
        p->secondary[1] = p->secondary[0];
        p->Keys[1] = q->Keys[i];
        p->secondary[0] = pL->secondary[pL->key_amount];

        if (p->secondary[0] != NULL)
        {
            p->secondary[0]->parent = p;
        }

        q->Keys[i] = pL->Keys[pL->key_amount];

        pL->key_amount--;
    }
    else
    {

        Node<temp> *pR = p;
        p = pL;
        MergeNode(p, q, pR, i);
    }
}

template <typename temp>
void BTree<temp>::LeftCompress(Node<temp> *p, int i)
{
    int n = p->key_amount;
    for (int j = i; j < n; j++)
    {
        p->Keys[j] = p->Keys[j + 1];
        p->secondary[j] = p->secondary[j + 1];
    }
    p->key_amount--;
}

template <typename temp>
void BTree<temp>::RightCompress(Node<temp> *p, int i)
{
    for (int j = p->key_amount; j >= i; j--)
    {
        p->Keys[j + 1] = p->Keys[j];
        p->secondary[j + 1] = p->secondary[j];
    }
    p->key_amount++;
}

template <typename temp>
void BTree<temp>::MergeNode(Node<temp> *p, Node<temp> *q, Node<temp> *pR, int i)
{
    int n = p->key_amount + 1;
    p->Keys[n] = q->Keys[i];
    p->secondary[n] = pR->secondary[0];
    for (int j = 1; j <= pR->key_amount; j++)
    {
        p->Keys[n + j] = pR->Keys[j];
        p->secondary[n + j] = pR->secondary[j];
    }
    if (p->secondary[0])
    {
        for (int j = 0; j <= pR->key_amount; j++)
            p->secondary[n + j]->parent = p;
    }
    LeftCompress(q, i);
    p->key_amount = p->key_amount + pR->key_amount + 1;
    delete pR;
    pR = NULL;
}

template <typename temp>
QString BTree<temp>::SaveBTree()
{
    QString nodes;
    if (m_pRoot != NULL)
    {
        queue<Node<temp> *> nodeQueue;
        nodeQueue.push(m_pRoot);
        while (nodeQueue.size())
        {
            Node<temp> * p = nodeQueue.front();
            if (p->secondary[0] != NULL)
            {
                nodeQueue.push(p->secondary[0]);
                for (int i = 1; i <= p->key_amount; i++)
                {
                    nodeQueue.push(p->secondary[i]);
                    nodes.push_back(QString("%1+").arg(p->Keys[i]));
                }
            }
            else
            {
                for (int i = 1; i <= p->key_amount; i++)
                    nodes.push_back(QString("%1+").arg(p->Keys[i]));
            }

            nodes .push_back('/');
            nodeQueue.pop();
        }
    }
    return nodes;
}
