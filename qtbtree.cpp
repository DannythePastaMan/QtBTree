#include "qtbtree.h"
#include<QtWidgets/qmessagebox.h>

#pragma execution_character_set("utf-8")
QtBtree::QtBtree(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    QPixmap bkgnd("/home/daniel/Desktop/Wallpapers/Project.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    connect(ui.exit, SIGNAL(released()), this, SLOT(close()));
    connect(ui.NewTree, SIGNAL(released()), this, SLOT(NewBtree()));
    connect(ui.Insdel, SIGNAL(released()), this, SLOT(InsDel()));
}

void QtBtree::NewBtree()
{

    nbdlg = new NewBtreeDLG(this);

    if (nbdlg->exec() != QDialog::Accepted)
    {
        return;
    }

    if (bt != nullptr)
    {
        delete bt;
    }

    bt = new BTree<int>(nbdlg->getM(), nullptr);
    vector<int> keys = nbdlg->getKeys();
    int num = nbdlg->getNum();

    for (int i = 0; i < num;i++)
    {
        bt->Insert(keys[i]);
    }
    if (da != nullptr)
    {
        delete da;
    }

    da = new DrawAll;
    da->Create(bt);
    ok = true;
    update();
    ui.Insdel->setEnabled(true);
}

void QtBtree::InsDel()
{
    InsAndDelDLG dlg;
    if (dlg.exec() != QDialog::Accepted)
    {
        return;
    }

    if (dlg.ui.radioButton_insert->isChecked())
    {
        int key = dlg.ui.lineEdit_keys->text().toInt();

        if (bt->Search(key).tag == true)
        {
            QMessageBox::warning(this, "Warning", "El nodo ya existe!");
            return;
        }

        bt->Insert(key);

        if (da != nullptr)
        {
            delete da;
        }

        da = new DrawAll;
        da->Create(bt);
        ok = true;
        update();
        return;
    }

    if (dlg.ui.radioButton_delete->isChecked())
    {
        int key = dlg.ui.lineEdit_keys->text().toInt();
        if (bt->Search(key).tag == false)//not exists
        {
            QMessageBox::warning(this, "warning", "the key does not exist");
            return;
        }
        bt->Delete(key);
        if (da != nullptr) delete da;
        da = new DrawAll;
        da->Create(bt);
        ok = true;
        update();
        return;
    }
}

void QtBtree::paintEvent(QPaintEvent*)
{
    if (!ok)
    {
        return;
    }

    QBrush mb(QColor(102, 0, 0));
    da->p = new QPainter(this);
    da->p->setBrush(mb);
    da->pt = QPoint(width()/2-50, 100);
    da->draw();
}

QtBtree::~QtBtree()
{
    if (bt != nullptr)
    delete bt;
    if (da != nullptr)
    delete da;
}

InsAndDelDLG::InsAndDelDLG(QWidget *parent)
:QDialog(parent)
{
    ui.setupUi(this);
}

InsAndDelDLG::~InsAndDelDLG()
{

}

NewBtreeDLG::NewBtreeDLG(QWidget *parent)
:QDialog(parent)
{
    ui.setupUi(this);
    ui.m_lineEdit->setValidator(new QRegExpValidator(QRegExp("[3-6]")));
    ui.lineEdit_num->setValidator(new QRegExpValidator(QRegExp("[0-9]{1,2}")));
}

int NewBtreeDLG::getM()
{

    if (ui.m_lineEdit->text().isEmpty())
        return 0;
    return ui.m_lineEdit->text().toInt();
}

int NewBtreeDLG::getNum()
{
    return ui.lineEdit_num->text().toInt();

}

vector<int> NewBtreeDLG::getKeys()
{
    QStringList sl=ui.TextEdit_v->toPlainText().split(',');
    vector<int> keys;
    for (int i = 0; i < sl.length(); i++)
        keys.push_back(sl[i].toInt());
    return keys;

}

NewBtreeDLG::~NewBtreeDLG()
{

}

DrawNode::DrawNode()
{
}

int DrawNode::getKeysNum()
{
    return keys.size();
}

void DrawNode::draw()
{
    if (parent.x() != 0 && parent.y() != 0)
    p->drawLine(parent, QPoint(pt.x()+w*keys.size()/2, pt.y()));
    for (int i = 0; i < keys.size(); i++)
    {
        p->drawRect(pt.x() + i*w, pt.y(), w, h);
        p->drawText(pt.x() + i*w + 5, pt.y()+12, QString("%1").arg(keys[i]));
    }
}

QPoint DrawNode::getPoint(int i)
{
    return QPoint(pt.x() + i*w, pt.y() + h);
}

DrawRow::DrawRow()
{
    this->pt = pt;
}

void DrawRow::draw()
{
    int xx = this->pt.x();
    for (int i = 0; i < rowNodes.size(); i++)
    {
        if (parentRow != nullptr)
        rowNodes[i].parent=parentRow->getParentPoint(i);
        else
        {
            rowNodes[i].parent = QPoint(0, 0);
        }
        rowNodes[i].p = p;
        rowNodes[i].pt.setX(xx);
        xx += rowNodes[i].getKeysNum()*DrawNode::w + d;
        rowNodes[i].pt.setY( this->pt.y());
        rowNodes[i].draw();
    }
}
void DrawRow::getNode(DrawNode dn)
{
    rowNodes.push_back(dn);
}

DrawAll::DrawAll()
{
}

void DrawAll::getRow(DrawRow dr)
{
    rows.push_back(dr);
}

void DrawAll::draw()
{
    for (int i = 0; i < rows.size(); i++)
    {
        if (i>0)
        {
            rows[i].parentRow =& rows[i - 1];
        }

        else
        {

            rows[0].parentRow = nullptr;
        }
        int Len = rows[i].GetLen();
        rows[i].p = p;
        rows[i].pt.setX(this->pt.x()+rows[0].GetLen()/2-Len/2);
        rows[i].pt.setY(this->pt.y() + (DrawNode::h+d)*i);
        rows[i].draw();
    }
}

void DrawAll::Create(BTree<int>* bt)
{
    QString allNodes = bt->SaveBTree();
    QStringList oneNode = allNodes.split('/', QString::SkipEmptyParts);
    for (int i = 0; i < oneNode.size(); i++)
    {
        if (isNeedNewRow())
        {
            DrawNode dn;
            DrawRow dr;
            QStringList oneKey = oneNode[i].split('+', QString::SkipEmptyParts);
            for (int j = 0; j < oneKey.size(); j++)
            {
                dn.keys.push_back(oneKey[j].toInt());
            }
            dr.getNode(dn);
            rows.push_back(dr);
        }
        else
        {
            DrawNode dn;
            QStringList oneKey = oneNode[i].split('+', QString::SkipEmptyParts);
            for (int j = 0; j < oneKey.size(); j++)
            {
                dn.keys.push_back(oneKey[j].toInt());
            }
            int r = rows.size() - 1;
            rows[r].rowNodes.push_back(dn);
        }

    }
}

bool DrawAll::isNeedNewRow()
{
    if (rows.size() == 0 || rows.size() == 1) return true;
    int LastChildSum=0;
    int LastRowNodeNum = rows[rows.size() - 2].rowNodes.size();
    for (int i = 0; i < LastRowNodeNum; i++)
    {
        LastChildSum +=( rows[rows.size() - 2].rowNodes[i].getKeysNum() + 1);
    }
    if (rows[rows.size() - 1].rowNodes.size() == LastChildSum)
        return true;
    return false;
}


int DrawRow::GetLen()
{
    int Len = 0;
    for (int i = 0; i < rowNodes.size(); i++)
    {
        int n = rowNodes[i].getKeysNum();
        Len += n*DrawNode::w;
    }
    Len += (rowNodes.size() - 1)*d;
    return Len;
}

QPoint DrawRow::getParentPoint(int index)
{
    int child = 0;
    for (int i = 0; i < rowNodes.size(); i++)
    {
        for (int j = 0; j < rowNodes[i].getKeysNum(); j++)
        {

            if (child == index)
            {
                QPoint pt = rowNodes[i].pt;
                pt.setX(pt.x() + DrawNode::w*j);
                pt.setY(pt.y() + DrawNode::h);
                return pt;
            }
            child++;
        }

        if (child == index)
        {
            QPoint pt = rowNodes[i].pt;
            pt.setX(pt.x() + DrawNode::w* rowNodes[i].getKeysNum());
            pt.setY(pt.y() + DrawNode::h);
            return pt;
        }
        child++;
    }
    return QPoint(0, 0);

}

void NewBtreeDLG::accept()
{
    QRegExp checkReg(QString("^(\\d{1,2},){%1}\\d{1,2}$").arg(getNum()-1));
    QString keys = ui.TextEdit_v->toPlainText();
    if (getM()==0||checkReg.indexIn(keys) == -1)
    {
        QMessageBox::warning(this, "Warning", "No keys");
        return;
    }
    QDialog::accept();
}
