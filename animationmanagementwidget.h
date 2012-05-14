#ifndef ANIMATIONMANAGEMENTWIDGET_H
#define ANIMATIONMANAGEMENTWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>

class QPushButton;
class QDoubleSpinBox;
class QSpinBox;
class QSlider;
class QLabel;

class AnimationManagementWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AnimationManagementWidget(QWidget *parent = 0);

    QPushButton *saveCurrentStateAsStartPointButton;
    QPushButton *saveCurrentStateAsEndPointButton;

    double getCurrentX();
    double getCurrentY();
    double getCurrentZ();

signals:
    void currentTimeChanged();

public slots:
    void saveCurrentCameraStateAsStartPoint(double x, double y, double z, double scale);
    void saveCurrentCameraStateAsEndPoint(double x, double y, double z, double scale);

private slots:
    void updateStartValueFromControls();
    void updateEndValueFromControls();
    void setDuration(int msec);
    void setCurrentTime(int msec);
    void processAnimationTrigger();
    void processTimeout();

private:

    QLabel *currentStateOfAnimationLabel;

    QPushButton *animationStart;

    QTimer *animationTimer;

    QPropertyAnimation xAnimation;
    QPropertyAnimation yAnimation;
    QPropertyAnimation zAnimation;

    QDoubleSpinBox *firstPointXDoubleSpinBox;
    QDoubleSpinBox *firstPointYDoubleSpinBox;
    QDoubleSpinBox *firstPointZDoubleSpinBox;
    QDoubleSpinBox *firstPointScaleDoubleSpinBox;

    QDoubleSpinBox *secondPointXDoubleSpinBox;
    QDoubleSpinBox *secondPointYDoubleSpinBox;
    QDoubleSpinBox *secondPointZDoubleSpinBox;
    QDoubleSpinBox *secondPointScaleDoubleSpinBox;

    QSpinBox *animationPeriodSpinBox;

    QSlider *currentStateOfAnimation;
};

#endif // ANIMATIONMANAGEMENTWIDGET_H
