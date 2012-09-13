//
//  StarsRenderer.hpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 06/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_StarsRenderer_hpp
#define G3MiOSSDK_StarsRenderer_hpp

#include "Renderer.hpp"

class DirectMesh;

class StarsRenderer: public Renderer{
  
  int _nStars;
  double _starsHeight;
  
  DirectMesh* _mesh1;
  DirectMesh* _mesh2;
  DirectMesh* _mesh3;
  DirectMesh* _mesh4;
  
public:
  StarsRenderer(int nStars){
    _nStars = nStars;
  }
  
  ~StarsRenderer();
  
  void initialize(const InitializationContext* ic);
  
  bool isReadyToRender(const RenderContext* rc){
    return true;
  }
  
  void render(const RenderContext* rc);
  
  bool onTouchEvent(const EventContext* ec, const TouchEvent* touchEvent){ 
    return false;
  };
  
  void onResizeViewportEvent(const EventContext* ec, int width, int height){ };
  
  void start(){ };
  
  void stop(){ };
  
};

#endif
