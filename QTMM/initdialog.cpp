#include <QMessageBox>
#include <QFileDialog>
#include "initdialog.h"
#include "ui_initdialog.h"

InitDialog::InitDialog(QWidget *parent) : QDialog(parent), ui(new Ui::InitDialog) {
    ui->setupUi(this);
}

InitDialog::~InitDialog() {
    delete ui;
}

void InitDialog::getDimensions(int *x, int *y) {
    *x = ui->spinBoxXSize->value();
    *y = ui->spinBoxYSize->value();
}
