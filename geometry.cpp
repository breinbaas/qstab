#include "geometry.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QPolygonF>

Geometry::Geometry(QObject *parent)
{

}

Geometry::~Geometry()
{

}

void Geometry::addSoilLayer(const QString name, const QString soilCode, const QList<QPointF> &points)
{
    QList<QPointF> p1;
    QList<QPointF> p2;
    p1 << QPointF(0.0, 0.0) << QPointF(10.0, 0.0) << QPointF(10.0, -10.0) << QPointF(0.0, -10.0);
    // p2 << QPointF(5.0, 2.0) << QPointF(12.0, 2.0) << QPointF(12.0, -1.0) << QPointF(5.0, -1.0);
    p2 << QPointF(4.0, 1.0) << QPointF(6.0, 1.0) << QPointF(6.0, -11.0) << QPointF(4.0, -11.0);
    QPolygonF pg1(p1);
    QPolygonF pg2(p2);

    qDebug() << pg1.subtracted(pg2);
    qDebug() << pg2.subtracted(pg1);

    // // check if this polygon intersectes other ones, if so subtract this polygon from the others
    // for(SoilLayer &sl : m_soillayers){
    //     QPolygonF pgOld = sl.asPolygonF();
    //     QPolygonF pgNew = QPolygonF(points);

    //     if(pgNew.intersects(pgOld)){
    //         QPolygonF subtracted = pgOld.subtracted(pgNew);
    //         qDebug () << "Intersection detected!" << subtracted;


    //     }
    // }

    // int soilLayerId = m_soillayers.count();
    // SoilLayer sl = SoilLayer(soilLayerId, name, soilCode, points);
    // m_soillayers.append(sl);
}

bool Geometry::lineIsCrossingOtherLines(const QLine &l1)
{
    // TODO
    // check if the given line intersects the lines in the geometry
    return false;
}

QJsonObject Geometry::to_json()
{
    QJsonObject jsonObject;
    jsonObject["name"] = m_name;

    QJsonArray jsonSoilLayerArray;
    for(SoilLayer& sl: m_soillayers){
        QJsonObject jsonSoilLayer = sl.to_json();
        jsonSoilLayerArray.append(jsonSoilLayer);
    }
    jsonObject["soilLayers"] = jsonSoilLayerArray;

    return jsonObject;
}

int Geometry::selectedSoilLayerId(const QPointF& p)
{
    for(SoilLayer &sl : m_soillayers){
        if(sl.containsPoint(p)){
            return sl.id();
        }
    }

    return -1;
}

void Geometry::setSoilLayerMaterial(const int soilLayerId, const QString& materialCode)
{
    for(SoilLayer &sl : m_soillayers){
        if(sl.id() == soilLayerId){
            sl.setMaterialCode(materialCode);
            break;
        }
    }
}

SoilLayer *Geometry::soilLayerById(const int soilLayerId)
{
    for(SoilLayer &sl : m_soillayers){
        if(sl.id() == soilLayerId){
            return &sl;
        }
    }
    qDebug () << "This should not happen, cannot get soil layer with id " << soilLayerId;
    throw std::invalid_argument("Cannot get soil layer with the given id");
}

void Geometry::removeLayerBydId(const int soilLayerId)
{
    for(int i=0; i<m_soillayers.count(); i++){
        if(m_soillayers.at(i).id() == soilLayerId){
            m_soillayers.removeAt(i);
        }
    }
}

float Geometry::left()
{
    if(m_soillayers.count()==0){
        return -10;
    }

    float result = 1e9;
    for(SoilLayer &sl : m_soillayers){
        for(QPointF &p :sl.points()){
            result = qMin(result, p.x());
        }
    }
    return result;
}

float Geometry::right()
{
    if(m_soillayers.count()==0){
        return 10;
    }

    float result = -1e9;
    for(SoilLayer &sl : m_soillayers){
        for(QPointF &p :sl.points()){
            result = qMax(result, p.x());
        }
    }
    return result;
}

float Geometry::top()
{
    if(m_soillayers.count()==0){
        return 10;
    }

    float result = -1e9;
    for(SoilLayer &sl : m_soillayers){
        for(QPointF &p :sl.points()){
            result = qMax(result, p.y());
        }
    }
    return result;
}

float Geometry::bottom()
{
    if(m_soillayers.count()==0){
        return -10;
    }

    float result = 1e9;
    for(SoilLayer &sl : m_soillayers){
        for(QPointF &p :sl.points()){
            result = qMin(result, p.y());
        }
    }
    return result;
}


