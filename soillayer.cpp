#include "soillayer.h"
#include <QPolygonF>

SoilLayer::SoilLayer(const int id, const QString &name, const QString &soilCode, const QList<QPointF> &points)
{
    m_id = id;
    m_name = name;
    m_materialCode = soilCode;
    m_points = points;
}

QList<QPointF> SoilLayer::points()
{
    return m_points;
}

QJsonObject SoilLayer::to_json()
{
    QJsonObject jsonSoilLayer;
    jsonSoilLayer["id"] = m_id;
    jsonSoilLayer["name"] = m_name;
    jsonSoilLayer["soilCode"] = m_materialCode;

    // Create a JSON array for the points
    QJsonArray jsonPointArray;
    for(const QPointF& p: m_points){
        QJsonObject jsonPoint;
        jsonPoint["x"] = p.x();
        jsonPoint["z"] = p.y();
        jsonPointArray.append(jsonPoint);
    }
    jsonSoilLayer["points"] = jsonPointArray;

    return jsonSoilLayer;
}

bool SoilLayer::containsPoint(const QPointF &p)
{
    QPolygonF polygon = QPolygonF(m_points);
    return polygon.containsPoint(p, Qt::OddEvenFill);
}

float SoilLayer::top()
{
    float result = -1e9;
    for(QPointF &p : m_points){
        result = qMax(result, p.y());
    }
    return result;
}

float SoilLayer::bottom()
{
    float result = 1e9;
    for(QPointF &p : m_points){
        result = qMin(result, p.y());
    }
    return result;
}

float SoilLayer::left()
{
    float result = 1e9;
    for(QPointF &p : m_points){
        result = qMin(result, p.x());
    }
    return result;
}

float SoilLayer::right()
{
    float result = -1e9;
    for(QPointF &p : m_points){
        result = qMax(result, p.x());
    }
    return result;
}

