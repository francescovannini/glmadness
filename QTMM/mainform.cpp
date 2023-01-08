#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include "mainform.h"
#include "ui_mainform.h"
#include "initdialog.h"

MainForm::MainForm(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainForm) {
    ui->setupUi(this);
    sg.Allocate("CIGridNode");
    sg.Allocate("CICubeMapNode");

    initSG();

    idleTimer.setInterval(40);
    idleTimer.start();
    connect(&idleTimer, SIGNAL(timeout()), ui->GLView, SLOT(update()));
}

MainForm::~MainForm() {
    delete ui;
}

void MainForm::fileNew() {
    auto *inid = new InitDialog();
    if (inid->exec()) {
        int vx, vy;
        inid->getDimensions(&vx, &vy);
        sg.g->resize(vx, vy);
        QString buf;
        ui->textGridSize->setText(buf.sprintf("Grid size %i x %i", vx, vy));
    }
}

void MainForm::fileOpen() {
    QString s = QFileDialog::getOpenFileName(this, "Choose one level file", "../levels/", "xml grids (*.xml)");
    sg.XMLRead(s);
    ui->GLView->setDirty(true);
    ui->spinboxlevelTime->setValue(sg.g->time);
}

void MainForm::fileSave() {
    QString s = QFileDialog::getSaveFileName(this, "Choose destination file", "../levels", "xml grids (*.xml)");
    sg.g->time = ui->spinboxlevelTime->value();
    QFile f(s);
    f.open(QFile::WriteOnly);
    QTextStream textStream(&f);
    sg.XMLWrite(textStream);
    f.close();
}

void MainForm::fileExit() {
    exit(0);
}

void MainForm::helpAbout() {
    QMessageBox::about(this, "About GLMadness Level Editor",
                       "Written by F.Vannini for CI2004\nBased on skeleton code by P.Cignoni");
}

void MainForm::editGridSize() {
    int ret = QMessageBox::question(this, "Changing ground size",
                                    "Do you really want to change the size of the ground? It will destroy the geometry!",
                                    QMessageBox::Yes, QMessageBox::Cancel);
    if (ret == QMessageBox::Yes) fileNew();

}

void MainForm::chooseCubeMap() {
    QString s = QFileDialog::getOpenFileName(this,
                                             "Choose one of cube map images (assuming *_negx.jpg *_negx.png format)",
                                             "../data/cubemaps/", "Images (*.png *.jpg)");
    // now strip the ending part of the cubemap name.
    QString extn = s.right(4);
    QString base = s.left(s.length() - 9);
    QString name = base + extn;
    sg.c->textureBaseName = name;
    ui->editCubeMap->setText(name);
    setCubeMap();
}

void MainForm::setCubeMap() {
    QString nm = ui->editCubeMap->text();
    sg.c->textureBaseName = nm;
    sg.c->dirty = true;
}

void MainForm::initSG() {
    ui->GLView->sg = &sg;
    sg.XMLRead("../levels/default.xml");
    ui->spinboxlevelTime->setValue(sg.g->time);
    ui->GLView->setDirty(true);
}

void MainForm::setBallPos() {
    sg.g->setBallOnSelectionCenter();
    sg.g->generateFace();
    ui->GLView->animBall = true;
}

void MainForm::smooth() {
    sg.g->smoothSelected();
}

void MainForm::setAsStart() {
    sg.g->setCellTypeSelection(1);
}

void MainForm::setAsFinish() {
    sg.g->setCellTypeSelection(2);
}

void MainForm::setAsIce() {
    sg.g->setCellTypeSelection(4);
}

void MainForm::setAsGlue() {
    sg.g->setCellTypeSelection(3);
}

void MainForm::resetTrackBall() {
    ui->GLView->resetTB();
}

void MainForm::vSmooth() {
    sg.g->smoothVerticalSelected();
}

void MainForm::hSmooth() {
    sg.g->smoothHorizontalSelected();
}

void MainForm::flatten() {
    sg.g->flattenSelected();
}



