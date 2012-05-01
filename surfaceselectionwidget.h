#ifndef SURFACESELECTIONWIDGET_H
#define SURFACESELECTIONWIDGET_H

#include <QWidget>

class QCheckBox;
class QPushButton;

class SurfaceSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SurfaceSelectionWidget(QWidget *parent = 0);

    QCheckBox *enableSurfaceSelectorCheckbox;

    QPushButton *openContour;
    QPushButton *saveContour;
signals:

public slots:

};

#endif // SURFACESELECTIONWIDGET_H
