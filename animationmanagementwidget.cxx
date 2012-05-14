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

    firstPointXDoubleSpinBox->setRange(-10000, 10000);
    firstPointYDoubleSpinBox->setRange(-10000, 10000);
    firstPointZDoubleSpinBox->setRange(-10000, 10000);
    firstPointScaleDoubleSpinBox->setRange(-10000, 10000);

    QLabel *firstPointXLabel = new QLabel(tr("X"));
    QLabel *firstPointYLabel = new QLabel(tr("Y"));
    QLabel *firstPointZLabel = new QLabel(tr("Z"));
    QLabel *firstPointScaleLabel = new QLabel(tr("Scale"));

    secondPointXDoubleSpinBox = new QDoubleSpinBox();
    secondPointYDoubleSpinBox = new QDoubleSpinBox();
    secondPointZDoubleSpinBox = new QDoubleSpinBox();
    secondPointScaleDoubleSpinBox = new QDoubleSpinBox();

    secondPointXDoubleSpinBox->setRange(-10000, 10000);
    secondPointYDoubleSpinBox->setRange(-10000, 10000);
    secondPointZDoubleSpinBox->setRange(-10000, 10000);
    secondPointScaleDoubleSpinBox->setRange(-10000, 10000);

    QLabel *secondPointXLabel = new QLabel(tr("X"));
    QLabel *secondPointYLabel = new QLabel(tr("Y"));
    QLabel *secondPointZLabel = new QLabel(tr("Z"));
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

    firstPointGroupBoxLayout->addWidget(saveCurrentStateAsStartPointButton, 4, 0, 1, 2);


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

    secondPointGroupBoxLayout->addWidget(saveCurrentStateAsEndPointButton, 4, 0, 1, 2);

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

void AnimationManagementWidget::saveCurrentCameraStateAsStartPoint(double x, double y, double z, double scale)
{
    firstPointXDoubleSpinBox->setValue(x);
    firstPointYDoubleSpinBox->setValue(y);
    firstPointZDoubleSpinBox->setValue(z);
    firstPointScaleDoubleSpinBox->setValue(scale);

    updateStartValueFromControls();
}

void AnimationManagementWidget::saveCurrentCameraStateAsEndPoint(double x, double y, double z, double scale)
{
    secondPointXDoubleSpinBox->setValue(x);
    secondPointYDoubleSpinBox->setValue(y);
    secondPointZDoubleSpinBox->setValue(z);
    secondPointScaleDoubleSpinBox->setValue(scale);

    updateEndValueFromControls();
}

void AnimationManagementWidget::updateStartValueFromControls()
{
    xAnimation.setStartValue(firstPointXDoubleSpinBox->value());
    yAnimation.setStartValue(firstPointYDoubleSpinBox->value());
    zAnimation.setStartValue(firstPointZDoubleSpinBox->value());
}

void AnimationManagementWidget::updateEndValueFromControls()
{
    xAnimation.setEndValue(secondPointXDoubleSpinBox->value());
    yAnimation.setEndValue(secondPointYDoubleSpinBox->value());
    zAnimation.setEndValue(secondPointZDoubleSpinBox->value());
}

void AnimationManagementWidget::setDuration(int msec)
{
    xAnimation.setDuration(msec);
    yAnimation.setDuration(msec);
    zAnimation.setDuration(msec);

    currentStateOfAnimation->setRange(0, msec);
}

void AnimationManagementWidget::setCurrentTime(int msec)
{
    xAnimation.setCurrentTime(msec);
    yAnimation.setCurrentTime(msec);
    zAnimation.setCurrentTime(msec);

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
