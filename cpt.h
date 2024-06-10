#ifndef CPT_H
#define CPT_H

#include <QList>
#include <QFile>

#include "soillayer.h"

class Cpt
{
public:
    Cpt();

    void loadJSON(const QString& fileName);

    QVector<double>& z() {return m_z;};
    QVector<double>& qc() {return m_qc;};
    QVector<double>& fs() {return m_fs;};
    QVector<double>& fr() {return m_fr;};
    QList<SoilLayer> to_soillayers(double left, double right, double minLayerHeight=0.1, double peatFrictionRatio = 10.0);

private:
    QString m_name;
    QString m_date;
    float m_x;
    float m_y;
    float m_top;
    float m_preExcavatedDepth;
    QVector<double> m_z;
    QVector<double> m_qc;
    QVector<double> m_fs;
    QVector<double> m_fr;

};

#endif // CPT_H
