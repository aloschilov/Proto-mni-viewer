#ifndef ANIMATIONMANAGEMENTWIDGET_H
#define ANIMATIONMANAGEMENTWIDGET_H

#include <QThread>
#include <QMutex>
#include <QWidget>
#include <QPropertyAnimation>

class QPushButton;
class QDoubleSpinBox;
class QSpinBox;
class QSlider;
class QLabel;
class QLineEdit;

class WritingLoopThread : public QThread
{
  Q_OBJECT
public:
    WritingLoopThread()
    {
        exit = false;
    }

    void prepareForExit()
    {
        mutex.lock();
        exit = true;
        mutex.unlock();
    }

protected:


    void run()
    {
        while(1)
        {
            mutex.lock();
            if(exit)
            {
                return;
            }
            mutex.unlock();
            emit writeNextFrame();
        }
    }
signals:
    void writeNextFrame();

private:
    QMutex mutex;
    bool exit;
};

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
    void writingToAviInitiated();
    void currentWritingFrameChanged();
    void writingToAviCompleted();
    void currentFilenameChanged(QString filename);
public slots:
    void saveCurrentCameraStateAsStartPoint(double x, double y, double z, double scale);
    void saveCurrentCameraStateAsEndPoint(double x, double y, double z, double scale);

private slots:
    void updateStartValueFromControls();
    void updateEndValueFromControls();
    void setDuration(int msec);
    void setCurrentTime(int msec);
    void processAnimationTrigger();
    void processWritingTrigger();

    void processTimeout();
    void processWritingFrame();

    void processSpecifyFilename();
private:

    QLabel *currentStateOfAnimationLabel;
    QLineEdit *pathToSaveAviFileLineEdit;

    QPushButton *animationStart;
    QPushButton *writingStart;
    QPushButton *specifyFilenameButton;

    QTimer *animationTimer;
    WritingLoopThread *writingLoopThread;

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