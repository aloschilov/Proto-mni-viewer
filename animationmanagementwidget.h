#ifndef ANIMATIONMANAGEMENTWIDGET_H
#define ANIMATIONMANAGEMENTWIDGET_H

#include <QThread>
#include <QMutex>
#include <QWidget>
#include <QDebug>
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

    void allowNextSignal()
    {
        singal_mutex.unlock();
    }

protected:


    void run()
    {
        exit = false;
        mutex.unlock();
        while(1)
        {
            qDebug() << "mutex.lock();";
            mutex.lock();
            if(exit)
            {
                qDebug() << "about to return";
                return;
            }
            qDebug() << "mutex.unlock()";
            mutex.unlock();
            qDebug() << "emit writeNextFrame();";
            emit writeNextFrame();
            if(!exit)
            {
                singal_mutex.lock();
            }
            usleep(10);
        }
    }
signals:
    void writeNextFrame();

private:
    QMutex mutex;
    QMutex singal_mutex;
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

    double getCurrentRotX();
    double getCurrentRotY();
    double getCurrentRotZ();

signals:
    void currentTimeChanged();
    void writingToAviInitiated();
    void currentWritingFrameChanged();
    void writingToAviCompleted();
    void currentAviFilenameChanged(QString filename);
    void currentPngFilenameChanged(QString filename);
    void pngWritingRequested();
    void tranformationResetionRequested();
public slots:
    void saveCurrentObjectStateAsStartPoint(double x, double y, double z, double rotX, double rotY, double rotZ);
    void saveCurrentObjectStateAsEndPoint(double x, double y, double z, double rotX, double rotY, double rotZ);
    void processFrameIsWritten();
    void setAviFilename(QString fileName);
    void setPngFilename(QString fileName);

private slots:
    void updateStartValueFromControls();
    void updateEndValueFromControls();
    void setDuration(int msec);
    void setCurrentTime(int msec);
    void processAnimationTrigger();
    void processWritingTrigger();

    void processTimeout();
    void processWritingFrame();

    void processSpecifyAviFilename();
    void processSpecifyPngFilename();

    void processFirstPointXDoubleSpinBoxValueChanged(double value);
    void processFirstPointYDoubleSpinBoxValueChanged(double value);
    void processFirstPointZDoubleSpinBoxValueChanged(double value);

    void processFirstPointRotXDoubleSpinBoxValueChanged(double value);
    void processFirstPointRotYDoubleSpinBoxValueChanged(double value);
    void processFirstPointRotZDoubleSpinBoxValueChanged(double value);

    void processSecondPointXDoubleSpinBoxValueChanged(double value);
    void processSecondPointYDoubleSpinBoxValueChanged(double value);
    void processSecondPointZDoubleSpinBoxValueChanged(double value);

    void processSecondPointRotXDoubleSpinBoxValueChanged(double value);
    void processSecondPointRotYDoubleSpinBoxValueChanged(double value);
    void processSecondPointRotZDoubleSpinBoxValueChanged(double value);

    void processPngWrite();

private:

    QLabel *currentStateOfAnimationLabel;
    QLineEdit *pathToSaveAviFileLineEdit;


    QPushButton *animationStart;
    QPushButton *writingStart;
    QPushButton *specifyAviFilenameButton;

    QLineEdit *pathToSavePngFileLineEdit;
    QPushButton *specifyPngFilenameButton;
    QPushButton *pngWrite;

    QTimer *animationTimer;
    WritingLoopThread *writingLoopThread;

    QPropertyAnimation xAnimation;
    QPropertyAnimation yAnimation;
    QPropertyAnimation zAnimation;
    QPropertyAnimation xRotAnimation;
    QPropertyAnimation yRotAnimation;
    QPropertyAnimation zRotAnimation;


    QDoubleSpinBox *firstPointXDoubleSpinBox;
    QDoubleSpinBox *firstPointYDoubleSpinBox;
    QDoubleSpinBox *firstPointZDoubleSpinBox;

    QDoubleSpinBox *firstPointRotXDoubleSpinBox;
    QDoubleSpinBox *firstPointRotYDoubleSpinBox;
    QDoubleSpinBox *firstPointRotZDoubleSpinBox;

    QDoubleSpinBox *secondPointXDoubleSpinBox;
    QDoubleSpinBox *secondPointYDoubleSpinBox;
    QDoubleSpinBox *secondPointZDoubleSpinBox;

    QDoubleSpinBox *secondPointRotXDoubleSpinBox;
    QDoubleSpinBox *secondPointRotYDoubleSpinBox;
    QDoubleSpinBox *secondPointRotZDoubleSpinBox;

    QPushButton *resetObjectTransformationButton;

    QSpinBox *animationPeriodSpinBox;

    QSlider *currentStateOfAnimation;
};

#endif // ANIMATIONMANAGEMENTWIDGET_H
