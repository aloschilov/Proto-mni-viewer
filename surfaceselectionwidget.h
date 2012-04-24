#ifndef SURFACESELECTIONWIDGET_H
#define SURFACESELECTIONWIDGET_H

#include <QWidget>

class QCheckBox;

class SurfaceSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SurfaceSelectionWidget(QWidget *parent = 0);

    QCheckBox *enableSurfaceSelectorCheckbox;
signals:

public slots:

};

#endif // SURFACESELECTIONWIDGET_H
