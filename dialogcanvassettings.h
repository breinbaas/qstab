#ifndef DIALOGCANVASSETTINGS_H
#define DIALOGCANVASSETTINGS_H

#include <QDialog>

namespace Ui {
class DialogCanvasSettings;
}

class DialogCanvasSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCanvasSettings(QWidget *parent, float rasterSize, float snapDistance);
    ~DialogCanvasSettings();
    float rasterSize() const {return m_rasterSize;};
    void setRasterSize(float newRasterSize) {m_rasterSize = newRasterSize;};
    float snapToGridDistance() const {return m_snapToGridDistance;};
    void setSnapToGridDistance(float newSnapToGridDistance) {m_snapToGridDistance = newSnapToGridDistance;};

private slots:
    void on_dspRasterSize_valueChanged(double arg1);
    void on_dspSnapDistance_valueChanged(double arg1);

private:
    float m_rasterSize;
    float m_snapToGridDistance;
    Ui::DialogCanvasSettings *ui;
};

#endif // DIALOGCANVASSETTINGS_H
