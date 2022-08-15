#ifndef __TREEWIDGET_H
#define __TREEWIDGET_H

#include <QTreeWidget>
class CTreeWidget : public QTreeWidget
{
    Q_OBJECT;
public:
    CTreeWidget( QWidget * parent = nullptr );
    ~CTreeWidget();
private:

};

#endif
