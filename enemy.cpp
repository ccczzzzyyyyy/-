#include "enemy.h"
#include <QtMath>

Enemy::Enemy(EnemyType type, const QVector<QPointF> &waypoints, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_type(type), m_waypoints(waypoints)
{
    switch (type) {
    case EnemyType::Normal:
        m_maxHp = 150;
        m_hp = 150;
        m_speed = 70;
        m_reward = 30;
        m_color = QColor(255, 100, 100);
        m_radius = 14;
        m_livesCost = 1;
        break;
    case EnemyType::Fast:
        m_maxHp = 90;
        m_hp = 90;
        m_speed = 110;
        m_reward = 40;
        m_color = QColor(100, 180, 255);
        m_radius = 12;
        m_livesCost = 1;
        break;
    case EnemyType::Tank:
        m_maxHp = 450;
        m_hp = 450;
        m_speed = 45;
        m_reward = 70;
        m_color = QColor(180, 100, 255);
        m_radius = 18;
        m_livesCost = 2;
        break;
    case EnemyType::Boss:
        m_maxHp = 1200;
        m_hp = 1200;
        m_speed = 35;
        m_reward = 300;
        m_color = QColor(255, 50, 50);
        m_radius = 24;
        m_livesCost = 5;
        break;
    }

    if (!m_waypoints.isEmpty())
        setPos(m_waypoints[0]);
}

Enemy::~Enemy() {}

QRectF Enemy::boundingRect() const
{
    // Extra margin for boss aura / crown / fast trail
    qreal extra = 0;
    if (m_type == EnemyType::Boss)
        extra = 14;
    else if (m_type == EnemyType::Tank)
        extra = 4;
    qreal r = m_radius + extra + 2;
    qreal barTop = -m_radius - 8 - extra;
    return QRectF(-r, barTop, r * 2, r - barTop);
}

void Enemy::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    qreal r = m_radius;

    switch (m_type) {
    case EnemyType::Normal: {
        // --- Slime / blob shape ---
        QPainterPath body;
        // Squishy bottom: a wide ellipse, slightly flat on bottom
        body.moveTo(0, -r);
        body.cubicTo(r * 1.2, -r, r * 1.1, r * 0.6, r * 0.9, r * 0.8);
        body.cubicTo(r * 0.8, r * 1.1, -r * 0.8, r * 1.1, -r * 0.9, r * 0.8);
        body.cubicTo(-r * 1.1, r * 0.6, -r * 1.2, -r, 0, -r);

        QRadialGradient grad(0, -r * 0.3, r * 1.2);
        grad.setColorAt(0, m_color.lighter(160));
        grad.setColorAt(0.7, m_color);
        grad.setColorAt(1, m_color.darker(150));
        painter->setBrush(grad);
        painter->setPen(QPen(m_color.darker(180), 2));
        painter->drawPath(body);

        // Specular highlight
        painter->setBrush(QColor(255, 255, 255, 100));
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(QPointF(-r * 0.25, -r * 0.35), r * 0.35, r * 0.25);

        // Googly eyes
        painter->setBrush(Qt::white);
        painter->setPen(QPen(Qt::black, 1));
        painter->drawEllipse(QPointF(-r * 0.35, -r * 0.25), r * 0.32, r * 0.38);
        painter->drawEllipse(QPointF(r * 0.35, -r * 0.25), r * 0.32, r * 0.38);
        painter->setBrush(Qt::black);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(QPointF(-r * 0.28, -r * 0.2), r * 0.16, r * 0.18);
        painter->drawEllipse(QPointF(r * 0.42, -r * 0.2), r * 0.16, r * 0.18);

        // Small mouth
        painter->setPen(QPen(Qt::black, 1.2));
        painter->drawArc(QRectF(-r * 0.2, r * 0.05, r * 0.4, r * 0.3), 0 * 16, 180 * 16);
        break;
    }
    case EnemyType::Fast: {
        // --- Sleek teardrop / rocket shape ---
        QPainterPath body;
        qreal a = r * 1.1;  // horizontal radius
        qreal b = r;        // vertical radius
        // Teardrop pointing right (moving direction)
        body.moveTo(a, 0);
        body.cubicTo(a * 0.6, -b, -a * 0.5, -b * 0.7, -a * 0.6, 0);
        body.cubicTo(-a * 0.5, b * 0.7, a * 0.6, b, a, 0);

        QLinearGradient grad(-a, 0, a, 0);
        grad.setColorAt(0, m_color.darker(140));
        grad.setColorAt(0.5, m_color.lighter(130));
        grad.setColorAt(1, m_color.lighter(160));
        painter->setBrush(grad);
        painter->setPen(QPen(m_color.darker(180), 2));
        painter->drawPath(body);

        // Motion streaks behind
        painter->setPen(QPen(m_color.lighter(140), 1.5));
        for (int i = 0; i < 3; ++i) {
            qreal yOff = (i - 1) * r * 0.45;
            painter->drawLine(QPointF(-a * 0.7, yOff), QPointF(-a * 0.95, yOff));
        }

        // Specular highlight
        painter->setBrush(QColor(255, 255, 255, 100));
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(QPointF(-r * 0.15, -r * 0.3), r * 0.3, r * 0.2);

        // Slanted angry eyes
        painter->setPen(QPen(Qt::black, 1.2));
        painter->drawLine(QPointF(-r * 0.1, -r * 0.35), QPointF(r * 0.25, -r * 0.15));
        painter->drawLine(QPointF(-r * 0.1, -r * 0.15), QPointF(r * 0.25, -r * 0.35));
        break;
    }
    case EnemyType::Tank: {
        // --- Armored hexagon with shield ---
        int sides = 6;
        QPolygonF hex;
        for (int i = 0; i < sides; ++i) {
            qreal angle = 2.0 * 3.1415926535 * i / sides - 3.1415926535 / 2.0;
            hex << QPointF(r * qCos(angle), r * qSin(angle));
        }

        QLinearGradient grad(0, -r, 0, r);
        grad.setColorAt(0, m_color.lighter(130));
        grad.setColorAt(0.5, m_color);
        grad.setColorAt(1, m_color.darker(160));
        painter->setBrush(grad);
        painter->setPen(QPen(m_color.darker(200), 3));
        painter->drawPolygon(hex);

        // Inner shield emblem
        painter->setBrush(m_color.lighter(160));
        painter->setPen(QPen(m_color.darker(140), 1.5));
        painter->drawEllipse(QPointF(0, 0), r * 0.55, r * 0.55);

        // Cross on shield
        painter->setPen(QPen(m_color.darker(180), 2));
        painter->drawLine(QPointF(0, -r * 0.42), QPointF(0, r * 0.42));
        painter->drawLine(QPointF(-r * 0.42, 0), QPointF(r * 0.42, 0));

        // Bolts at corners
        painter->setBrush(QColor(60, 60, 60));
        painter->setPen(QPen(Qt::black, 1));
        for (int i = 0; i < sides; ++i) {
            qreal angle = 2.0 * 3.1415926535 * i / sides - 3.1415926535 / 2.0;
            QPointF bolt = QPointF(r * 0.78 * qCos(angle), r * 0.78 * qSin(angle));
            painter->drawEllipse(bolt, 2.5, 2.5);
        }

        // Angry eyes
        painter->setBrush(Qt::white);
        painter->setPen(QPen(Qt::black, 1.5));
        painter->drawEllipse(QPointF(-r * 0.28, -r * 0.22), r * 0.22, r * 0.18);
        painter->drawEllipse(QPointF(r * 0.28, -r * 0.22), r * 0.22, r * 0.18);
        painter->setBrush(Qt::red);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(QPointF(-r * 0.28, -r * 0.2), r * 0.12, r * 0.13);
        painter->drawEllipse(QPointF(r * 0.28, -r * 0.2), r * 0.12, r * 0.13);
        break;
    }
    case EnemyType::Boss: {
        // --- Skill aura glow (pulsing) ---
        if (m_hasSkill) {
            qreal auraPhase = qAbs(qSin(static_cast<qreal>(m_hp) / m_maxHp * 6.28));
            qreal auraR = r + 6 + auraPhase * 6;
            QRadialGradient auraGrad(0, 0, auraR);
            auraGrad.setColorAt(0, QColor(255, 100, 30, 0));
            auraGrad.setColorAt(0.75, QColor(255, 80, 20, (int)(40 + auraPhase * 60)));
            auraGrad.setColorAt(1, QColor(255, 50, 10, 0));
            painter->setBrush(auraGrad);
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(QPointF(0, 0), auraR, auraR);

            // Orbiting particles
            for (int i = 0; i < 4; ++i) {
                qreal angle = auraPhase * 3.14 + 2.0 * 3.1415926535 * i / 4.0;
                QPointF orb(auraR * 0.8 * qCos(angle), auraR * 0.8 * qSin(angle));
                painter->setBrush(QColor(255, 200, 50, 180));
                painter->drawEllipse(orb, 3, 3);
            }
        }

        // --- Spiked body ---
        int spikes = 8;
        QPainterPath body;
        for (int i = 0; i < spikes * 2; ++i) {
            qreal angle = 2.0 * 3.1415926535 * i / (spikes * 2) - 3.1415926535 / 2.0;
            qreal rad = (i % 2 == 0) ? r : r * 0.78;
            QPointF pt(rad * qCos(angle), rad * qSin(angle));
            if (i == 0)
                body.moveTo(pt);
            else
                body.lineTo(pt);
        }
        body.closeSubpath();

        QRadialGradient grad(-r * 0.2, -r * 0.3, r * 1.3);
        grad.setColorAt(0, QColor(255, 130, 60));
        grad.setColorAt(0.5, m_color);
        grad.setColorAt(1, QColor(120, 10, 10));
        painter->setBrush(grad);
        painter->setPen(QPen(QColor(80, 0, 0), 3));
        painter->drawPath(body);

        // --- Crown / horns ---
        painter->setBrush(QColor(255, 200, 50));
        painter->setPen(QPen(QColor(180, 130, 20), 2));
        QPolygonF crown;
        crown << QPointF(-r * 0.55, -r * 0.9)
              << QPointF(-r * 0.5, -r * 1.4)
              << QPointF(-r * 0.2, -r * 1.05)
              << QPointF(0, -r * 1.5)
              << QPointF(r * 0.2, -r * 1.05)
              << QPointF(r * 0.5, -r * 1.4)
              << QPointF(r * 0.55, -r * 0.9);
        painter->drawPolygon(crown);

        // Crown gems
        painter->setBrush(QColor(255, 50, 50));
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(QPointF(0, -r * 1.15), 3.5, 3.5);
        painter->drawEllipse(QPointF(-r * 0.35, -r * 1.05), 2.5, 2.5);
        painter->drawEllipse(QPointF(r * 0.35, -r * 1.05), 2.5, 2.5);

        // --- Fiery eyes ---
        painter->setBrush(QColor(255, 255, 100));
        painter->setPen(QPen(Qt::black, 2));
        painter->drawEllipse(QPointF(-r * 0.28, -r * 0.15), r * 0.24, r * 0.22);
        painter->drawEllipse(QPointF(r * 0.28, -r * 0.15), r * 0.24, r * 0.22);
        // Pupils (slit-like)
        painter->setBrush(Qt::black);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(QPointF(-r * 0.28, -r * 0.13), r * 0.08, r * 0.16);
        painter->drawEllipse(QPointF(r * 0.28, -r * 0.13), r * 0.08, r * 0.16);

        // --- Scary mouth with teeth ---
        painter->setPen(QPen(Qt::black, 2));
        painter->setBrush(QColor(40, 0, 0));
        painter->drawArc(QRectF(-r * 0.38, r * 0.1, r * 0.76, r * 0.4), 0 * 16, 180 * 16);
        // Teeth
        painter->setBrush(Qt::white);
        painter->setPen(QPen(Qt::black, 1));
        for (int i = 0; i < 3; ++i) {
            qreal tx = (i - 1) * r * 0.2;
            painter->drawPolygon(QPolygonF()
                << QPointF(tx - r * 0.06, r * 0.28)
                << QPointF(tx + r * 0.06, r * 0.28)
                << QPointF(tx, r * 0.42));
        }
        break;
    }
    }

    // --- HP bar (common) ---
    qreal barWidth = r * 2;
    qreal barHeight = 4;
    qreal barY = -r - 10;
    if (m_type == EnemyType::Boss) barY -= 10; // below crown
    qreal hpRatio = qBound(0.0, static_cast<qreal>(m_hp) / m_maxHp, 1.0);

    painter->setBrush(QColor(40, 40, 40));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(QRectF(-barWidth / 2, barY, barWidth, barHeight), 2, 2);
    painter->setBrush(hpRatio > 0.3 ? Qt::green : Qt::red);
    painter->drawRoundedRect(QRectF(-barWidth / 2, barY, barWidth * hpRatio, barHeight), 2, 2);
}

void Enemy::updateMovement(int deltaMs)
{
    // Tick slow timer
    if (m_slowTimer > 0) {
        m_slowTimer -= deltaMs;
        if (m_slowTimer < 0) m_slowTimer = 0;
    }
    qreal speed = (m_slowTimer > 0) ? m_speed * m_slowFactor : m_speed;

    if (m_reachedEnd || m_currentWaypoint >= m_waypoints.size())
        return;

    QPointF target = m_waypoints[m_currentWaypoint];
    QPointF current = pos();
    QPointF dir = target - current;
    qreal dist = qSqrt(dir.x() * dir.x() + dir.y() * dir.y());

    qreal moveDistance = speed * deltaMs / 1000.0;

    if (dist <= moveDistance) {
        prepareGeometryChange();
        setPos(target);
        m_currentWaypoint++;
        if (m_currentWaypoint >= m_waypoints.size()) {
            m_reachedEnd = true;
            return;
        }
        qreal remaining = moveDistance - dist;
        QPointF nextTarget = m_waypoints[m_currentWaypoint];
        QPointF nextDir = nextTarget - target;
        qreal nextDist = qSqrt(nextDir.x() * nextDir.x() + nextDir.y() * nextDir.y());
        if (nextDist > 0) {
            qreal ratio = qMin(remaining / nextDist, 1.0);
            prepareGeometryChange();
            setPos(target + nextDir * ratio);
        }
    } else {
        qreal ratio = moveDistance / dist;
        prepareGeometryChange();
        setPos(current + dir * ratio);
    }
}

void Enemy::takeDamage(int damage)
{
    m_hp -= damage;
    if (m_hp < 0)
        m_hp = 0;
}

void Enemy::applySlow(qreal factor, int durationMs)
{
    m_slowFactor = factor;
    m_slowTimer = durationMs;
}
