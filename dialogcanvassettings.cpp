#include "dialogcanvassettings.h"
#include "ui_dialogcanvassettings.h"

DialogCanvasSettings::DialogCanvasSettings(QWidget *parent, float rasterSize, float snapDistance)
    : QDialog(parent)
    , ui(new Ui::DialogCanvasSettings)
{
    ui->setupUi(this);
    m_rasterSize = rasterSize;
    m_snapToGridDistance = snapDistance;

    ui->dspRasterSize->setMaximum(10.0);
    ui->dspRasterSize->setMinimum(0.1);
    ui->dspRasterSize->setSingleStep(0.1);
    ui->dspRasterSize->setValue(m_rasterSize);

    ui->dspSnapDistance->setMaximum(10.0);
    ui->dspSnapDistance->setMinimum(0.1);
    ui->dspSnapDistance->setSingleStep(0.1);
    ui->dspSnapDistance->setValue(m_snapToGridDistance);
}

DialogCanvasSettings::~DialogCanvasSettings()
{
    delete ui;
}

void DialogCanvasSettings::on_dspRasterSize_valueChanged(double arg)
{
    m_rasterSize = arg;
}

void DialogCanvasSettings::on_dspSnapDistance_valueChanged(double arg)
{
    m_snapToGridDistance = arg;
}

