#include "towereffect.h"
#include <QGraphicsEllipseItem>
#include <QTimer>
#include <QtMath>

static constexpr qreal PI = 3.1415926535;
static constexpr int EFFECT_DURATION = 600; // ms
static constexpr int TICK_MS = 16;
static constexpr int PARTICLE_COUNT = 10;

void TowerDestroyEffect::play(QPointF pos, QGraphicsScene *scene)
{
    // Shockwave ring
    auto *ring = scene->addEllipse(QRectF(-5, -5, 10, 10),
                                    QPen(QColor(255, 60, 30), 4),
                                    QBrush(Qt::NoBrush));
    ring->setPos(pos);
    ring->setZValue(100);

    // Inner flash
    auto *flash = scene->addEllipse(QRectF(-15, -15, 30, 30),
                                     QPen(Qt::NoPen),
                                     QBrush(QColor(255, 200, 50, 200)));
    flash->setPos(pos);
    flash->setZValue(99);

    // Particles
    QVector<QGraphicsEllipseItem*> particles;
    for (int i = 0; i < PARTICLE_COUNT; ++i) {
        auto *p = scene->addEllipse(QRectF(-4, -4, 8, 8),
                                     QPen(Qt::NoPen),
                                     QBrush(QColor(255, 100, 20)));
        p->setPos(pos);
        p->setZValue(101);
        particles.append(p);
    }

    // Debris lines
    QVector<QGraphicsLineItem*> debris;
    for (int i = 0; i < 6; ++i) {
        qreal angle = 2.0 * PI * i / 6.0;
        QLineF ln(0, 0, qCos(angle) * 8, qSin(angle) * 8);
        auto *line = scene->addLine(ln, QPen(QColor(255, 180, 50), 2));
        line->setPos(pos);
        line->setZValue(101);
        debris.append(line);
    }

    // Animation state on heap, cleaned up when timer stops
    auto *elapsed = new int(0);
    auto *timer = new QTimer(scene);
    timer->setInterval(TICK_MS);

    QObject::connect(timer, &QTimer::timeout, [=]() mutable {
        *elapsed += TICK_MS;
        qreal t = qMin(*elapsed / qreal(EFFECT_DURATION), 1.0);

        if (t >= 1.0) {
            timer->stop();
            scene->removeItem(ring);
            scene->removeItem(flash);
            delete ring;
            delete flash;
            for (auto *p : particles) {
                scene->removeItem(p);
                delete p;
            }
            for (auto *d : debris) {
                scene->removeItem(d);
                delete d;
            }
            timer->deleteLater();
            delete elapsed;
            return;
        }

        // Ring expands and fades
        qreal ringSize = 5 + t * 80;
        int alpha = (int)((1.0 - t) * 220);
        ring->setPen(QPen(QColor(255, 60, 30, alpha), 4 * (1.0 - t)));
        ring->setRect(QRectF(-ringSize, -ringSize, ringSize * 2, ringSize * 2));

        // Flash fades
        flash->setBrush(QColor(255, 200, 50, (int)(200 * (1.0 - t))));
        qreal flashScale = 1.0 + t * 1.5;
        flash->setRect(QRectF(-15 * flashScale, -15 * flashScale,
                               30 * flashScale, 30 * flashScale));

        // Particles fly outward and shrink
        for (int i = 0; i < particles.size(); ++i) {
            qreal angle = 2.0 * PI * i / particles.size();
            qreal dist = t * 55;
            QPointF ppos = pos + QPointF(qCos(angle) * dist, qSin(angle) * dist);
            particles[i]->setPos(ppos);
            qreal pr = 4 * (1.0 - t);
            particles[i]->setRect(QRectF(-pr, -pr, pr * 2, pr * 2));
            particles[i]->setBrush(QColor(255, 100 + (int)(t * 155), 20, (int)((1.0 - t) * 255)));
        }

        // Debris rotates and flies outward, fades
        for (int i = 0; i < debris.size(); ++i) {
            qreal angle = 2.0 * PI * i / debris.size() + t * 3.0;
            qreal dist = t * 40;
            QPointF dpos = pos + QPointF(qCos(angle) * dist, qSin(angle) * dist);
            debris[i]->setPos(dpos);
            qreal rot = t * 180;
            debris[i]->setRotation(rot);
            int da = (int)((1.0 - t) * 255);
            debris[i]->setPen(QPen(QColor(255, 180, 50, da), 2 * (1.0 - t)));
        }
    });

    timer->start();
}
