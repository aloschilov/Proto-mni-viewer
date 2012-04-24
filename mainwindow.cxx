#include <QtGui>

#include <math.h>

#include "mainwindow.h"

#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkQtTableView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkVectorText.h>
#include <vtkMNIObjectReader.h>
#include <QVTKWidget.h>
#include <vtkScalarBarActor.h>
#include <vtkLookupTable.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkCamera.h>

#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkPolyDataCollection.h>

// vtkWidgets

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>


#include "lookuptableselectionwidget.h"
#include "shadingmodelselectionwidget.h"
#include "lightingpropertieswidget.h"
#include "surfaceselectionwidget.h"


#define VTK_CREATE(type, name) \
    vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

using namespace std;

double string_to_double( const std::string& s ) {
    istringstream i(s);
    double x;
    if (!(i >> x))
        return 0;

    return x;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("MNI object viewer"));

    qvtkWidget = new QVTKWidget();

    lookupTableSelectionWidget = new LookupTableSelectionWidget();

    lookupTableSelectionDockWidget = new QDockWidget(tr("Lookup table properties"), this);
    lookupTableSelectionDockWidget->setWidget(lookupTableSelectionWidget);
    lookupTableSelectionDockWidget->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    lookupTableSelectionDockWidget->setVisible(true);
    addDockWidget(Qt::RightDockWidgetArea, lookupTableSelectionDockWidget, Qt::Horizontal);

    shadingModelSelectionWidget = new ShadingModelSelectionWidget();

    shadingModelSelectionDockWidget = new QDockWidget(tr("Shading model properties"), this);
    shadingModelSelectionDockWidget->setWidget(shadingModelSelectionWidget);
    shadingModelSelectionDockWidget->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    shadingModelSelectionDockWidget->setVisible(true);
    addDockWidget(Qt::RightDockWidgetArea, shadingModelSelectionDockWidget, Qt::Horizontal);

    lightingPropertiesWidget = new LightingPropertiesWidget();

    lightingPropertiesDockWidget = new QDockWidget(tr("Lighting properties"), this);
    lightingPropertiesDockWidget->setWidget(lightingPropertiesWidget);
    lightingPropertiesDockWidget->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    lightingPropertiesDockWidget->setVisible(true);
    addDockWidget(Qt::RightDockWidgetArea, lightingPropertiesDockWidget, Qt::Horizontal);

    surfaceSelectionWidget = new SurfaceSelectionWidget();

    surfaceSelectionDockWidget = new QDockWidget(tr("Surface selection"), this);
    surfaceSelectionDockWidget->setWidget(surfaceSelectionWidget);
    surfaceSelectionDockWidget->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, surfaceSelectionDockWidget, Qt::Horizontal);

    setCentralWidget(qvtkWidget);

    createActions();
    createMenu();
    initializeVtk();

    connect(lookupTableSelectionWidget, SIGNAL(currentLookupTableChanged(vtkSmartPointer<vtkLookupTable>)),
            this, SLOT(updateLookupTable(vtkSmartPointer<vtkLookupTable>)));

    connect(shadingModelSelectionWidget, SIGNAL(shadingModelChangedToFlat()),
            this, SLOT(setFlatShadingModel()));
    connect(shadingModelSelectionWidget, SIGNAL(shadingModelChangedToGouraud()),
            this, SLOT(setGouraudShadingModel()));
    connect(shadingModelSelectionWidget, SIGNAL(shadingModelChangedToPhong()),
            this, SLOT(setPhongShadingModel()));
    connect(lightingPropertiesWidget->enableLightingCheckbox, SIGNAL(stateChanged(int)),
            this, SLOT(processLightingStateChanged(int)));
    connect(lightingPropertiesWidget->ambientDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processAmbientChanged(double)));
    connect(lightingPropertiesWidget->diffuseDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processDiffuseChanged(double)));
    connect(lightingPropertiesWidget->specularDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processSpecularChanged(double)));
    connect(lightingPropertiesWidget->opacityDoubleSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(processOpacityChanged(double)));
    connect(lightingPropertiesWidget->lightingWidgetVisibilityCheckbox, SIGNAL(stateChanged(int)),
            this, SLOT(processLightingWidgetStateChanged(int)));
    connect(surfaceSelectionWidget->enableSurfaceSelectorCheckbox, SIGNAL(stateChanged(int)),
            this, SLOT(processSurfaceSelectorStateChanged(int)));
}

void MainWindow::initializeVtk()
{
    reader = vtkSmartPointer<vtkMNIObjectReader>::New();

    mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->ImmediateModeRenderingOn();
    mapper->SetColorModeToMapScalars();
    mapper->SetLookupTable(lookupTableSelectionWidget->getCurrentLookupTable());

    actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    scalar_bar = vtkSmartPointer<vtkScalarBarActor >::New();
    scalar_bar->SetLookupTable(lookupTableSelectionWidget->getCurrentLookupTable());

    ren = vtkSmartPointer<vtkRenderer>::New();

    ren->AddActor(actor);
    ren->AddActor(scalar_bar);

    qvtkWidget->GetRenderWindow()->AddRenderer(ren);
    qvtkWidget->GetInteractor()->LightFollowCameraOff();

    sphereWidget = vtkSphereWidget::New();
    sphereWidget->SetInteractor(qvtkWidget->GetInteractor());
    sphereWidget->SetPlaceFactor(4);
    sphereWidget->TranslationOff();
    sphereWidget->ScaleOff();
    sphereWidget->KeyPressActivationOff();
    sphereWidget->HandleVisibilityOn();

    light = ren->MakeLight();
    ren->RemoveAllLights();

//    coneSource = vtkSmartPointer<vtkConeSource>::New();
//    coneSource->CappingOn();
//    coneSource->SetHeight(12);
//    coneSource->SetRadius(5);
//    coneSource->SetResolution(31);
//    coneSource->SetCenter(6,0,0);
//    coneSource->SetDirection(-1,0,0);

//    coneMapper = vtkSmartPointer<vtkDataSetMapper>::New();
//    coneMapper->SetInputConnection(coneSource->GetOutputPort());

//    redCone = vtkSmartPointer<vtkActor>::New();
//    redCone->PickableOff();
//    redCone->SetMapper(coneMapper);
//    redCone->GetProperty()->SetColor(1,0,0);

//    greenCone = vtkSmartPointer<vtkActor>::New();
//    greenCone->PickableOff();
//    greenCone->SetMapper(coneMapper);
//    greenCone->GetProperty()->SetColor(0,1,0);
    
//    ren->AddViewProp(redCone);
//    ren->AddViewProp(greenCone);

//    picker = vtkSmartPointer<vtkVolumePicker>::New();
//    picker->SetTolerance(1e-6);

//    redCone->SetVisibility(false);
//    greenCone->SetVisibility(false);
    contourWidget =
      vtkSmartPointer<vtkContourWidget>::New();
    vtkOrientedGlyphContourRepresentation *rep =
        vtkOrientedGlyphContourRepresentation::SafeDownCast(
                          contourWidget->GetRepresentation());
    rep->GetLinesProperty()->SetColor(1, 0.2, 0);
    rep->GetLinesProperty()->SetLineWidth(3.0);
    contourWidget->SetInteractor(qvtkWidget->GetInteractor());
    contourWidget->KeyPressActivationOff();

    pointPlacer =
      vtkSmartPointer<vtkPolygonalSurfacePointPlacer>::New();
    pointPlacer->AddProp(actor);
    rep->SetPointPlacer(pointPlacer);

    // Set a terrain interpolator. Interpolates points as they are placed,
    // so that they lie on the terrain.

    interpolator = vtkSmartPointer<vtkPolygonalSurfaceContourLineInterpolator>::New();
    rep->SetLineInterpolator(interpolator);

    Connections = vtkEventQtSlotConnect::New();
    Connections->Connect(sphereWidget,
                         vtkCommand::InteractionEvent,
                         this,
                         SLOT(processSphereWidgetInteractionEvent(vtkObject*, unsigned long, void*, void*, vtkCommand*)) );
//    Connections->Connect(qvtkWidget->GetInteractor(),
//                         vtkCommand::MouseMoveEvent,
//                         this,
//                         SLOT(processMouseMoveEvent(vtkObject*, unsigned long, void*, void*, vtkCommand*)) );
//    Connections->Connect(qvtkWidget->GetInteractor(),
//                         vtkCommand::LeftButtonPressEvent,
//                         this,
//                         SLOT());
}

MainWindow::~MainWindow()
{

}

void MainWindow::openMeshFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select MNI object file."),
                                                    "",
                                                    tr("MNI object files(*.obj)"));
    if(fileName.isEmpty())
    {
        return;
    }

    if(reader->CanReadFile(fileName.toStdString().c_str()))
    {
        reader->SetFileName(fileName.toStdString().c_str());
        reader->Update();
        mapper->SetInputConnection(reader->GetOutputPort());
        setWindowTitle(tr("MNI object viewer - %1").arg(fileName));
        statusBar()->showMessage(tr("MNI object file was successfully opened."));
        ren->ResetCamera();
        sphereWidget->SetProp3D(actor);
        sphereWidget->PlaceWidget();
        light->SetFocalPoint(reader->GetOutput()->GetCenter());


/*

        vtkSmartPointer<vtkPolyDataNormals> normals =
          vtkSmartPointer<vtkPolyDataNormals>::New();

        bool   distanceOffsetSpecified = false;
        double distanceOffset = 20.0;

        for (int i = 0; i < argc-1; i++)
        {
            distanceOffset = atof(argv[i+1]);
            distanceOffsetSpecified = true;
        }

        if (distanceOffsetSpecified)
        {
            normals->SetInputConnection(warp->GetOutputPort());
            normals->SetFeatureAngle(60);
            normals->SplittingOff();

            normals->ComputeCellNormalsOn();
            normals->ComputePointNormalsOn();
            normals->Update();
        }

        vtkPolyData *pd = normals->GetOutput();

        pointPlacer->GetPolys()->AddItem( pd );
        interpolator->GetPolys()->AddItem( pd );

        //interpolator->SetImageData();

//        redCone->SetVisibility(true);
//        greenCone->SetVisibility(true);*/
    }
    else
    {
        statusBar()->showMessage(tr("The file seems not to contain MNI object."));
    }
}

void MainWindow::openPerPointScalarsFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select file with per point scalars."),
                                                    "",
                                                    tr("Scalars file (*.txt *.map)"));
    if(fileName.isEmpty())
    {
        return;
    }

    VTK_CREATE(vtkFloatArray, scalars);
    ifstream in(fileName.toStdString().c_str(), ios::in);

    string current_token;
    int i =0;
    double max;
    double min;

    while(!in.eof()) {
        in >> current_token;
        scalars->InsertTuple1(i, string_to_double(current_token));
        if(i==0)
        {
            max = min = string_to_double(current_token);
        }
        else
        {
            max = (string_to_double(current_token)>max) ? string_to_double(current_token) : max;
            min = (string_to_double(current_token)<min) ? string_to_double(current_token) : min;
        }
        ++i;
    }

    reader->GetOutput()->GetPointData()->SetScalars(scalars);
    mapper->SetScalarRange(min, max);
    reader->GetOutput()->Print(cout);
}

void MainWindow::updateLookupTable(vtkSmartPointer<vtkLookupTable> lookupTable)
{
    mapper->SetLookupTable(lookupTable);
    scalar_bar->SetLookupTable(lookupTable);
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::createActions()
{
    // Open file action
    openMeshFileAction = new QAction(QIcon(":/images/open.png"), QString("&Open"), this);
    openMeshFileAction->setShortcut(QKeySequence::Open);
    connect(openMeshFileAction, SIGNAL(triggered()), this, SLOT(openMeshFile()));

    //
    openPerPointScalarsFileAction = new QAction(QString("Open &scalars file"), this);
    //openPerPointScalarsFileAction->setShortcut();
    connect(openPerPointScalarsFileAction, SIGNAL(triggered()), this, SLOT(openPerPointScalarsFile()));
}

void MainWindow::createMenu()
{
    QMenuBar *menubar = menuBar();
    QMenu *file = menubar->addMenu("&File");
    file->addAction(openMeshFileAction);
    file->addAction(openPerPointScalarsFileAction);

    QMenu *view = menubar->addMenu("&View");
    view->addAction(lookupTableSelectionDockWidget->toggleViewAction());
}

void MainWindow::setFlatShadingModel()
{
    actor->GetProperty()->SetInterpolationToFlat();
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::setGouraudShadingModel()
{
    actor->GetProperty()->SetInterpolationToGouraud();
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::setPhongShadingModel()
{
    actor->GetProperty()->SetInterpolationToPhong();
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processSphereWidgetInteractionEvent(vtkObject *caller, unsigned long, void*, void*, vtkCommand*)
{
    vtkSphereWidget *widget = reinterpret_cast<vtkSphereWidget*>(caller);
    light->SetPosition(widget->GetHandlePosition());
}

//void MainWindow::processMouseMoveEvent(vtkObject *caller, unsigned long, void*, void*, vtkCommand*)
//{
//  vtkRenderWindowInteractor *iren = reinterpret_cast<vtkRenderWindowInteractor*>(caller);
  
//  int pos[2];
//  iren->GetEventPosition(pos);
//  picker->Pick(pos[0], pos[1], 0, ren);
//  double p[3];
//  double n[3];
//  picker->GetPickPosition(p);
//  picker->GetPickNormal(n);
//  redCone->SetPosition(p);
//  pointCone(redCone, n[0], n[1], n[2]);
//  greenCone->SetPosition(p);
//  pointCone(greenCone, -n[0], -n[1], -n[2]);
//  iren->Render();
//}


void MainWindow::pointCone(vtkActor* actorToRotate, double nx, double ny, double nz)
{
    actorToRotate->SetOrientation(0.0, 0.0, 0.0);
    double n = sqrt(nx*nx + ny*ny + nz*nz);
    if (nx < 0.0)
    {
        actorToRotate->RotateWXYZ(180, 0, 1, 0);
        n = -n;
    }
    actorToRotate->RotateWXYZ(180, (nx+n)*0.5, ny*0.5, nz*0.5);
}

void MainWindow::processLightingStateChanged(int state)
{
    if(state == Qt::Checked)
    {
        enableLighting();
    } else if (state == Qt::Unchecked)
    {
        disableLighting();
    }
}

void MainWindow::enableLighting()
{
    ren->AddLight(light);
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::disableLighting()
{
    ren->RemoveAllLights();
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processAmbientChanged(double value)
{
    actor->GetProperty()->SetAmbient(value);
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processSpecularChanged(double value)
{
    actor->GetProperty()->SetSpecular(value);
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processDiffuseChanged(double value)
{
    actor->GetProperty()->SetDiffuse(value);
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processOpacityChanged(double value)
{
    actor->GetProperty()->SetOpacity(value);
    qvtkWidget->GetRenderWindow()->Render();
}

void MainWindow::processLightingWidgetStateChanged(int state)
{
    if(state == Qt::Checked)
    {
        sphereWidget->On();
    } else if (state == Qt::Unchecked)
    {
        sphereWidget->Off();
    }
}

void MainWindow::processSurfaceSelectorStateChanged(int state)
{
    if(state == Qt::Checked)
    {
        contourWidget->On();
    } else if(state == Qt::Unchecked)
    {
        contourWidget->Off();
    }
}
