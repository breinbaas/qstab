#ifndef MATERIAL_H
#define MATERIAL_H

#include <QString>
#include <QJsonObject>

class Material
{
public:
    Material(const QString& code, const float yd, const float ys, const float c, const float phi, const float k, const QString& color);

    QString code() const;
    void setCode(const QString &newCode);
    float yd() const;
    void setYd(float newYd);
    float ys() const;
    void setYs(float newYs);
    float c() const;
    void setC(float newC);
    float phi() const;
    void setPhi(float newPhi);
    float k() const;
    void setK(float newK);
    QString color() const;
    void setColor(const QString &newColor);

    QJsonObject to_json();

private:
    QString m_code;
    float m_yd;
    float m_ys;
    float m_c;
    float m_phi;
    float m_k;
    QString m_color;

};

#endif // MATERIAL_H
