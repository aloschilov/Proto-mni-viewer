#include <string>
#include <vtkPolygonalSurfacePointPlacer.h>
#include "jsoncpp/include/json/json.h"

class SaveablePolygonalSurfacePointPlacer : public vtkPolygonalSurfacePointPlacer
{
  public:
    static SaveablePolygonalSurfacePointPlacer *New();

    Json::Value getStateInJsonFormat();
    void setStateInJsonFormat(Json::Value state, vtkPolyData *pd);
};
