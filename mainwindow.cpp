#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "dialogcanvassettings.h"
#include "dialogsoiltypes.h"
#include "dialogimportcpt.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* add the canvas */
    m_canvas = new Canvas(this, &m_project);
    QVBoxLayout *layout = new QVBoxLayout(ui->frmCanvas);
    layout->addWidget(m_canvas);

    /* set the default */
    ui->actionSnap->setChecked(m_canvas->snapToGrid());
    ui->actionShowHideRaster->setChecked(m_canvas->displayRaster());

    /* connect to signals */
    connect(m_canvas, SIGNAL(mouseWorldPositionChanged(QPointF)), this, SLOT(updateMouseWorldLabel(QPointF)));
}

MainWindow::~MainWindow()
{
    delete m_canvas;
    delete ui;
}

void MainWindow::updateMouseWorldLabel(QPointF pos)
{
    ui->lblMouseWorldPosition->setText(QString("%1,%2").arg(pos.x(),0,'f',2).arg(pos.y(),0,'f',2));
}


void MainWindow::on_actionSnap_triggered(bool checked)
{
    m_canvas->setSnapToGrid(checked);
}




void MainWindow::on_actionShowHideRaster_triggered(bool checked)
{
    m_canvas->setDisplayRaster(checked);
}


void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Project", "", "JSON Files (*.json);;All Files (*)");
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, "No File Selected", "No file was selected. Please select a file to save the geometry data.");
        return;
    }
    try {
        m_project.save(fileName);
        QMessageBox::information(this, "Succes", QString("The project is saved to '%1'.").arg(fileName));
    } catch (const std::exception &e) {
        QString errorMessage = QString("Couldn't write to file, got error '%1'.").arg(QString::fromUtf8(e.what()));
        QMessageBox::warning(this, "Error", errorMessage);
    }
}

void MainWindow::on_actionAssignMaterials_triggered(bool checked)
{
    if(m_canvas->canvasMode() != CanvasMode::AssignMaterials && checked){
        m_canvas->setCanvasMode(CanvasMode::AssignMaterials);
        ui->actionEditSoilLayers->setChecked(false);
    }else{
        m_canvas->setCanvasMode(CanvasMode::Viewing);
    }
}


void MainWindow::on_actionEditSoilLayers_triggered(bool checked)
{
    if(m_canvas->canvasMode() != CanvasMode::EditSoilLayers && checked){
        m_canvas->setCanvasMode(CanvasMode::EditSoilLayers);
        ui->actionAssignMaterials->setChecked(false);
    }else{
        m_canvas->setCanvasMode(CanvasMode::Viewing);
    }
}

/***********
 * DIALOGS *
 ***********/

void MainWindow::on_actionCanvas_settings_triggered()
{
    DialogCanvasSettings dlg(this, m_canvas->rasterSize(), m_canvas->snapToGridDistance());

    if (dlg.exec() == QDialog::Accepted) {
        if(dlg.rasterSize() != m_canvas->rasterSize()){
            m_canvas->setRasterSize(dlg.rasterSize());
        }
        if(dlg.snapToGridDistance() != m_canvas->snapToGridDistance()){
            m_canvas->setSnapToGridDistance(dlg.snapToGridDistance());
        }
    }
}


void MainWindow::on_actionMaterials_triggered()
{
    DialogSoiltypes dlg(this);
    dlg.setMaterials(m_project.materials());

    if (dlg.exec() == QDialog::Accepted) {
        m_project.materials().clear();
        QList<Material> materials = dlg.getMaterials();
        for(Material &m : materials){
            m_project.addMaterial(m.code(), m.yd(), m.ys(), m.c(), m.phi(), m.k(), m.color());
        }
    }
}

void MainWindow::on_actionImport_CPT_triggered()
{
    DialogImportCpt dlg(this);
    dlg.setSoilColorMap(m_project.getSoilColorMap());

    if (dlg.exec() == QDialog::Accepted) {
        for(SoilLayer &sl : dlg.soilLayers()){
            m_project.geometry()->addSoilLayer(sl.name(), sl.materialCode(), sl.points());
        }
    }

    m_canvas->zoomGeometry();
}









