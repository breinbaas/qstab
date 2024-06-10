#include "canvas.h"

#include "canvas.h"
#include <cmath>

const float MAX_WORLD_SCALE = 1; // 1 px = 1m
const float MIN_WORLD_SCALE = 0.01; // 1px = 0.01m
const float DEFAULT_RASTER_SIZE = 1.0; // raster at every meter
const float DEFAULT_WORLD_SCALE = 0.05;  // 1px = 0.05m
const float DEFAULT_SNAP_TO_GRID_DISTANCE = 0.1;
const int SNAP_INDICATOR_RADIUS = 5;
const float ACCURACY = 0.01;


float roundToNearest(float value, float nearest) {
    return std::round(value / nearest) * nearest;
}

QPointF roundPointToNearest(const QPointF& p, float nearest) {
    QPointF roundedPoint;
    roundedPoint.setX(roundToNearest(p.x(), nearest));
    roundedPoint.setY(roundToNearest(p.y(), nearest));
    return roundedPoint;
}

Canvas::Canvas(QWidget *parent, Project *project)
    : QWidget(parent), m_isDrawing(false)
{
    m_project = project;
    setMouseTracking(true);
    m_isPanning = false;
    m_displayRaster = true;
    m_rasterSize = DEFAULT_RASTER_SIZE;
    m_worldScale = DEFAULT_WORLD_SCALE;

    m_snapToGrid = true;
    m_snapToGridDistance = DEFAULT_SNAP_TO_GRID_DISTANCE;

    m_addingLayer = false;
    m_snappedOnGeometryPoint = false;

    m_selectedSoilLayerId = -1;

    // create delete layer menu
    m_layerDeletePopupMenu.addAction("delete", this, &Canvas::onPopupMenuDeleteTriggered);



    // some test data to start with
    //QList<QPointF> some_points;
    //some_points << QPointF(10.01, -2.09) << QPointF(20.02, -2.53) << QPointF(20.0, -4.63) << QPointF(10.04, -4.0);
    //m_project->geometry()->addSoilLayer("temp", "peat", some_points);
}

/* MOUSE EVENTS */

void Canvas::mousePressEvent(QMouseEvent *event)
{    
    if (event->button() == Qt::MiddleButton){
        m_isPanning = true;
        m_middleMouseDownScreen = event->pos();
    }else if(event->button() == Qt::LeftButton) {
        switch (m_canvasMode) {
        case CanvasMode::EditSoilLayers:
        {
            if (event->button() == Qt::LeftButton){ // adding points / layers
                // get to position in the world coordinates
                QPointF posInWorld = screenToWorld(event->pos());

                // if we have snap enabled we need to snap to the closest point on the grid
                // or on an exisiting point
                if(m_snapToGrid){
                    posInWorld = roundPointToNearest(posInWorld, m_snapToGridDistance);
                }

                if(m_snappedOnGeometryPoint){
                    posInWorld = m_mouseWorldPosition;
                }

                // round to the given accuracy
                posInWorld.setX(roundToNearest(posInWorld.x(), ACCURACY));
                posInWorld.setY(roundToNearest(posInWorld.y(), ACCURACY));

                if(!m_addingLayer){ // this is the first point of a new layer
                    m_addingLayer = true;
                    m_addedPoints.append(posInWorld);
                }else{ // this is another point on the layer
                    QPointF firstPoint = m_addedPoints[0];
                    // adding points
                    if(m_addedPoints.length()>2){ // if we have more than 2 points we can close the layer
                        /* Check if the current point is the first point of the layer
                         * if so, end the soillayer and add it to the geometry
                         * TODO > gaat dit altijd goed? misschien marge nodig om twee QPointF te checken op 1mm afstand?
                         */
                        if(firstPoint.x() == posInWorld.x() && firstPoint.y() == posInWorld.y()){
                            // layer is complete
                            m_project->geometry()->addSoilLayer("temp", "peat", m_addedPoints);
                            m_addingLayer = false;
                            m_addedPoints.clear();
                            update();
                        }
                        else{
                            m_addedPoints.append(posInWorld);
                        }
                    }else{
                        m_addedPoints.append(posInWorld);
                    }
                }
            }
            break;
        }
        default:
            break;
        }
        update();
    }else if(event->button() == Qt::RightButton){
        switch (m_canvasMode)
        {
            case CanvasMode::EditSoilLayers:{
                // if we are adding a layer remove the points from last to first
                if(m_addingLayer){
                    if(m_addedPoints.length()>1){
                        m_addedPoints.pop_back();
                    }
                    else if(m_addedPoints.length()==1){ // nothing left, stop adding the layer
                        m_addedPoints.clear();
                        m_addingLayer = false;
                    }
                    update();
                }else if (m_selectedSoilLayerId != -1){
                    // show delete menu
                    m_layerDeletePopupMenu.exec(event->globalPosition().toPoint());
                }
                break;
            }
            case CanvasMode::AssignMaterials:
            {
                // if a layer is selected and we have materials, show the material popup
                if (m_selectedSoilLayerId != -1){
                    int numEntries = generateMaterialPopupMenu();
                    if(numEntries > 0){
                        m_materialPopupMenu.exec(event->globalPosition().toPoint());
                    }
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
}


void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    // set the current mouseposition as world coordinates
    m_mouseWorldPosition = screenToWorld(event->pos());
    m_snappedOnGeometryPoint = false;

    if(m_snapToGrid){
        m_mouseWorldPosition = roundPointToNearest(m_mouseWorldPosition, m_snapToGridDistance);
    }

    // TODO if we are close to other geometry points we snap to the geometry point
    // iterate over all layers in the geometry
    // convert the point to screen coordinates
    // if the distance between the event.pos and the screen coord < range
    // then snap on this point
    for(int i=0; i<m_project->geometry()->soillayers().count(); i++){
        SoilLayer sl = m_project->geometry()->soillayers().at(i);
        for(QPointF p: sl.points()){
            QPoint pointOnScreen = worldToScreen(p);
            int dx = pointOnScreen.x() - event->pos().x();
            int dy = pointOnScreen.y() - event->pos().y();
            float dl = std::sqrt(std::pow(dx, 2) + std::pow(dy,2));
            if(dl < SNAP_INDICATOR_RADIUS){
                m_mouseWorldPosition = p;
                m_snappedOnGeometryPoint = true;
                break;
            }
        }
    }

    emit mouseWorldPositionChanged(m_mouseWorldPosition);

    // if we are panning, move the canvas
    if(m_isPanning){
        int dx = event->pos().x() - m_middleMouseDownScreen.x();
        int dy = event->pos().y() - m_middleMouseDownScreen.y();

        m_topLeft.setX(m_topLeft.x() - dx * m_worldScale);
        m_topLeft.setY(m_topLeft.y() + dy * m_worldScale);
        m_middleMouseDownScreen = event->pos();
    }

    // remember the mouse position
    m_mouseScreenPosition = event->pos();

    // check the selected soillayer
    if(m_canvasMode == CanvasMode::AssignMaterials || m_canvasMode == CanvasMode::EditSoilLayers && !m_addingLayer){
        QPointF worldPoint = screenToWorld(event->pos());
        m_selectedSoilLayerId = m_project->geometry()->selectedSoilLayerId(worldPoint);
    }

    update();
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton){
        m_isPanning = false;
    }
}

void Canvas::wheelEvent(QWheelEvent *event)
{    
    int angle_delta = event->angleDelta().y() / 120;

    if(angle_delta == 1){ // zoom in
        m_worldScale *= 0.95;
    }else if (angle_delta == -1){ // zoom out
        m_worldScale *= 1.05;
    }
    m_worldScale = qMax(m_worldScale, MIN_WORLD_SCALE);
    m_worldScale = qMin(m_worldScale, MAX_WORLD_SCALE);

    m_topLeft.setX(m_mouseWorldPosition.x() - m_mouseScreenPosition.x() * m_worldScale);
    m_topLeft.setY(m_mouseWorldPosition.y() + m_mouseScreenPosition.y() * m_worldScale);

    update();
}

/* PAINTING */

void Canvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);    
    painter.setRenderHint(QPainter::Antialiasing);
    QBrush originalBrush = painter.brush();
    painter.fillRect(rect(), Qt::white);

    QColor darkGrayColor(Qt::darkGray);
    QPen pen_raster(darkGrayColor);
    QPen pen_snapIndicator(darkGrayColor);
    QColor grayColor(Qt::gray);
    QPen pen_geometry(grayColor);
    QColor blackColor(Qt::black);
    QPen pen_black(blackColor);
    QPen pen_addedPoints(blackColor);

    // show raster
    if(m_displayRaster){
        painter.setPen(pen_raster);
        float _left = roundToNearest(left(), m_rasterSize) - m_rasterSize;
        float _right = roundToNearest(right(), m_rasterSize) + m_rasterSize;
        float _top = roundToNearest(top(), m_rasterSize) + m_rasterSize;
        float _bottom = roundToNearest(bottom(), m_rasterSize) - m_rasterSize;

        float x = _left;
        while(x < _right){
            float y = _top;
            while(y > _bottom){
                y -= m_rasterSize;
                QPoint p = worldToScreen(QPointF(x,y));
                painter.drawPoint(p);
            }
            x += m_rasterSize;
        }
    }

    // show geometry
    painter.setPen(pen_geometry);
    for(int i=0; i<m_project->geometry()->soillayers().count(); i++){
        SoilLayer sl = m_project->geometry()->soillayers().at(i);

        QPoint prev_point(-1,-1);

        QList<QPoint> screenPoints;
        for(int i=0; i<sl.points().length(); i++){
            screenPoints.append(worldToScreen(sl.points().at(i)));
        }

        // TODO > netter om deze als laatste te tekenen
        // anders worden de overige lagen er overheen getekend
        // en krijg je rare weergaves
        if(m_selectedSoilLayerId == sl.id()){
            QPen tempPen = QPen(Qt::black);
            tempPen.setWidth(2);
            painter.setPen(tempPen);
        }else{
            painter.setPen(pen_geometry);
        }

        // draw color of the layer if a material is assigned
        if(!sl.materialCode().isEmpty()){
            QColor brushColor = m_project->getMaterialColor(sl.materialCode());
            painter.setBrush(QBrush(brushColor));
        }

        painter.drawPolygon(screenPoints);
        painter.setBrush(originalBrush);
    }

    // redraw the lines if a layer is selected
    for(int i=0; i<m_project->geometry()->soillayers().count(); i++){
        SoilLayer sl = m_project->geometry()->soillayers().at(i);
        if(sl.id() == m_selectedSoilLayerId){
            QList<QPoint> screenPoints;
            for(int i=0; i<sl.points().length(); i++){
                screenPoints.append(worldToScreen(sl.points().at(i)));
            }
            QPen tempPen = QPen(Qt::black);
            tempPen.setWidth(2);
            painter.setPen(tempPen);
            painter.drawPolygon(screenPoints);
            break;
        }
    }

    // show layer in progress
    painter.setPen(pen_addedPoints);
    for(int i=1; i<m_addedPoints.length(); i++){
        QPoint p1 = worldToScreen(m_addedPoints[i-1]);
        QPoint p2 = worldToScreen(m_addedPoints[i]);
        QLine l = QLine(p1, p2);
        painter.drawLine(l);
    }

    // if we are drawing lines show the line from
    // the last point to the current mouse position
    if(m_addingLayer && m_addedPoints.length()>0){
        QPoint p1 = worldToScreen(m_addedPoints.last());
        QPoint current = worldToScreen(m_mouseWorldPosition);
        QLine l = QLine(p1, current);
        painter.drawLine(l);
    }

    // if we are snapping show the snapped point
    if (m_canvasMode == CanvasMode::EditSoilLayers){
        if(m_snapToGrid || m_snappedOnGeometryPoint){
            QColor snapColor = m_snappedOnGeometryPoint ? QColor(Qt::green) : QColor(Qt::gray);
            painter.setPen(QPen(snapColor));
            QPoint center = worldToScreen(m_mouseWorldPosition);
            painter.drawEllipse(center, SNAP_INDICATOR_RADIUS, SNAP_INDICATOR_RADIUS);
        }
    }
}

/* HELPER FUNCTIONS */


/* COORDINATE SYSTEM FUNCTIONS */

QPointF Canvas::screenToWorld(const QPoint &p)
{
    float x = m_topLeft.x() + p.x() * m_worldScale;
    float z = m_topLeft.y() - p.y() * m_worldScale;
    return QPointF(x,z);
}

QPoint Canvas::worldToScreen(const QPointF &p)
{
    int x = int((p.x() - m_topLeft.x()) / m_worldScale);
    int y = int((m_topLeft.y() - p.y()) / m_worldScale);
    return QPoint(x, y);
}

float Canvas::left()
{
    return m_topLeft.x();
}

float Canvas::right()
{
    return m_topLeft.x() + width() * m_worldScale;
}

float Canvas::top()
{
    return m_topLeft.y();
}

float Canvas::bottom()
{
    return m_topLeft.y() - height() * m_worldScale;
}

void Canvas::setSnapToGridDistance(float newSnapToGridDistance)
{
    m_snapToGridDistance = newSnapToGridDistance;
    update();
}

void Canvas::setCanvasMode(CanvasMode canvasMode)
{
    m_canvasMode = canvasMode;

    //TODO > stop laag definitie als die begonnen was

    update();
}



void Canvas::setRasterSize(float newRasterSize)
{
    m_rasterSize = newRasterSize;
    update();
}

void Canvas::setSnapToGrid(bool newSnapToGrid)
{
    m_snapToGrid = newSnapToGrid;
    update();
}

void Canvas::setDisplayRaster(bool newDisplayRaster)
{
    m_displayRaster = newDisplayRaster;
    update();
}

/*
 * MENU HANDLERS
 */
int Canvas::generateMaterialPopupMenu()
{
    if(m_project->materials().count() == 0) return 0;

    m_materialPopupMenu.clear();
    for(Material &m : m_project->materials()){
        m_materialPopupMenu.addAction(m.code(), this, &Canvas::onPopupMenuMaterialTriggered);
    };
    return m_project->materials().count();
}



void Canvas::zoomGeometry()
{
    const float MARGIN = 1.0;
    float left = m_project->geometry()->left() - MARGIN;
    float right = m_project->geometry()->right() + MARGIN;
    float top = m_project->geometry()->top() + MARGIN;
    float bottom = m_project->geometry()->bottom() - MARGIN;

    float worldScaleX = (right - left) / width();
    float worldScaleY = (top - bottom) / height();

    if(worldScaleX > worldScaleY){
        m_worldScale = worldScaleX;
        top = (top + bottom) / 2.0 + height() / 2.0 * m_worldScale;
        m_topLeft = QPointF(left, top);
    }else{
        m_worldScale = worldScaleX;
        left = (left + right) / 2.0 - width() / 2.0 * m_worldScale;
        m_topLeft = QPointF(left, top);
    }
    update();
}

void Canvas::onPopupMenuMaterialTriggered()
{
    QAction* selectedAction = qobject_cast<QAction*>(sender());

    if (selectedAction) {
        QString materialCode = selectedAction->text();
        m_project->geometry()->setSoilLayerMaterial(m_selectedSoilLayerId, materialCode);
        update();
    }
}

void Canvas::onPopupMenuDeleteTriggered()
{
    if(m_selectedSoilLayerId != -1){
        m_project->geometry()->removeLayerBydId(m_selectedSoilLayerId);
    }
    update();
}




