#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QList>
#include <QJsonObject>

#include "soillayer.h"


class Geometry
{
public:
    explicit Geometry(QObject *parent = nullptr);
    ~Geometry();

    QList<SoilLayer>& soillayers() {return m_soillayers;}
    void addSoilLayer(const QString name, const QString soilCode, const QList<QPointF> &points);

    bool lineIsCrossingOtherLines(const QLine& l1);
    QJsonObject to_json();

    int selectedSoilLayerId(const QPointF &p);
    void setSoilLayerMaterial(const int soilLayerId, const QString& materialCode);
    SoilLayer *soilLayerById(const int soilLayerId);
    void removeLayerBydId(const int soilLayerId);

    float left();
    float right();
    float top();
    float bottom();

private:
    QString m_name;
    QList<SoilLayer> m_soillayers;

};

#endif // GEOMETRY_H
