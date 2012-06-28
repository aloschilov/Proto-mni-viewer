#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt includes section
#include <QtGui>

// VTK includes section
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkMNIObjectReader.h>
#include <vtkScalarBarActor.h>
#include <vtkActor.h>
#include <vtkLight.h>
#include <vtkSphereWidget.h>
#include <vtkConeSource.h>
#include <vtkDataSetMapper.h>
#include <vtkVolumePicker.h>
#include <vtkPointPicker.h>
#include <vtkWindowToImageFilter.h>
#include <vtkFFMPEGWriter.h>
#include <vtkPNGWriter.h>

#include <vtkEventQtSlotConnect.h>
#include "saveablepolygonalsurfacepointplacer.h"
#include <vtkTransform.h>



// Forward declarations section
class QVTKWidget;
class LookupTableSelectionWidget;
class ShadingModelSelectionWidget;
class LightingPropertiesWidget;
class SurfaceSelectionWidget;
class AnimationManagementWidget;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:

    void showEvent ( QShowEvent * event );

private slots:

    void openMeshFile();
    void openPerPointScalarsFile();

    void openPerPointRgbFile();
    void savePerPointRgbFile();

    void updateLookupTable(vtkSmartPointer<vtkLookupTable> lookupTable);
    void updateDirectRgbColors(vtkSmartPointer<vtkUnsignedCharArray> colors);

    void setFlatShadingModel();
    void setGouraudShadingModel();
    void setPhongShadingModel();

    void processSphereWidgetInteractionEvent(vtkObject*, unsigned long, void*, void*, vtkCommand*);
    void processMouseMoveEvent(vtkObject*, unsigned long, void*, void*, vtkCommand*);
    void processLeftButtonPressEvent(vtkObject*, unsigned long, void*, void*, vtkCommand*);
    void processLeftButtonReleaseEvent(vtkObject*, unsigned long, void*, void*, vtkCommand*);
    void pointCone(vtkActor* actorToRotate, double nx, double ny, double nz);

    void processAmbientChanged(double value);
    void processSpecularChanged(double value);
    void processDiffuseChanged(double value);
    void processOpacityChanged(double value);
    void processLightingStateChanged(int state);
    void processLightingWidgetStateChanged(int state);

    void openSelection();
    void saveSelection();
    void clearSelection();

    void saveObjectStateAsFirstAnimationPoint();
    void saveObjectStateAsSecondAnimationPoint();

    void processCurrentTimeChanged();

    void processCurrentWritingFrameChanged();
    void processWritingToAviInitiated();
    void processWritingToAviCompleted();
    void processWritingAviFilenameChanged(QString filename);
    void processWritingPngFilenameChanged(QString filename);

    void processWritePng();

    void processAnimationModeStateEntered();
    void processAnimationModeStateExited();

    void processCameraModeStateEntered();
    void processCameraModeStateExited();

    void processPaintModeStateEntered();
    void processPaintModeStateExited();

    void processSelectColorForPencil();
    void processSelectBackgroundColor();

    void showLegend();
    void hideLegend();

    void processScalarRangeChanged(double min, double max);

private:
    void createActions();
    void createMenu();
    void createToolbar();
    void initializeVtk();
    void initializeStateMachine();

    void disableLighting();
    void enableLighting();

    void openMeshFileByName(const QString &filename);

    QIcon getIconFilledWithColor(QColor color);

    QAction *openMeshFileAction;
    QAction *openPerPointScalarsFileAction;
    
    QDockWidget *lookupTableSelectionDockWidget;
    LookupTableSelectionWidget *lookupTableSelectionWidget;

    QDockWidget *shadingModelSelectionDockWidget;
    ShadingModelSelectionWidget *shadingModelSelectionWidget;

    QDockWidget *lightingPropertiesDockWidget;
    LightingPropertiesWidget *lightingPropertiesWidget;

    QDockWidget *surfaceSelectionDockWidget;
    SurfaceSelectionWidget *surfaceSelectionWidget;

    QDockWidget *animationManagementDockWidget;
    AnimationManagementWidget *animationManagementWidget;

    QVTKWidget *qvtkWidget;

    vtkSmartPointer<vtkPolyDataMapper> mapper;
    vtkSmartPointer<vtkMNIObjectReader> reader;
    vtkSmartPointer<vtkRenderer> ren;
    vtkSmartPointer<vtkScalarBarActor > scalar_bar;
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkEventQtSlotConnect> Connections;
    vtkSmartPointer<vtkLight> light;
    vtkSmartPointer<vtkSphereWidget> sphereWidget;

    vtkFloatArray *scalars;
    float min;
    float max;

    vtkSmartPointer<SaveablePolygonalSurfacePointPlacer> pointPlacer;

    // Vtk objects related to marking surface

    vtkSmartPointer<vtkVolumePicker> volumePicker;
    vtkSmartPointer<vtkPointPicker> pointPicker;

    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter;
    vtkSmartPointer<vtkFFMPEGWriter> ffmpegWriter;

    vtkSmartPointer<vtkPNGWriter> pngWriter;

//    vtkSmartPointer<vtkConeSource> coneSource;
//    vtkSmartPointer<vtkDataSetMapper> coneMapper;
//    vtkSmartPointer<vtkActor> pencilActor;
//    vtkSmartPointer<vtkActor> greenCone;

    QStateMachine machine;
    QState animationModeState;
    QState cameraModeState;
    QState paintModeState;

    QAction *activateCameraModeAction;
    QAction *activateObjectAnimationModeAction;
    QAction *activatePaintModeAction;
    QToolButton *selectPencilColorToolButton;
    QColor pencilColor;

    QToolButton *selectBackgroundColorToolButton;

    bool leftMouseButtonIsPressed;
    bool isInPaintMode;

    vtkSmartPointer<vtkTransform> mniObjectTransfrom;
    vtkSmartPointer<vtkTransform> markedAreaTransfrom;
    vtkSmartPointer<vtkActor> markedAreaPinActor;

    QMap<vtkIdType, vtkSmartPointer<vtkActor> > placedPoints;
};

#endif // MAINWINDOW_H
