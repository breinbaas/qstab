#include "project.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QColor>

Project::Project()
{
    m_isDirty = false;

    addMaterial("peat", 10.0, 10.0, 2.0, 15.0, 1e-7, "#9e7a24");
    addMaterial("clay", 14.0, 14.0, 3.0, 22.5, 1e-8, "#63c72a");
    addMaterial("sand", 17.0, 19.0, 0.0, 30.0, 1e-3, "#ede998");
    addMaterial("preexcavated", 15.0, 15.0, 2.0, 27.5, 1e-5, "#8c897e");
}

void Project::save(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        throw std::runtime_error("Couldn't open save file.");
    }

    try {
        QJsonObject json;

        // Create a JSON object for the geometry
        json["geometry"] = m_geometry.to_json();

        // and for the materials
        QJsonArray jsonMaterialArray;
        for(Material& m: m_materials){
            QJsonObject jsonMaterial = m.to_json();
            jsonMaterialArray.append(jsonMaterial);
        }
        json["materials"] = jsonMaterialArray;


        // Convert JSON object to QJsonDocument
        QJsonDocument jsonDoc(json);
        file.write(jsonDoc.toJson());
        file.close();

        m_isDirty = false;

        qDebug() << "JSON file written successfully";
    } catch (const std::exception &e) {
        QString msg;
        qCritical() << "An error occurred:" << e.what();
        throw std::runtime_error("Couldn't write to file, got error .");
    }
}

void Project::load(QString filename)
{

}

void Project::addMaterial(const QString &code, const float yd, const float ys, const float c, const float phi, const float k, const QString &color)
{
    Material material(code, yd, ys, c, phi, k, color);
    m_materials.append(material);
}

QColor Project::getMaterialColor(const QString materialCode)
{
    for(Material &m : m_materials){
        if(m.code() == materialCode){
            return QColor(m.color());
        }
    }
    qDebug() << "This should not happen, no color found for material " << materialCode;
    return QColor("#ffffff");
}

QMap<QString, QColor> Project::getSoilColorMap()
{
    QMap<QString, QColor> result;
    for(Material &m : m_materials){
        result.insert(m.code(), m.color());
    }
    return result;
}

