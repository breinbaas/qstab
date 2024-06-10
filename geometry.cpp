#include "geometry.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QPolygonF>

Geometry::Geometry(QObject *parent)
{
    m_layerId = 1;
}

Geometry::~Geometry()
{

}

void Geometry::addSoilLayer(const QString name, const QString soilCode, const QList<QPointF> &points)
{
    // start with a clean list of soillayers
    QList<SoilLayer> newSoilLayers;

    // add the new one
    SoilLayer sl = SoilLayer(++m_layerId, name, soilCode, points);
    newSoilLayers.append(sl);

    // check for intersections
    for(SoilLayer &sl : m_soillayers){
        QPolygonF pgOld = sl.asPolygonF();
        QPolygonF pgNew = QPolygonF(points);

        if(pgNew.intersects(pgOld)){
            QPolygonF pgSubtracted = pgOld.subtracted(pgNew);
            // the result can have points like a,b,c,d,a,e,f,g,h,e,a
            // so we want to create multiple layers -> [a, b, c, d] and [e, f, g, h]
            QList<QPointF> subtractedLayerPoints = pgSubtracted.toList();
            QList<QPointF> newLayerPoints;
            while(subtractedLayerPoints.count() > 0){
                newLayerPoints.append(subtractedLayerPoints.at(0));
                if(newLayerPoints.count() > 1 && newLayerPoints.at(0) == subtractedLayerPoints.at(0)){
                    int soilLayerId = m_soillayers.count();
                    newLayerPoints.pop_back();
                    newSoilLayers.append(SoilLayer(0, sl.name(), sl.materialCode(), newLayerPoints));
                    newLayerPoints.clear();
                }
                subtractedLayerPoints.pop_front();
            }
        }else{
            newSoilLayers.append(SoilLayer(0, sl.name(), sl.materialCode(), sl.points()));
        }
    }

    // remove the old, add the new
    m_soillayers.clear();
    for(SoilLayer &sl : newSoilLayers){
        int soilLayerId = m_soillayers.count();
        m_soillayers.append(SoilLayer(soilLayerId, sl.name(), sl.materialCode(), sl.points()));
    }
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



