//
//  GEOTileRasterizer.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 7/10/13.
//
//

#ifndef __G3MiOSSDK__GEOTileRasterizer__
#define __G3MiOSSDK__GEOTileRasterizer__

#include "CanvasTileRasterizer.hpp"
#include "QuadTree.hpp"

class GEORasterSymbol;


class GEOTileRasterizer : public CanvasTileRasterizer {
private:
  QuadTree          _quadTree;
#ifdef C_CODE
  const G3MContext* _context;
#endif
#ifdef JAVA_CODE
  private G3MContext _context;
#endif

public:
  GEOTileRasterizer() :
  _context(NULL)
  {
  }

  std::string getId() const {
    return "GEOTileRasterizer";
  }

  void initialize(const G3MContext* context);

  void rawRasterize(const IImage* image,
                    const TileRasterizerContext& trc,
                    IImageListener* listener,
                    bool autodelete) const;

  void addSymbol(const GEORasterSymbol* symbol);
  
};

#endif