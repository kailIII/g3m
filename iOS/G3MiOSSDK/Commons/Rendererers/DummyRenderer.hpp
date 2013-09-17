//
//  DummyRenderer.hpp
//  Glob3 Mobile
//
//  Created by Agustin Trujillo Pino on 02/05/11.
//  Copyright 2011 Universidad de Las Palmas. All rights reserved.
//

#ifndef DUMMYRENDERER
#define DUMMYRENDERER

#include "LeafRenderer.hpp"

class IFloatBuffer;
class IShortBuffer;

class DummyRenderer: public LeafRenderer {

private:
  double        _halfSize;

  IShortBuffer* _indices;
  IFloatBuffer* _vertices;

public:
  ~DummyRenderer();
  
  void initialize(const G3MContext* context);  
  
  void render(const G3MRenderContext* rc,
              const GLState& parentState);
  
  bool onTouchEvent(const G3MEventContext* ec,
                    const TouchEvent* touchEvent);
  
  void onResizeViewportEvent(const G3MEventContext* ec,
                             int width, int height) {

  }
  
  bool isReadyToRender(const G3MRenderContext* rc) {
    return true;
  }

  void start(const G3MRenderContext* rc) {
    
  }
  
  void stop(const G3MRenderContext* rc) {
    
  }

  void onResume(const G3MContext* context) {
    
  }
  
  void onPause(const G3MContext* context) {
    
  }

  void onDestroy(const G3MContext* context) {

  }

};

#endif