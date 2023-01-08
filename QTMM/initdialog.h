#ifndef QTMM_INITDIALOG_H
#define QTMM_INITDIALOG_H

#include <QSpinBox>
#include <QDialog>

namespace Ui {
    class InitDialog;
}

class InitDialog : public QDialog {
Q_OBJECT

public:
    explicit InitDialog(QWidget *parent = nullptr);

    ~InitDialog() override;

    void getDimensions(int *x, int *y);

private slots:

private:
    Ui::InitDialog *ui;
};


#endif //QTMM_INITDIALOG_H
