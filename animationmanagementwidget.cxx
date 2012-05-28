#include <QtGui>

#include "animationmanagementwidget.h"

AnimationManagementWidget::AnimationManagementWidget(QWidget *parent) :
    QWidget(parent)
{
    saveCurrentStateAsStartPointButton = new QPushButton(tr("Grab from current state"));
    saveCurrentStateAsEndPointButton = new QPushButton(tr("Grab from current state"));

    firstPointXDoubleSpinBox = new QDoubleSpinBox();
    firstPointYDoubleSpinBox = new QDoubleSpinBox();
    firstPointZDoubleSpinBox = new QDoubleSpinBox();
    firstPointScaleDoubleSpinBox = new QDoubleSpinBox();
    firstPointRotXDoubleSpinBox = new QDoubleSpinBox();
    firstPointRotYDoubleSpinBox = new QDoubleSpinBox();
    firstPointRotZDoubleSpinBox = new QDoubleSpinBox();

    firstPointXDoubleSpinBox->setRange(-10000, 10000);
    firstPointYDoubleSpinBox->setRange(-10000, 10000);
    firstPointZDoubleSpinBox->setRange(-10000, 10000);
    firstPointScaleDoubleSpinBox->setRange(-10000, 10000);
    firstPointRotXDoubleSpinBox->setRange(-10000, 10000);
    firstPointRotYDoubleSpinBox->setRange(-10000, 10000);
    firstPointRotZDoubleSpinBox->setRange(-10000, 10000);

    QLabel *firstPointXLabel = new QLabel(tr("X"));
    QLabel *firstPointYLabel = new QLabel(tr("Y"));
    QLabel *firstPointZLabel = new QLabel(tr("Z"));
    QLabel *firstPointRotXLabel = new QLabel(tr("Rot X"));
    QLabel *firstPointRotYLabel = new QLabel(tr("Rot Y"));
    QLabel *firstPointRotZLabel = new QLabel(tr("Rot Z"));
    QLabel *firstPointScaleLabel = new QLabel(tr("Scale"));

    secondPointXDoubleSpinBox = new QDoubleSpinBox();
    secondPointYDoubleSpinBox = new QDoubleSpinBox();
    secondPointZDoubleSpinBox = new QDoubleSpinBox();
    secondPointScaleDoubleSpinBox = new QDoubleSpinBox();
    secondPointRotXDoubleSpinBox = new QDoubleSpinBox();
    secondPointRotYDoubleSpinBox = new QDoubleSpinBox();
    secondPointRotZDoubleSpinBox = new QDoubleSpinBox();

    secondPointXDoubleSpinBox->setRange(-10000, 10000);
    secondPointYDoubleSpinBox->setRange(-10000, 10000);
    secondPointZDoubleSpinBox->setRange(-10000, 10000);
    secondPointScaleDoubleSpinBox->setRange(-10000, 10000);
    secondPointRotXDoubleSpinBox->setRange(-10000, 10000);
    secondPointRotYDoubleSpinBox->setRange(-10000, 10000);
    secondPointRotZDoubleSpinBox->setRange(-10000, 10000);

    QLabel *secondPointXLabel = new QLabel(tr("X"));
    QLabel *secondPointYLabel = new QLabel(tr("Y"));
    QLabel *secondPointZLabel = new QLabel(tr("Z"));
    QLabel *secondPointRotXLabel = new QLabel(tr("Rot X"));
    QLabel *secondPointRotYLabel = new QLabel(tr("Rot Y"));
    QLabel *secondPointRotZLabel = new QLabel(tr("Rot Z"));
    QLabel *secondPointScaleLabel = new QLabel(tr("Scale"));

    QGroupBox *firstPointGroupBox = new QGroupBox;
    firstPointGroupBox->setTitle(tr("1-st animation point"));

    QGridLayout *firstPointGroupBoxLayout = new QGridLayout();
    firstPointGroupBox->setLayout(firstPointGroupBoxLayout);
    firstPointGroupBoxLayout->addWidget(firstPointXLabel, 0, 0);
    firstPointGroupBoxLayout->addWidget(firstPointXDoubleSpinBox, 0, 1);

    firstPointGroupBoxLayout->addWidget(firstPointYLabel, 1, 0);
    firstPointGroupBoxLayout->addWidget(firstPointYDoubleSpinBox, 1, 1);

    firstPointGroupBoxLayout->addWidget(firstPointZLabel, 2, 0);
    firstPointGroupBoxLayout->addWidget(firstPointZDoubleSpinBox, 2, 1);

    firstPointGroupBoxLayout->addWidget(firstPointScaleLabel, 3, 0);
    firstPointGroupBoxLayout->addWidget(firstPointScaleDoubleSpinBox, 3, 1);

    firstPointGroupBoxLayout->addWidget(firstPointRotXLabel, 4, 0);
    firstPointGroupBoxLayout->addWidget(firstPointRotXDoubleSpinBox, 4, 1);

    firstPointGroupBoxLayout->addWidget(firstPointRotYLabel, 5, 0);
    firstPointGroupBoxLayout->addWidget(firstPointRotYDoubleSpinBox, 5, 1);

    firstPointGroupBoxLayout->addWidget(firstPointRotZLabel, 6, 0);
    firstPointGroupBoxLayout->addWidget(firstPointRotZDoubleSpinBox, 6, 1);

    firstPointGroupBoxLayout->addWidget(saveCurrentStateAsStartPointButton, 7, 0, 1, 2);


    QGroupBox *secondPointGroupBox = new QGroupBox;
    secondPointGroupBox->setTitle(tr("2-nd animation point"));

    QGridLayout *secondPointGroupBoxLayout = new QGridLayout();
    secondPointGroupBoxLayout->addWidget(secondPointXLabel, 0, 0);
    secondPointGroupBoxLayout->addWidget(secondPointXDoubleSpinBox, 0, 1);

    secondPointGroupBoxLayout->addWidget(secondPointYLabel, 1, 0);
    secondPointGroupBoxLayout->addWidget(secondPointYDoubleSpinBox, 1, 1);

    secondPointGroupBoxLayout->addWidget(secondPointZLabel, 2, 0);
    secondPointGroupBoxLayout->addWidget(secondPointZDoubleSpinBox, 2, 1);

    secondPointGroupBoxLayout->addWidget(secondPointScaleLabel, 3, 0);
    secondPointGroupBoxLayout->addWidget(secondPointScaleDoubleSpinBox, 3, 1);

    secondPointGroupBoxLayout->addWidget(secondPointRotXLabel, 4, 0);
    secondPointGroupBoxLayout->addWidget(secondPointRotXDoubleSpinBox, 4, 1);

    secondPointGroupBoxLayout->addWidget(secondPointRotYLabel, 5, 0);
    secondPointGroupBoxLayout->addWidget(secondPointRotYDoubleSpinBox, 5, 1);

    secondPointGroupBoxLayout->addWidget(secondPointRotZLabel, 6, 0);
    secondPointGroupBoxLayout->addWidget(secondPointRotZDoubleSpinBox, 6, 1);

    secondPointGroupBoxLayout->addWidget(saveCurrentStateAsEndPointButton, 7, 0, 1, 2);

    secondPointGroupBox->setLayout(secondPointGroupBoxLayout);


    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(firstPointGroupBox);
    mainLayout->addWidget(secondPointGroupBox);

    animationPeriodSpinBox = new QSpinBox();
    animationPeriodSpinBox->setRange(0, 1000000);

    QHBoxLayout *animationPeriodLayout = new QHBoxLayout();
    QLabel *animationPeriodLabel = new QLabel(tr("Animation period in msec:"));
    animationPeriodLayout->addWidget(animationPeriodLabel);
    animationPeriodLayout->addWidget(animationPeriodSpinBox);

    mainLayout->addLayout(animationPeriodLayout);


    currentStateOfAnimation = new QSlider(Qt::Horizontal);
    currentStateOfAnimationLabel = new QLabel();
    currentStateOfAnimationLabel->setText(QString("%1").arg(currentStateOfAnimation->value()));

    QHBoxLayout *animationStateLayout = new QHBoxLayout();
    animationStart = new QPushButton(tr("Start"));
    animationStateLayout->addWidget(animationStart);
    animationStateLayout->addWidget(currentStateOfAnimation);
    animationStateLayout->addWidget(currentStateOfAnimationLabel);
    mainLayout->addLayout(animationStateLayout);

    writingStart = new QPushButton(tr("Write"));
    writingStart->setEnabled(false);
    specifyFilenameButton = new QPushButton(tr("Specify filename"));
    pathToSaveAviFileLineEdit = new QLineEdit();
    pathToSaveAviFileLineEdit->setReadOnly(true);

    QGridLayout *aviConfigurationLayout = new QGridLayout();

    aviConfigurationLayout->addWidget(specifyFilenameButton, 0, 0);
    aviConfigurationLayout->addWidget(pathToSaveAviFileLineEdit, 0, 1);
    aviConfigurationLayout->addWidget(writingStart, 1, 0);

    mainLayout->addLayout(aviConfigurationLayout);

    mainLayout->addStretch();
    setLayout(mainLayout);

    animationTimer = new QTimer(this);
    connect(animationTimer, SIGNAL(timeout()), this, SLOT(processTimeout()));

    connect(animationPeriodSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(setDuration(int)));
    connect(currentStateOfAnimation, SIGNAL(valueChanged(int)),
            this, SLOT(setCurrentTime(int)));
    connect(animationStart, SIGNAL(clicked()),
            this, SLOT(processAnimationTrigger()));
    connect(writingStart, SIGNAL(clicked()),
            this, SLOT(processWritingTrigger()));

    connect(specifyFilenameButton, SIGNAL(clicked()),
            this, SLOT(processSpecifyFilename()));

    writingLoopThread = new WritingLoopThread();
    writingLoopThread->setParent(this);
}

double AnimationManagementWidget::getCurrentX()
{
    return xAnimation.currentValue().toDouble();
}

double AnimationManagementWidget::getCurrentY()
{
    return yAnimation.currentValue().toDouble();
}

double AnimationManagementWidget::getCurrentZ()
{
    return zAnimation.currentValue().toDouble();
}


double AnimationManagementWidget::getCurrentRotX()
{
    return xRotAnimation.currentValue().toDouble();
}

double AnimationManagementWidget::getCurrentRotY()
{
    return yRotAnimation.currentValue().toDouble();
}

double AnimationManagementWidget::getCurrentRotZ()
{
    return zRotAnimation.currentValue().toDouble();
}

void AnimationManagementWidget::saveCurrentObjectStateAsStartPoint(double x, double y, double z, double scale, double rotX, double rotY, double rotZ)
{
    firstPointXDoubleSpinBox->setValue(x);
    firstPointYDoubleSpinBox->setValue(y);
    firstPointZDoubleSpinBox->setValue(z);
    firstPointRotXDoubleSpinBox->setValue(rotX);
    firstPointRotYDoubleSpinBox->setValue(rotY);
    firstPointRotZDoubleSpinBox->setValue(rotZ);
    firstPointScaleDoubleSpinBox->setValue(scale);

    updateStartValueFromControls();
}

void AnimationManagementWidget::saveCurrentObjectStateAsEndPoint(double x, double y, double z, double scale, double rotX, double rotY, double rotZ)
{
    secondPointXDoubleSpinBox->setValue(x);
    secondPointYDoubleSpinBox->setValue(y);
    secondPointZDoubleSpinBox->setValue(z);
    secondPointRotXDoubleSpinBox->setValue(rotX);
    secondPointRotYDoubleSpinBox->setValue(rotY);
    secondPointRotZDoubleSpinBox->setValue(rotZ);
    secondPointScaleDoubleSpinBox->setValue(scale);

    updateEndValueFromControls();
}

void AnimationManagementWidget::processFrameIsWritten()
{
    writingLoopThread->allowNextSignal();
}

void AnimationManagementWidget::updateStartValueFromControls()
{
    xAnimation.setStartValue(firstPointXDoubleSpinBox->value());
    yAnimation.setStartValue(firstPointYDoubleSpinBox->value());
    zAnimation.setStartValue(firstPointZDoubleSpinBox->value());
    xRotAnimation.setStartValue(firstPointRotXDoubleSpinBox->value());
    yRotAnimation.setStartValue(firstPointRotYDoubleSpinBox->value());
    zRotAnimation.setStartValue(firstPointRotZDoubleSpinBox->value());
}

void AnimationManagementWidget::updateEndValueFromControls()
{
    xAnimation.setEndValue(secondPointXDoubleSpinBox->value());
    yAnimation.setEndValue(secondPointYDoubleSpinBox->value());
    zAnimation.setEndValue(secondPointZDoubleSpinBox->value());
    xRotAnimation.setEndValue(secondPointRotXDoubleSpinBox->value());
    yRotAnimation.setEndValue(secondPointRotYDoubleSpinBox->value());
    zRotAnimation.setEndValue(secondPointRotZDoubleSpinBox->value());
}

void AnimationManagementWidget::setDuration(int msec)
{
    xAnimation.setDuration(msec);
    yAnimation.setDuration(msec);
    zAnimation.setDuration(msec);
    xRotAnimation.setDuration(msec);
    yRotAnimation.setDuration(msec);
    zRotAnimation.setDuration(msec);

    currentStateOfAnimation->setRange(0, msec);
}

void AnimationManagementWidget::setCurrentTime(int msec)
{
    xAnimation.setCurrentTime(msec);
    yAnimation.setCurrentTime(msec);
    zAnimation.setCurrentTime(msec);
    xRotAnimation.setCurrentTime(msec);
    yRotAnimation.setCurrentTime(msec);
    zRotAnimation.setCurrentTime(msec);

    currentStateOfAnimationLabel->setText(QString("%1").arg(currentStateOfAnimation->value()));

    emit currentTimeChanged();
}

void AnimationManagementWidget::processAnimationTrigger()
{
    if(!animationTimer->isActive())
    {
        animationStart->setText(tr("Stop"));
        animationTimer->start(100);
        currentStateOfAnimation->setValue(0);
    }
    else
    {
        animationStart->setText(tr("Start"));
        animationTimer->stop();
        currentStateOfAnimation->setValue(0);
    }
}

void AnimationManagementWidget::processWritingTrigger()
{
    currentStateOfAnimation->setValue(0);
    emit writingToAviInitiated();
    connect(writingLoopThread, SIGNAL(writeNextFrame()),
            this, SLOT(processWritingFrame()));
    writingLoopThread->start();
}

void AnimationManagementWidget::processTimeout()
{
    currentStateOfAnimation->setValue(currentStateOfAnimation->value()+100);
    if(currentStateOfAnimation->value()==currentStateOfAnimation->maximum())
    {
        animationStart->setText(tr("Start"));
        animationTimer->stop();
        currentStateOfAnimation->setValue(0);
    }
}

void AnimationManagementWidget::processWritingFrame()
{
    if(!writingLoopThread->isRunning())
    {
        return;
    }
    currentStateOfAnimation->setValue(currentStateOfAnimation->value()+100);
    if(currentStateOfAnimation->value() >= currentStateOfAnimation->maximum())
    {
        disconnect(writingLoopThread, SIGNAL(writeNextFrame()),
                this, SLOT(processWritingFrame()));
        writingLoopThread->prepareForExit();
        writingLoopThread->allowNextSignal();
        writingLoopThread->wait();
        writingLoopThread->terminate();
        currentStateOfAnimation->setValue(0);
        emit writingToAviCompleted();
        return;
    }

    emit currentWritingFrameChanged();
}

void AnimationManagementWidget::processSpecifyFilename()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Specify animation file to write"),
                                                    "",
                                                    tr("Animation file(*.avi)"));

    if(fileName.isEmpty())
    {
        return;
    }

    pathToSaveAviFileLineEdit->setText(fileName);
    writingStart->setEnabled(true);

    emit currentFilenameChanged(fileName);
}
