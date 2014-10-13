//
//  CameraMouseWheelHandler.cpp
//  G3MiOSSDK
//
//  Created by Jose Miguel SN on 10/10/14.
//
//

#include "CameraMouseWheelHandler.hpp"
#include "Camera.hpp"
#include "TouchEvent.hpp"

bool CameraMouseWheelHandler::onTouchEvent(const G3MEventContext *eventContext,
                  const TouchEvent* touchEvent,
                  CameraContext *cameraContext){
  
  if (touchEvent->getType() == MouseWheelChanged){
    onMouseWheel(eventContext, *touchEvent, cameraContext);
    return true;
  }
  return false;

}


void CameraMouseWheelHandler::onMouseWheel(const G3MEventContext *eventContext,
                                           const TouchEvent& touchEvent,
                                           CameraContext *cameraContext){
  Camera* cam = cameraContext->getNextCamera();
  const Planet* planet = eventContext->getPlanet();
  
  const Vector3D dir = cam->pixel2Ray(touchEvent.getTouch(0)->getPos()).normalized();
  
#warning USE ZRENDER IN THE FUTURE
  std::vector<double> dists = planet->intersectionsDistances(cam->getCartesianPosition(), dir);
  
  if (dists.size() > 0){     //Research other behaviours as Google Earth

    const double delta = touchEvent.getMouseWheelDelta();
    double factor = 0.1;
    if (delta < 0){
      factor *= -1;
    }
    
    double dist = dists.at(0);
    Vector3D translation = dir.normalized().times(dist * factor);
    
    cam->translateCamera(translation);
  }
  
  
}