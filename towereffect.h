#ifndef TOWEREFFECT_H
#define TOWEREFFECT_H

#include <QGraphicsScene>
#include <QPointF>

class TowerDestroyEffect
{
public:
    // Play a destruction effect at the given position, then clean up
    static void play(QPointF pos, QGraphicsScene *scene);
};

#endif // TOWEREFFECT_H
