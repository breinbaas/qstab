#ifndef PROJECT_H
#define PROJECT_H

#include "geometry.h"
#include "material.h"

class Project
{
public:
    explicit Project();

    Geometry* geometry() {return &m_geometry;};

    void save(QString filename);
    void load(QString filename);

    QList<Material>& materials() {return m_materials;};
    void addMaterial(const QString& code, const float yd, const float ys, const float c, const float phi, const float k, const QString& color);
    QColor getMaterialColor(const QString materialCode);
    QMap<QString, QColor> getSoilColorMap();
private:
    Geometry m_geometry;
    bool m_isDirty;
    QList<Material> m_materials;

};

#endif // PROJECT_H
