#include "dialogsoiltypes.h"
#include "ui_dialogsoiltypes.h"

#include <QFileDialog>
#include <QMessageBox>
#include "material.h"


DialogSoiltypes::DialogSoiltypes(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSoiltypes)
{
    ui->setupUi(this);


}

DialogSoiltypes::~DialogSoiltypes()
{
    delete ui;

}

void DialogSoiltypes::setMaterials(QList<Material> &materials)
{
    // clear table
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    // update table
    for(int i=0; i<materials.length(); i++){
        int newRowIndex = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(newRowIndex);

        QTableWidgetItem* itemCode = new QTableWidgetItem(materials.at(i).code());
        ui->tableWidget->setItem(newRowIndex, 0, itemCode);
        QTableWidgetItem* itemYd = new QTableWidgetItem(QString::number(materials.at(i).yd()));
        ui->tableWidget->setItem(newRowIndex, 1, itemYd);
        QTableWidgetItem* itemYs = new QTableWidgetItem(QString::number(materials.at(i).ys()));
        ui->tableWidget->setItem(newRowIndex, 2, itemYs);
        QTableWidgetItem* itemC = new QTableWidgetItem(QString::number(materials.at(i).c()));
        ui->tableWidget->setItem(newRowIndex, 3, itemC);
        QTableWidgetItem* itemPhi = new QTableWidgetItem(QString::number(materials.at(i).phi()));
        ui->tableWidget->setItem(newRowIndex, 4, itemPhi);
        QTableWidgetItem* itemK = new QTableWidgetItem(QString::number(materials.at(i).k()));
        ui->tableWidget->setItem(newRowIndex, 5, itemK);
        QTableWidgetItem* itemColor = new QTableWidgetItem(materials.at(i).color());
        ui->tableWidget->setItem(newRowIndex, 6, itemColor);
    }
}

QList<Material> DialogSoiltypes::getMaterials()
{
    QList<Material> materials;

    for(int i=0; i<ui->tableWidget->rowCount(); i++){
        QString code;
        float yd;
        float ys;
        float c;
        float phi;
        float k;
        QString color;

        QTableWidgetItem *item = ui->tableWidget->item(i, 0);
        bool materialOk = true;
        bool ok;
        code = item->text();
        item = ui->tableWidget->item(i, 1);
        yd = item->text().toFloat(&ok);
        materialOk &= ok;
        item = ui->tableWidget->item(i, 2);
        ys = item->text().toFloat(&ok);
        materialOk &= ok;
        item = ui->tableWidget->item(i, 3);
        c = item->text().toFloat(&ok);
        materialOk &= ok;
        item = ui->tableWidget->item(i, 4);
        phi = item->text().toFloat(&ok);
        materialOk &= ok;
        item = ui->tableWidget->item(i, 5);
        k = item->text().toFloat(&ok);
        materialOk &= ok;
        item = ui->tableWidget->item(i, 6);
        color = item->text();

        if(materialOk){
            materials.append(Material(code, yd, ys, c, phi, k, color));
        }
    }
    return materials;
}

void DialogSoiltypes::on_pbAdd_clicked()
{
    int newRowIndex = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(newRowIndex);
}


void DialogSoiltypes::on_tableWidget_cellChanged(int row, int column)
{
    if(column>0 & column<ui->tableWidget->columnCount()-1){
        QTableWidgetItem* item = ui->tableWidget->item(row, column);

        bool ok;
        double f = item->text().toDouble(&ok);

        if (!ok){
            item->setBackground(Qt::red);
        }else{
            item->setBackground(Qt::white);
        }
    }
}




void DialogSoiltypes::on_pbLoad_clicked()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open CSV", "", "*.csv");
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, "No File Selected", "No file was selected. Please select a file to load the materials.");
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Error reading file", "Could not read the file!");
    }

    QTextStream in(&file);

    // Read data line by line
    QStringList rows;

    try{
    while (!in.atEnd()) {
        int newRowIndex = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(newRowIndex);

        QString line = in.readLine();
        QStringList cells = line.split(",", Qt::SkipEmptyParts);  // Split on comma

        for(int currentColumn=0; currentColumn<cells.length(); currentColumn++){
            QTableWidgetItem* item = new QTableWidgetItem(cells.at(currentColumn));
            ui->tableWidget->setItem(newRowIndex, currentColumn, item);
        }
    }
    }catch(...){
        QMessageBox::warning(this, "Error", "Error reading the provided materials file.");
    }

    file.close();
}

