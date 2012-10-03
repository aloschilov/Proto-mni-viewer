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
    firstPointRotXDoubleSpinBox = new QDoubleSpinBox();
    firstPointRotYDoubleSpinBox = new QDoubleSpinBox();
    firstPointRotZDoubleSpinBox = new QDoubleSpinBox();

    firstPointXDoubleSpinBox->setRange(-10000, 10000);
    firstPointYDoubleSpinBox->setRange(-10000, 10000);
    firstPointZDoubleSpinBox->setRange(-10000, 10000);
    firstPointRotXDoubleSpinBox->setRange(-10000, 10000);
    firstPointRotYDoubleSpinBox->setRange(-10000, 10000);
    firstPointRotZDoubleSpinBox->setRange(-10000, 10000);

    QLabel *firstPointXLabel = new QLabel(tr("X"));
    QLabel *firstPointYLabel = new QLabel(tr("Y"));
    QLabel *firstPointZLabel = new QLabel(tr("Z"));
    QLabel *firstPointRotXLabel = new QLabel(tr("Rot X"));
    QLabel *firstPointRotYLabel = new QLabel(tr("Rot Y"));
    QLabel *firstPointRotZLabel = new QLabel(tr("Rot Z"));

    secondPointXDoubleSpinBox = new QDoubleSpinBox();
    secondPointYDoubleSpinBox = new QDoubleSpinBox();
    secondPointZDoubleSpinBox = new QDoubleSpinBox();
    secondPointRotXDoubleSpinBox = new QDoubleSpinBox();
    secondPointRotYDoubleSpinBox = new QDoubleSpinBox();
    secondPointRotZDoubleSpinBox = new QDoubleSpinBox();

    secondPointXDoubleSpinBox->setRange(-10000, 10000);
    secondPointYDoubleSpinBox->setRange(-10000, 10000);
    secondPointZDoubleSpinBox->setRange(-10000, 10000);
    secondPointRotXDoubleSpinBox->setRange(-10000, 10000);
    secondPointRotYDoubleSpinBox->setRange(-10000, 10000);
    secondPointRotZDoubleSpinBox->setRange(-10000, 10000);

    QLabel *secondPointXLabel = new QLabel(tr("X"));
    QLabel *secondPointYLabel = new QLabel(tr("Y"));
    QLabel *secondPointZLabel = new QLabel(tr("Z"));
    QLabel *secondPointRotXLabel = new QLabel(tr("Rot X"));
    QLabel *secondPointRotYLabel = new QLabel(tr("Rot Y"));
    QLabel *secondPointRotZLabel = new QLabel(tr("Rot Z"));

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

    firstPointGroupBoxLayout->addWidget(firstPointRotXLabel, 3, 0);
    firstPointGroupBoxLayout->addWidget(firstPointRotXDoubleSpinBox, 3, 1);

    firstPointGroupBoxLayout->addWidget(firstPointRotYLabel, 4, 0);
    firstPointGroupBoxLayout->addWidget(firstPointRotYDoubleSpinBox, 4, 1);

    firstPointGroupBoxLayout->addWidget(firstPointRotZLabel, 5, 0);
    firstPointGroupBoxLayout->addWidget(firstPointRotZDoubleSpinBox, 5, 1);

    firstPointGroupBoxLayout->addWidget(saveCurrentStateAsStartPointButton, 6, 0, 1, 2);


    QGroupBox *secondPointGroupBox = new QGroupBox;
    secondPointGroupBox->setTitle(tr("2-nd animation point"));

    QGridLayout *secondPointGroupBoxLayout = new QGridLayout();
    secondPointGroupBoxLayout->addWidget(secondPointXLabel, 0, 0);
    secondPointGroupBoxLayout->addWidget(secondPointXDoubleSpinBox, 0, 1);

    secondPointGroupBoxLayout->addWidget(secondPointYLabel, 1, 0);
    secondPointGroupBoxLayout->addWidget(secondPointYDoubleSpinBox, 1, 1);

    secondPointGroupBoxLayout->addWidget(secondPointZLabel, 2, 0);
    secondPointGroupBoxLayout->addWidget(secondPointZDoubleSpinBox, 2, 1);

    secondPointGroupBoxLayout->addWidget(secondPointRotXLabel, 3, 0);
    secondPointGroupBoxLayout->addWidget(secondPointRotXDoubleSpinBox, 3, 1);

    secondPointGroupBoxLayout->addWidget(secondPointRotYLabel, 4, 0);
    secondPointGroupBoxLayout->addWidget(secondPointRotYDoubleSpinBox, 4, 1);

    secondPointGroupBoxLayout->addWidget(secondPointRotZLabel, 5, 0);
    secondPointGroupBoxLayout->addWidget(secondPointRotZDoubleSpinBox, 5, 1);

    secondPointGroupBoxLayout->addWidget(saveCurrentStateAsEndPointButton, 6, 0, 1, 2);

    secondPointGroupBox->setLayout(secondPointGroupBoxLayout);


    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(firstPointGroupBox);
    mainLayout->addWidget(secondPointGroupBox);

    resetObjectTransformationButton = new QPushButton(tr("Reset transformation"));
    mainLayout->addWidget(resetObjectTransformationButton);

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
    specifyAviFilenameButton = new QPushButton(tr("Specify filename"));
    pathToSaveAviFileLineEdit = new QLineEdit();
    pathToSaveAviFileLineEdit->setReadOnly(true);

    QGridLayout *aviConfigurationLayout = new QGridLayout();

    aviConfigurationLayout->addWidget(specifyAviFilenameButton, 0, 0);
    aviConfigurationLayout->addWidget(pathToSaveAviFileLineEdit, 0, 1);
    aviConfigurationLayout->addWidget(writingStart, 1, 0);

    // PNG START

    pngWrite = new QPushButton(tr("Write frame"));
    pngWrite->setEnabled(false);
    specifyPngFilenameButton = new QPushButton(tr("Specify filename"));
    pathToSavePngFileLineEdit = new QLineEdit();
    pathToSavePngFileLineEdit->setReadOnly(true);

    QGridLayout *pngConfigurationLayout = new QGridLayout();

    pngConfigurationLayout->addWidget(specifyPngFilenameButton, 0, 0);
    pngConfigurationLayout->addWidget(pathToSavePngFileLineEdit, 0, 1);
    pngConfigurationLayout->addWidget(pngWrite);

    // PNG END

    mainLayout->addLayout(aviConfigurationLayout);
    mainLayout->addLayout(pngConfigurationLayout);

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

    connect(specifyAviFilenameButton, SIGNAL(clicked()),
            this, SLOT(processSpecifyAviFilename()));

    connect(specifyPngFilenameButton, SIGNAL(clicked()),
            this, SLOT(processSpecifyPngFilename()));


    connect(firstPointXDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processFirstPointXDoubleSpinBoxValueChanged(double)));
    connect(firstPointYDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processFirstPointYDoubleSpinBoxValueChanged(double)));
    connect(firstPointZDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processFirstPointZDoubleSpinBoxValueChanged(double)));
    connect(firstPointRotXDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processFirstPointRotXDoubleSpinBoxValueChanged(double)));
    connect(firstPointRotYDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processFirstPointRotYDoubleSpinBoxValueChanged(double)));
    connect(firstPointRotZDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processFirstPointRotZDoubleSpinBoxValueChanged(double)));

    connect(secondPointXDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processSecondPointXDoubleSpinBoxValueChanged(double)));
    connect(secondPointYDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processSecondPointYDoubleSpinBoxValueChanged(double)));
    connect(secondPointZDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processSecondPointZDoubleSpinBoxValueChanged(double)));

    connect(secondPointRotXDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processSecondPointRotXDoubleSpinBoxValueChanged(double)));
    connect(secondPointRotYDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processSecondPointRotYDoubleSpinBoxValueChanged(double)));
    connect(secondPointRotZDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processSecondPointRotZDoubleSpinBoxValueChanged(double)));

    connect(pngWrite, SIGNAL(clicked()),
            this, SLOT(processPngWrite()));

    connect(resetObjectTransformationButton, SIGNAL(clicked()),
            this, SIGNAL(tranformationResetionRequested()));

    writingLoopThread = new WritingLoopThread();
    writingLoopThread->setParent(this);

    setDuration(0);
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

void AnimationManagementWidget::saveCurrentObjectStateAsStartPoint(double x, double y, double z, double rotX, double rotY, double rotZ)
{
    firstPointXDoubleSpinBox->setValue(x);
    firstPointYDoubleSpinBox->setValue(y);
    firstPointZDoubleSpinBox->setValue(z);
    firstPointRotXDoubleSpinBox->setValue(rotX);
    firstPointRotYDoubleSpinBox->setValue(rotY);
    firstPointRotZDoubleSpinBox->setValue(rotZ);

    updateValuesFromControls();
}

void AnimationManagementWidget::saveCurrentObjectStateAsEndPoint(double x, double y, double z, double rotX, double rotY, double rotZ)
{
    secondPointXDoubleSpinBox->setValue(x);
    secondPointYDoubleSpinBox->setValue(y);
    secondPointZDoubleSpinBox->setValue(z);
    secondPointRotXDoubleSpinBox->setValue(rotX);
    secondPointRotYDoubleSpinBox->setValue(rotY);
    secondPointRotZDoubleSpinBox->setValue(rotZ);

    updateValuesFromControls();
}

void AnimationManagementWidget::processFrameIsWritten()
{
    writingLoopThread->allowNextSignal();
}

void AnimationManagementWidget::setAviFilename(QString fileName)
{
    pathToSaveAviFileLineEdit->setText(fileName);
    writingStart->setEnabled(true);

    emit currentAviFilenameChanged(fileName);
}

void AnimationManagementWidget::setPngFilename(QString fileName)
{
    pathToSavePngFileLineEdit->setText(fileName);
    pngWrite->setEnabled(true);

    emit currentPngFilenameChanged(fileName);
}

void AnimationManagementWidget::updateValuesFromControls()
{
    xAnimation.setStartValue(firstPointXDoubleSpinBox->value());
    yAnimation.setStartValue(firstPointYDoubleSpinBox->value());
    zAnimation.setStartValue(firstPointZDoubleSpinBox->value());
    xRotAnimation.setStartValue(firstPointRotXDoubleSpinBox->value());
    yRotAnimation.setStartValue(firstPointRotYDoubleSpinBox->value());
    zRotAnimation.setStartValue(firstPointRotZDoubleSpinBox->value());

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
    qDebug() << "void AnimationManagementWidget::setCurrentTime(int msec)";
    qDebug() << "msec:"  << msec;
    xAnimation.setCurrentTime(msec);
    yAnimation.setCurrentTime(msec);
    zAnimation.setCurrentTime(msec);
    xRotAnimation.setCurrentTime(msec);
    yRotAnimation.setCurrentTime(msec);
    zRotAnimation.setCurrentTime(msec);


    qDebug() << "xAnimation.currentValue().toDouble():"  << xAnimation.currentValue().toDouble();
    qDebug() << "yAnimation.currentValue().toDouble():"  << yAnimation.currentValue().toDouble();
    qDebug() << "zAnimation.currentValue().toDouble():"  << zAnimation.currentValue().toDouble();
    qDebug() << "xRotAnimation.currentValue().toDouble():"  << xRotAnimation.currentValue().toDouble();
    qDebug() << "yRotAnimation.currentValue().toDouble():"  << yRotAnimation.currentValue().toDouble();
    qDebug() << "zRotAnimation.currentValue().toDouble():"  << zRotAnimation.currentValue().toDouble();

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
        qDebug() << "writingLoopThread->prepareForExit();";
        writingLoopThread->prepareForExit();
        qDebug() << "writingLoopThread->allowNextSignal();";
        writingLoopThread->allowNextSignal();
        qDebug() << "writingLoopThread->wait();";
        writingLoopThread->wait();
        qDebug() << "writingLoopThread->terminate();";
        writingLoopThread->terminate();
        qDebug() << "currentStateOfAnimation->setValue(0);";
        currentStateOfAnimation->setValue(0);
        qDebug() << "emit writingToAviCompleted();";
        emit writingToAviCompleted();
        return;
    }

    emit currentWritingFrameChanged();
}

void AnimationManagementWidget::processSpecifyAviFilename()
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

    emit currentAviFilenameChanged(fileName);
}

void AnimationManagementWidget::processSpecifyPngFilename()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Specify image file to write"),
                                                    "",
                                                    tr("Portable network graphics(*.png)"));

    if(fileName.isEmpty())
    {
        return;
    }

    pathToSavePngFileLineEdit->setText(fileName);
    pngWrite->setEnabled(true);

    emit currentPngFilenameChanged(fileName);
}

void AnimationManagementWidget::processFirstPointXDoubleSpinBoxValueChanged(double value)
{
    qDebug() << "void AnimationManagementWidget::processFirstPointXDoubleSpinBoxValueChanged(double value)";
    updateValuesFromControls();
}

void AnimationManagementWidget::processFirstPointYDoubleSpinBoxValueChanged(double value)
{
    qDebug() << "void AnimationManagementWidget::processFirstPointYDoubleSpinBoxValueChanged(double value)";
    updateValuesFromControls();
}

void AnimationManagementWidget::processFirstPointZDoubleSpinBoxValueChanged(double value)
{
    qDebug() << "void AnimationManagementWidget::processFirstPointZDoubleSpinBoxValueChanged(double value)";
    updateValuesFromControls();
}

void AnimationManagementWidget::processFirstPointRotXDoubleSpinBoxValueChanged(double value)
{
    qDebug() << "void AnimationManagementWidget::processFirstPointRotXDoubleSpinBoxValueChanged(double value)";
    updateValuesFromControls();
}

void AnimationManagementWidget::processFirstPointRotYDoubleSpinBoxValueChanged(double value)
{
    qDebug() << "void AnimationManagementWidget::processFirstPointRotYDoubleSpinBoxValueChanged(double value)";
    updateValuesFromControls();
}

void AnimationManagementWidget::processFirstPointRotZDoubleSpinBoxValueChanged(double value)
{
    qDebug() << "void AnimationManagementWidget::processFirstPointRotZDoubleSpinBoxValueChanged(double value)";
    updateValuesFromControls();
}

void AnimationManagementWidget::processSecondPointXDoubleSpinBoxValueChanged(double value)
{
    qDebug() << "void AnimationManagementWidget::processSecondPointXDoubleSpinBoxValueChanged(double value)";
    updateValuesFromControls();
}

void AnimationManagementWidget::processSecondPointYDoubleSpinBoxValueChanged(double value)
{
    qDebug() << "void AnimationManagementWidget::processSecondPointYDoubleSpinBoxValueChanged(double value)";
    updateValuesFromControls();
}

void AnimationManagementWidget::processSecondPointZDoubleSpinBoxValueChanged(double value)
{
    qDebug() << "void AnimationManagementWidget::processSecondPointZDoubleSpinBoxValueChanged(double value)";
    updateValuesFromControls();
}

void AnimationManagementWidget::processSecondPointRotXDoubleSpinBoxValueChanged(double value)
{
    qDebug() << "void AnimationManagementWidget::processSecondPointRotXDoubleSpinBoxValueChanged(double value)";
    updateValuesFromControls();
}

void AnimationManagementWidget::processSecondPointRotYDoubleSpinBoxValueChanged(double value)
{
    qDebug() << "void AnimationManagementWidget::processSecondPointRotYDoubleSpinBoxValueChanged(double value)";
    updateValuesFromControls();
}

void AnimationManagementWidget::processSecondPointRotZDoubleSpinBoxValueChanged(double value)
{
    qDebug() << "void AnimationManagementWidget::processSecondPointRotZDoubleSpinBoxValueChanged(double value)";
    updateValuesFromControls();
}

void AnimationManagementWidget::processPngWrite()
{
    qDebug() << "void AnimationManagementWidget::processPngWrite()";
    emit pngWritingRequested();
}
