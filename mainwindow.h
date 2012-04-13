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

#include <vtkEventQtSlotConnect.h>

// Forward declarations section
class QVTKWidget;
class LookupTableSelectionWidget;
class ShadingModelSelectionWidget;
class LightingPropertiesWidget;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openMeshFile();
    void openPerPointScalarsFile();
    void updateLookupTable(vtkSmartPointer<vtkLookupTable> lookupTable);

    void setFlatShadingModel();
    void setGouraudShadingModel();
    void setPhongShadingModel();

    void processSphereWidgetInteractionEvent(vtkObject*, unsigned long, void*, void*, vtkCommand*);
    void processMouseMoveEvent(vtkObject*, unsigned long, void*, void*, vtkCommand*);

    void processAmbientChanged(double value);
    void processSpecularChanged(double value);
    void processDiffuseChanged(double value);
    void processOpacityChanged(double value);
    void processLightingStateChanged(int state);
    void processLightingWidgetStateChanged(int state);

private:
    void createActions();
    void createMenu();
    void initializeVtk();

    void disableLighting();
    void enableLighting();

    QAction *openMeshFileAction;
    QAction *openPerPointScalarsFileAction;
    
    QDockWidget *lookupTableSelectionDockWidget;
    LookupTableSelectionWidget *lookupTableSelectionWidget;

    QDockWidget *shadingModelSelectionDockWidget;
    ShadingModelSelectionWidget *shadingModelSelectionWidget;

    QDockWidget *lightingPropertiesDockWidget;
    LightingPropertiesWidget *lightingPropertiesWidget;

    QVTKWidget *qvtkWidget;

    vtkSmartPointer<vtkPolyDataMapper> mapper;
    vtkSmartPointer<vtkMNIObjectReader> reader;
    vtkSmartPointer<vtkRenderer> ren;
    vtkSmartPointer<vtkScalarBarActor > scalar_bar;
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkEventQtSlotConnect> Connections;
    vtkSmartPointer<vtkLight> light;
    vtkSmartPointer<vtkSphereWidget> sphereWidget;

    // Vtk objects related to marking surface

    vtkSmartPointer<vtkConeSource> coneSource;
    vtkSmartPointer<vtkDataSetMapper> coneMapper;
    vtkSmartPointer<vtkActor> redCone;
    vtkSmartPointer<vtkActor> greenCone;
    vtkSmartPointer<vtkVolumePicker> picker;
};

#endif // MAINWINDOW_H
