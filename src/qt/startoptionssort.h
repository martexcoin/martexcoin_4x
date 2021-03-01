//
// Created by Kolby on 6/19/2019.
//

#include <QListView>
#include <QLabel>
#include <QWidget>
#include <QGraphicsRectItem>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QPainter>
#include <QListWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QHBoxLayout>
#include <QKeyEvent>

class QDragEnterEvent;
class QDropEvent;

namespace Ui {
    class StartOptionsSort;
}

/** Dialog to ask for passphrases. Used for encryption only
 */
class StartOptionsSort : public QWidget {
    Q_OBJECT

public:
    explicit StartOptionsSort(std::vector<std::string> Words, int rows, QWidget *parent = nullptr);
    ~StartOptionsSort();
    std::list<QString> getOrderedStrings();
    void keyPressEvent(QKeyEvent  *);


private:
    Ui::StartOptionsSort *ui;
    std::list<QListWidget*> labelsList;
    std::list<QGraphicsRectItem*> graphicsList;
    QString m_text;
    QGraphicsView* view;
    QGraphicsScene* scene;

};

class CustomRectItem : public QGraphicsRectItem
{
public:
    CustomRectItem(QGraphicsItem * parent = 0);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setText(const QString& text);
    QString text() const {return m_text;}
private:
    QString m_text;
};