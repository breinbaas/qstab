#ifndef SOILLAYER_H
#define SOILLAYER_H

#include <QPointF>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QPolygonF>

class SoilLayer
{
public:
    explicit SoilLayer(const int id, const QString &name, const QString &soilcode, const QList<QPointF> &points);

    QList<QPointF> points();
    QPolygonF asPolygonF() const {return QPolygonF(m_points);};

    QJsonObject to_json();

    bool containsPoint(const QPointF &p);
    QString name() const {return m_name;};
    QString materialCode() const {return m_materialCode;};
    int id() const {return m_id;};
    void setMaterialCode(const QString materialCode) {m_materialCode = materialCode;};

    float top();
    float bottom();
    float left();
    float right();




private:
    int m_id;
    QString m_name;
    QString m_materialCode;
    QList<QPointF> m_points;


};

#endif // SOILLAYER_H
