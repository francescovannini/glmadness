#ifndef QTMM_MAINFORM_H
#define QTMM_MAINFORM_H

#include <QMainWindow>
#include <QTimer>
#include "CIGLWidget.h"

namespace Ui {
    class MainForm;
}

class MainForm : public QMainWindow {
Q_OBJECT

public:
    explicit MainForm(QWidget *parent = nullptr);

    ~MainForm() override;

private slots:

    void helpAbout();

    void setAsGlue();

    void fileNew();

    void fileOpen();

    void fileSave();

    void fileExit();

    void editGridSize();

    void chooseCubeMap();

    void setCubeMap();

    void initSG();

    void setBallPos();

    void smooth();

    void setAsStart();

    void setAsFinish();

    void setAsIce();

    void vSmooth();

    void hSmooth();

    void resetTrackBall();

    void flatten();

private:
    Ui::MainForm *ui;
    vcg::MMSG sg;
    QTimer idleTimer;
};

#endif //QTMM_MAINFORM_H
