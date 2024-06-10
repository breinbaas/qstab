#ifndef DIALOGIMPORTCPT_H
#define DIALOGIMPORTCPT_H

#include <QDialog>
#include "cpt.h"
#include <QtCharts/QtCharts>

namespace Ui {
class DialogImportCpt;
}

class DialogImportCpt : public QDialog
{
    Q_OBJECT

public:
    explicit DialogImportCpt(QWidget *parent = nullptr);
    ~DialogImportCpt();

    QList<SoilLayer> soilLayers() {return m_soilLayers; };
    void setSoilColorMap(QMap<QString, QColor> soilColorMap) {m_soilColorMap = soilColorMap;};

private slots:
    void on_pbImport_clicked();


private:
    Ui::DialogImportCpt *ui;
    Cpt *m_cpt;

    QChart *m_chart_qc;
    QChart *m_chart_fs;
    QChart *m_chart_fr;

    QChartView *m_chartViewQc;
    QChartView *m_chartViewFs;
    QChartView *m_chartViewFr;

    QList<SoilLayer> m_soilLayers;
    QMap<QString, QColor> m_soilColorMap;
};

#endif // DIALOGIMPORTCPT_H
