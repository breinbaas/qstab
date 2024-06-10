#include "cpt.h"
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>

Cpt::Cpt() {}

void Cpt::loadJSON(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        // TODO throw error
        //QMessageBox::warning(this, "Error reading file", "Could not read the file!");
    }

    QByteArray jsonData = file.readAll();
    file.close();

    // Read data line by line
    QStringList rows;

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);

    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Error parsing JSON file: " << error.errorString();
        return;
    }

    try {
        QJsonObject jsonObj = doc.object();
        m_name = jsonObj["name"].toString();
        m_date = jsonObj["date"].toString();
        m_x = jsonObj["x"].toDouble();
        m_y = jsonObj["y"].toDouble();
        m_top = jsonObj["top"].toDouble();
        m_preExcavatedDepth = jsonObj["pre_excavated_depth"].toDouble();
        QJsonArray jsonArray = jsonObj["z"].toArray();
        for (int i = 0; i < jsonArray.size(); ++i) {
            QJsonValue value = jsonArray.at(i);
            m_z.append(value.toDouble());
        }
        jsonArray = jsonObj["qc"].toArray();
        for (int i = 0; i < jsonArray.size(); ++i) {
            QJsonValue value = jsonArray.at(i);
            m_qc.append(value.toDouble());
        }
        jsonArray = jsonObj["fs"].toArray();
        for (int i = 0; i < jsonArray.size(); ++i) {
            QJsonValue value = jsonArray.at(i);
            m_fs.append(value.toDouble());
        }
        jsonArray = jsonObj["fr"].toArray();
        for (int i = 0; i < jsonArray.size(); ++i) {
            QJsonValue value = jsonArray.at(i);
            m_fr.append(value.toDouble());
        }

    } catch (...) {
        qDebug() << "Error reading json file";
    }
}

QList<SoilLayer> Cpt::to_soillayers(double left, double right, double minLayerHeight, double peatFrictionRatio)
{
    QList<SoilLayer> result;

    // create a list of all soiltypes for each z coordinate
    QList<int> m_soiltypes; //0 = preexcavated, 1 = sand, 2 = clay, 3 = peat
    for(int i=0; i<m_qc.count(); i++){
        int soilType = 0;

        if(m_z[i] < m_top - m_preExcavatedDepth){
            double x = qMax(m_fr[i], 0.0);
            x = qMin(x, 10.0);
            double y = qMin(log10(m_qc[i]),-1.0);
            y = qMax(y, 2.0);

            if(y <= x*0.4 - 2.0){
                if(x < 4.0){
                    soilType = 2;
                }else{
                    soilType = 3;
                }
            }else if(y > x * 0.4 - 0.30103){
                soilType = 1;
            }else{
                soilType = 2;
            }

            if(x > peatFrictionRatio){
                soilType = 3;
            }
        }
        m_soiltypes.append(soilType);
    }

    // start at the top and check the dominant soiltype for each part of minLayerHeight
    QList<QString> soilLayers;

    // add the preexcavated layer if present
    if(m_preExcavatedDepth > 0.0){
        soilLayers.append(QString("%1,%2,%3").arg("preexcavated").arg(m_top).arg(m_top - m_preExcavatedDepth));
    }

    int countPeat = 0;
    int countClay = 0;
    int countSand = 0;
    double prevDepth = m_top - m_preExcavatedDepth;
    for(int i=0; i<m_z.count(); i++){
        double z = m_z[i];

        switch (m_soiltypes[i]) {
        case 1:
            countSand += 1;
            break;
        case 2:
            countClay += 1;
            break;
        default:
            countPeat += 1;
        }

        if(z < prevDepth - minLayerHeight){
            if(countClay > countPeat && countClay > countSand){
                soilLayers.append(QString("%1,%2,%3").arg("clay").arg(prevDepth).arg(z));
            }else if(countPeat > countClay && countPeat > countSand){
                soilLayers.append(QString("%1,%2,%3").arg("peat").arg(prevDepth).arg(z));
            }else{
                soilLayers.append(QString("%1,%2,%3").arg("sand").arg(prevDepth).arg(z));
            }

            // reset
            countPeat = 0;
            countClay = 0;
            countSand = 0;
            prevDepth = z;
        }
    }

    // merge adjecent layers with the same soiltype
    QList<QString> finalSoillayers;
    QString prevSoilCode;
    for(int i=0; i<soilLayers.count(); i++){
        QList args = soilLayers[i].split(",");
        if(i == 0){
            prevSoilCode = args[0];
            finalSoillayers.append(soilLayers[i]);
        }else{
            if(args[0] == prevSoilCode){
                QList<QString> old_args = finalSoillayers[finalSoillayers.count()-1].split(',');
                finalSoillayers[finalSoillayers.count()-1] = QString("%1,%2,%3").arg(prevSoilCode).arg(old_args[1]).arg(args[2]);
            }else{
                prevSoilCode = args[0];
                finalSoillayers.append(soilLayers[i]);
            }
        }
    }

    // create the final result
    for(QString &s : finalSoillayers){
        QList args = s.split(",");
        double top = args[1].toDouble();
        double bottom = args[2].toDouble();
        QList<QPointF> points;
        points << QPointF(left, top) << QPointF(right, top) << QPointF(right, bottom) << QPointF(left, bottom);
        result.append(SoilLayer(-1, args[0], args[0], points));
    }

    return result;
}
