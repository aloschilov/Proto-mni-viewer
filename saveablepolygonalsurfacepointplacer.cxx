#ifndef __SaveablePolygonalSurfacePointPlacer_h
#define __SaveablePolygonalSurfacePointPlacer_h

#include "saveablepolygonalsurfacepointplacer.h"

#include <vtkObjectFactory.h>
#include <vtkPolyDataWriter.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkProp.h>
#include <vtkPropCollection.h>
#include <vtkPolyDataCollection.h>
#include <vtkCellPicker.h>
#include <vtkAssemblyPath.h>
#include <vtkAssemblyNode.h>
#include <vtkInteractorObserver.h>
#include <vtkMapper.h>
#include <vtkPolyData.h>
#include <vtkMath.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkIdList.h>
#include <vtkPolyDataReader.h>

#include <vector>
#include <string>

vtkStandardNewMacro(SaveablePolygonalSurfacePointPlacer);

class vtkPolygonalSurfacePointPlacerInternals
{

public:
    typedef std::vector<
    vtkPolygonalSurfacePointPlacerNode * > NodesContainerType;

    NodesContainerType Nodes;

    ~vtkPolygonalSurfacePointPlacerInternals()
    {
        for (unsigned int i = 0; i < this->Nodes.size(); i++)
        {
            delete this->Nodes[i];
        }
        this->Nodes.clear();
    }

    vtkPolygonalSurfacePointPlacerNode
    *GetNodeAtSurfaceWorldPosition( double worldPos[3] )
    {
        const double tolerance = 0.0005;
        for (unsigned int i = 0; i < this->Nodes.size(); i++)
        {
            if (vtkMath::Distance2BetweenPoints(
                        this->Nodes[i]->SurfaceWorldPosition, worldPos ) < tolerance)
            {
                return this->Nodes[i];
            }
        }
        return NULL;
    }

    vtkPolygonalSurfacePointPlacerNode
    *GetNodeAtWorldPosition( double worldPos[3] )
    {
        const double tolerance = 0.0005;
        for (unsigned int i = 0; i < this->Nodes.size(); i++)
        {
            if (vtkMath::Distance2BetweenPoints(
                        this->Nodes[i]->WorldPosition, worldPos ) < tolerance)
            {
                return this->Nodes[i];
            }
        }
        return NULL;
    }

    vtkPolygonalSurfacePointPlacerNode
    *InsertNodeAtCurrentPickPosition( vtkCellPicker *picker,
                                      const double distanceOffset,
                                      int snapToClosestPoint )
    {
        double worldPos[3];
        picker->GetPickPosition(worldPos);

        // Get a node at this position if one exists and overwrite it
        // with the current pick position. If one doesn't exist, add
        // a new node.
        vtkPolygonalSurfacePointPlacerNode
                * node = this->GetNodeAtSurfaceWorldPosition(worldPos);
        if (!node)
        {
            node = new vtkPolygonalSurfacePointPlacerNode;
            this->Nodes.push_back(node);
        }

        vtkMapper *mapper =
                vtkMapper::SafeDownCast(picker->GetMapper());
        if (!mapper)
        {
            return NULL;
        }

        // Get the underlying dataset
        vtkPolyData *pd = vtkPolyData::SafeDownCast(mapper->GetInput());
        if (!pd)
        {
            return NULL;
        }

        node->CellId = picker->GetCellId();
        picker->GetPCoords(node->ParametricCoords);

        // translate to the closest point on that cell, if requested

        if (snapToClosestPoint)
        {
            vtkIdList *ids = vtkIdList::New();
            pd->GetCellPoints( picker->GetCellId(), ids );
            double p[3], minDistance = VTK_DOUBLE_MAX;
            for (vtkIdType i = 0; i < ids->GetNumberOfIds(); ++i)
            {
                pd->GetPoints()->GetPoint(ids->GetId(i), p);
                const double dist2 = vtkMath::Distance2BetweenPoints(
                            worldPos, pd->GetPoints()->GetPoint(ids->GetId(i)));
                if (dist2 < minDistance)
                {
                    minDistance = dist2;
                    worldPos[0] = p[0];
                    worldPos[1] = p[1];
                    worldPos[2] = p[2];
                }
            }
            ids->Delete();
        }

        node->SurfaceWorldPosition[0] = worldPos[0];
        node->SurfaceWorldPosition[1] = worldPos[1];
        node->SurfaceWorldPosition[2] = worldPos[2];
        node->PolyData = pd;
        double cellNormal[3];

        if (distanceOffset != 0.0)
        {
            pd->GetCellData()->GetNormals()->GetTuple( node->CellId, cellNormal );

            // Polyline can be drawn on polydata at a height offset.
            for (unsigned int i =0; i < 3; i++)
            {
                node->WorldPosition[i] =
                        node->SurfaceWorldPosition[i] + cellNormal[i] * distanceOffset;
            }
        }
        else
        {
            for (unsigned int i =0; i < 3; i++)
            {
                node->WorldPosition[i] = node->SurfaceWorldPosition[i];
            }
        }
        return node;
    }


    vtkPolygonalSurfacePointPlacerNode
    *InsertNodeAtCurrentPickPosition( vtkPolyData *pd,
                                      double worldPos[3],
                                      vtkIdType cellId,
                                      vtkIdType pointId,
                                      const double vtkNotUsed(distanceOffset),
                                      int vtkNotUsed(snapToClosestPoint) )
    {

        // Get a node at this position if one exists and overwrite it
        // with the current pick position. If one doesn't exist, add
        // a new node.
        vtkPolygonalSurfacePointPlacerNode
                * node = this->GetNodeAtSurfaceWorldPosition(worldPos);
        if (!node)
        {
            node = new vtkPolygonalSurfacePointPlacerNode;
            this->Nodes.push_back(node);
        }

        node->CellId = cellId;
        node->PointId = pointId;

        node->SurfaceWorldPosition[0] = worldPos[0];
        node->SurfaceWorldPosition[1] = worldPos[1];
        node->SurfaceWorldPosition[2] = worldPos[2];
        node->PolyData = pd;

        for (unsigned int i =0; i < 3; i++)
        {
            node->WorldPosition[i] = node->SurfaceWorldPosition[i];
        }
        return node;
    }
    //ashish
};


Json::Value SaveablePolygonalSurfacePointPlacer::getStateInJsonFormat()
{
    Json::StyledWriter writer;

    Json::Value root;

    for(unsigned int i = 0; i < this->Internals->Nodes.size(); ++i)
    {
        Json::Value value;
        Json::Value WorldPosition;

        WorldPosition.append(Json::Value(this->Internals->Nodes[i]->WorldPosition[0]));
        WorldPosition.append(Json::Value(this->Internals->Nodes[i]->WorldPosition[1]));
        WorldPosition.append(Json::Value(this->Internals->Nodes[i]->WorldPosition[2]));
        value["WorldPosition"] = WorldPosition;

        Json::Value SurfaceWorldPosition;
        SurfaceWorldPosition.append(Json::Value(this->Internals->Nodes[i]->SurfaceWorldPosition[0]));
        SurfaceWorldPosition.append(Json::Value(this->Internals->Nodes[i]->SurfaceWorldPosition[1]));
        SurfaceWorldPosition.append(Json::Value(this->Internals->Nodes[i]->SurfaceWorldPosition[2]));
        value["SurfaceWorldPosition"] = SurfaceWorldPosition;

        value["CellId"] = this->Internals->Nodes[i]->CellId;
        value["PointId"] = this->Internals->Nodes[i]->PointId;

        Json::Value ParametricCoords;

        ParametricCoords.append(Json::Value(this->Internals->Nodes[i]->ParametricCoords[0]));
        ParametricCoords.append(Json::Value(this->Internals->Nodes[i]->ParametricCoords[1]));
        ParametricCoords.append(Json::Value(this->Internals->Nodes[i]->ParametricCoords[2]));

        value["ParametricCoords"] = ParametricCoords;

        root.append(value);
    }

    return root;
}

void SaveablePolygonalSurfacePointPlacer::setStateInJsonFormat(Json::Value state, vtkPolyData *pd)
{
    for(int i = 0; i < state.size(); ++i)
    {
        vtkPolygonalSurfacePointPlacerNode *node = new vtkPolygonalSurfacePointPlacerNode;
        this->Internals->Nodes.push_back(node);

        node->WorldPosition[0] = state[i]["WorldPosition"][0].asDouble();
        node->WorldPosition[1] = state[i]["WorldPosition"][1].asDouble();
        node->WorldPosition[2] = state[i]["WorldPosition"][2].asDouble();

        node->SurfaceWorldPosition[0] = state[i]["SurfaceWorldPosition"][0].asDouble();
        node->SurfaceWorldPosition[1] = state[i]["SurfaceWorldPosition"][1].asDouble();
        node->SurfaceWorldPosition[2] = state[i]["SurfaceWorldPosition"][2].asDouble();

        node->CellId = state[i]["CellId"].asLargestInt();
        node->CellId = state[i]["PointId"].asLargestInt();

        node->ParametricCoords[0] = state[i]["ParametricCoords"][0].asDouble();
        node->ParametricCoords[1] = state[i]["ParametricCoords"][1].asDouble();
        node->ParametricCoords[2] = state[i]["ParametricCoords"][2].asDouble();

        node->PolyData = pd;
    }
    this->Modified();
}

#endif // __SaveablePolygonalSurfacePointPlacer_h
