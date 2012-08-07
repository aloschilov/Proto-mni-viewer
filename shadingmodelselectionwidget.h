#ifndef SHADINGMODELSELECTIONWIDGET_H
#define SHADINGMODELSELECTIONWIDGET_H

#include <QWidget>
#include <QRadioButton>


class ShadingModelSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShadingModelSelectionWidget(QWidget *parent = 0);

signals:
    void shadingModelChangedToFlat();
    void shadingModelChangedToGouraud();

public slots:

private slots:
    void processModelChanged();

private:
    QRadioButton *flatShadingModelRadioButton;
    QRadioButton *gouraundShadingModelRadioButton;
};

#endif // SHADINGMODELSELECTIONWIDGET_H
