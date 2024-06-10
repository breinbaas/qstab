#include "material.h"

Material::Material(const QString& code, const float yd, const float ys, const float c, const float phi, const float k, const QString& color) {
    setCode(code);
    setYd(yd);
    setYs(ys);
    setC(c);
    setPhi(phi);
    setK(k);
    setColor(color);
}

QString Material::code() const
{
    return m_code;
}

void Material::setCode(const QString &newCode)
{
    m_code = newCode;
}

float Material::yd() const
{
    return m_yd;
}

void Material::setYd(float newYd)
{
    m_yd = newYd;
}

float Material::ys() const
{
    return m_ys;
}

void Material::setYs(float newYs)
{
    m_ys = newYs;
}

float Material::c() const
{
    return m_c;
}

void Material::setC(float newC)
{
    m_c = newC;
}

float Material::phi() const
{
    return m_phi;
}

void Material::setPhi(float newPhi)
{
    m_phi = newPhi;
}

float Material::k() const
{
    return m_k;
}

void Material::setK(float newK)
{
    m_k = newK;
}

QString Material::color() const
{
    return m_color;
}

void Material::setColor(const QString &newColor)
{
    m_color = newColor;
}

QJsonObject Material::to_json()
{
    QJsonObject jsonMaterial;
    jsonMaterial["code"] = m_code;
    jsonMaterial["yd"] = m_yd;
    jsonMaterial["ys"] = m_ys;
    jsonMaterial["c"] = m_c;
    jsonMaterial["phi"] = m_phi;
    jsonMaterial["k"] = m_k;
    jsonMaterial["color"] = m_color;


    return jsonMaterial;
}
