#ifndef GRADIENTS_H
#define GRADIENTS_H

#include <QtGui>

class HoverPoints;


class ShadeWidget : public QWidget
{
    Q_OBJECT
public:
    enum ShadeType {
        RedShade,
        GreenShade,
        BlueShade,
        ARGBShade
    };

    ShadeWidget(ShadeType type, QWidget *parent);

    void setGradientStops(const QGradientStops &stops);

    void paintEvent(QPaintEvent *e);

    QSize sizeHint() const { return QSize(150, 40); }
    QPolygonF points() const;

    HoverPoints *hoverPoints() const { return m_hoverPoints; }

    uint colorAt(int x);

signals:
    void colorsChanged();

private:
    void generateShade();

    ShadeType m_shade_type;
    QImage m_shade;
    HoverPoints *m_hoverPoints;
    QLinearGradient m_alpha_gradient;
};

class GradientEditor : public QWidget
{
    Q_OBJECT
public:
    GradientEditor(QWidget *parent);

    void setGradientStops(const QGradientStops &stops);

public slots:
    void pointsUpdated();

signals:
    void gradientStopsChanged(const QGradientStops &stops);

private:
    ShadeWidget *m_red_shade;
    ShadeWidget *m_green_shade;
    ShadeWidget *m_blue_shade;
    ShadeWidget *m_alpha_shade;
};


class GradientRenderer : public QWidget
{
    Q_OBJECT
public:
    GradientRenderer(QWidget *parent);
    void paint(QPainter *p);

    QImage paintToPixmap();

    QSize sizeHint() const { return QSize(400, 400); }

public slots:
    void setGradientStops(const QGradientStops &stops);

protected:
    void paintEvent(QPaintEvent *);

private:
    QGradientStops m_stops;
    QGradient::Spread m_spread;
    Qt::BrushStyle m_gradientType;
};


class GradientDialog : public QDialog
{
    Q_OBJECT
public:
    GradientDialog(QWidget *parent);

    QImage getGradientImage();

public slots:
    void setDefault1() { setDefault(1); }
    void setDefault2() { setDefault(2); }
    void setDefault3() { setDefault(3); }
    void setDefault4() { setDefault(4); }

private:
    void setDefault(int i);

    GradientRenderer *m_renderer;
    GradientEditor *m_editor;
};

#endif // GRADIENTS_H
