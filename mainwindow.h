#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "canvas.h"
#include "project.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void updateMouseWorldLabel(QPointF pos);

private slots:
    void on_actionSnap_triggered(bool checked);
    void on_actionCanvas_settings_triggered();
    void on_actionShowHideRaster_triggered(bool checked);

    void on_actionSave_triggered();

    void on_actionMaterials_triggered();

    void on_actionAssignMaterials_triggered(bool checked);
    void on_actionEditSoilLayers_triggered(bool checked);

    void on_actionImport_CPT_triggered();

private:
    Ui::MainWindow *ui;
    Project m_project;
    Canvas *m_canvas;

};
#endif // MAINWINDOW_H
