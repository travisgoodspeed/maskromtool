#ifndef ROMTHRESHOLDDIALOG_H
#define ROMTHRESHOLDDIALOG_H

#include <QDialog>
#include <QtCharts>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QT_CHARTS_USE_NAMESPACE
#endif

class MaskRomTool;

namespace Ui {
class RomThresholdDialog;
}

class RomThresholdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RomThresholdDialog();
    ~RomThresholdDialog();
    void postThresholds();
    void setMaskRomTool(MaskRomTool* parent);
    void setChart(QChart *chart);
    void refreshStats(bool remarkbits=true);

private:
    Ui::RomThresholdDialog *ui;
    float average_red=0, average_green=0, average_blue=0;
    QChartView *chartview=0;
    MaskRomTool *mrt=0;


private slots:
    void on_averageButton_clicked();
    void on_redScrollBar_valueChanged(int value);
    void on_greenScrollBar_valueChanged(int value);
    void on_blueScrollBar_valueChanged(int value);

    void on_redEdit_textEdited(const QString &arg1);
    void on_greenEdit_textEdited(const QString &arg1);
    void on_blueEdit_textEdited(const QString &arg1);
    void on_sizeScrollBar_valueChanged(int value);
    void on_samplerBox_activated(int index);
    void on_samplesizeEdit_textEdited(const QString &arg1);
    void on_samplesizeScrollBar_valueChanged(int value);
    void on_checkInverted_stateChanged(int arg1);
    void on_redScrollBar_sliderPressed();
    void on_greenScrollBar_sliderPressed();
    void on_blueScrollBar_sliderPressed();
    void on_samplesizeScrollBar_sliderPressed();
    void on_redScrollBar_sliderReleased();
    void on_greenScrollBar_sliderReleased();
    void on_blueScrollBar_sliderReleased();
    void on_samplesizeScrollBar_sliderReleased();
};

#endif // ROMTHRESHOLDDIALOG_H
