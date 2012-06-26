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

    QPushButton *openSelectedPoints;
    QPushButton *saveSelectedPoints;
    QPushButton *clearSelection;
signals:

public slots:

};

#endif // SURFACESELECTIONWIDGET_H
