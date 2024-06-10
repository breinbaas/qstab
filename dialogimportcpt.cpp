#include "dialogimportcpt.h"
#include "ui_dialogimportcpt.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QGraphicsRectItem>

const double QMAX = 30.0;

DialogImportCpt::DialogImportCpt(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogImportCpt)
{
    ui->setupUi(this);
    m_cpt = nullptr;
    m_chart_qc = new QChart();
    m_chart_fs = new QChart();
    m_chart_fr = new QChart();

    m_chartViewQc = new QChartView(m_chart_qc);
    QHBoxLayout *layoutQc = new QHBoxLayout(ui->frameQc);
    layoutQc->addWidget(m_chartViewQc);
    ui->frameQc->setLayout(layoutQc);

    m_chartViewFs = new QChartView(m_chart_fs);
    QHBoxLayout *layoutFs = new QHBoxLayout(ui->frameFs);
    layoutFs->addWidget(m_chartViewFs);
    ui->frameFs->setLayout(layoutFs);

    m_chartViewFr = new QChartView(m_chart_fr);
    QHBoxLayout *layoutFr = new QHBoxLayout(ui->frameFr);
    layoutFr->addWidget(m_chartViewFr);
    ui->frameFr->setLayout(layoutFr);
}

DialogImportCpt::~DialogImportCpt()
{
    delete ui;
    delete m_cpt;
    delete m_chart_qc;
    delete m_chart_fs;
    delete m_chart_fr;
}

void DialogImportCpt::on_pbImport_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open JSON", "", "*.json");
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, "No File Selected", "No file was selected.");
        return;
    }

    try {
        if(m_cpt != nullptr) delete m_cpt;
        m_cpt = new Cpt();
        m_cpt->loadJSON(fileName);
        m_soilLayers = m_cpt->to_soillayers(ui->dspbLeft->value(), ui->dspbRight->value(), 0.2, ui->dspbPeatFrictionRatio->value()); //TODO > ui->dspbLayerHeight->value() instead of 0.2

        QLineSeries *seriesQc = new QLineSeries();
        seriesQc->setName("Qc [MPa]");
        for(int i=0; i<m_cpt->z().count(); i++){
            double qc = qMin(m_cpt->qc().at(i), QMAX);
            seriesQc->append(qc, m_cpt->z().at(i));
        }
        m_chart_qc->addSeries(seriesQc);
        m_chart_qc->createDefaultAxes();
        QList<QAbstractAxis *> axes = m_chart_qc->axes(Qt::Horizontal);
        if (!axes.isEmpty()) {
            QValueAxis *axisX = qobject_cast<QValueAxis *>(axes.first());
            if (axisX) {
                axisX->setRange(0, QMAX); // Set the range for the X axis
            }
        }

        QLineSeries *seriesFs = new QLineSeries();
        seriesFs->setName("Fs [MPa]");
        for(int i=0; i<m_cpt->z().count(); i++){
            seriesFs->append(m_cpt->fs().at(i), m_cpt->z().at(i));
        }
        m_chart_fs->addSeries(seriesFs);
        m_chart_fs->createDefaultAxes();

        QLineSeries *seriesFr = new QLineSeries();
        seriesFr->setName("Fr [%]");
        for(int i=0; i<m_cpt->z().count(); i++){
            seriesFr->append(m_cpt->fr().at(i), m_cpt->z().at(i));
        }
        m_chart_fr->addSeries(seriesFr);
        m_chart_fr->createDefaultAxes();

        for(SoilLayer &sl : m_soilLayers){
            float top = sl.top();
            float bottom = sl.bottom();
            QPointF topLeft = m_chart_qc->mapToPosition(QPointF(28,top), seriesQc);
            QPointF bottomRight = m_chart_qc->mapToPosition(QPointF(29,bottom), seriesQc);
            QRectF rect(topLeft, bottomRight);
            QGraphicsRectItem *rectItem = new QGraphicsRectItem(rect);
            rectItem->setBrush(QBrush(m_soilColorMap.value(sl.materialCode())));
            m_chart_qc->scene()->addItem(rectItem);
        }
    } catch (...) {
        //QMessageBox::error
    }
}

