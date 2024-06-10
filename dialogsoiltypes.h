#ifndef DIALOGSOILTYPES_H
#define DIALOGSOILTYPES_H

#include <QDialog>
#include "material.h"


namespace Ui {
class DialogSoiltypes;
}

class DialogSoiltypes : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSoiltypes(QWidget *parent = nullptr);
    ~DialogSoiltypes();

    void setMaterials(QList<Material> &materials);
    QList<Material> getMaterials();

private slots:
    void on_pbAdd_clicked();
    void on_tableWidget_cellChanged(int row, int column);
    void on_pbLoad_clicked();

private:
    Ui::DialogSoiltypes *ui;
};

#endif // DIALOGSOILTYPES_H
