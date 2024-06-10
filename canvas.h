#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QMenu>
#include "project.h"

enum CanvasMode {
    Viewing,
    EditSoilLayers,
    AssignMaterials,
};

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent, Project *project);
    QPointF mouseWorldPosition() const {return m_mouseWorldPosition;};

    //void setSnapGridDistance(float distance);

    /* GETTERS AND SETTERS */
    void setDisplayRaster(bool newDisplayRaster);
    void setSnapToGrid(bool newSnapToGrid);
    void setRasterSize(float newRasterSize);
    float rasterSize() const {return m_rasterSize;};

    /* RASTER AND SNAPPING */
    bool snapToGrid() const {return m_snapToGrid;};
    bool displayRaster() const {return m_displayRaster;};

    float snapToGridDistance() const {return m_snapToGridDistance;};
    void setSnapToGridDistance(float newSnapToGridDistance);

    void setCanvasMode(CanvasMode canvasMode);
    CanvasMode canvasMode() {return m_canvasMode;};
    void zoomGeometry();

signals:
    void mouseWorldPositionChanged(QPointF pos);

protected slots:
    void onPopupMenuMaterialTriggered();
    void onPopupMenuDeleteTriggered();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    QPointF screenToWorld(const QPoint &p);
    QPoint worldToScreen(const QPointF &p);

    float left();
    float right();
    float top();
    float bottom();

private:
    CanvasMode m_canvasMode = CanvasMode::Viewing;

    QPointF m_topLeft = QPointF(0.0,0.0);    
    QPointF m_mouseWorldPosition;
    QPoint m_mouseScreenPosition;
    float m_worldScale;

    bool m_isPanning;
    QPoint m_middleMouseDownScreen;

    bool m_displayRaster;
    float m_rasterSize;

    bool m_snapToGrid;
    float m_snapToGridDistance;
    bool m_snappedOnGeometryPoint;

    bool m_isDrawing;
    Project* m_project;

    bool m_addingLayer;
    QList<QPointF> m_addedPoints;

    int m_selectedSoilLayerId;

    QMenu m_materialPopupMenu;
    QMenu m_layerDeletePopupMenu;

    int generateMaterialPopupMenu();

};

#endif // CANVAS_H
