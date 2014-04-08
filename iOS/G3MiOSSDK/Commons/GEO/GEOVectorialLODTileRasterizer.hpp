//
//  GEOVectorialLODTileRasterizer.h
//  G3MiOSSDK
//
//  Created by fpulido on 17/03/14.
//
//

#ifndef __G3MiOSSDK__GEOVectorialLODTileRasterizer__
#define __G3MiOSSDK__GEOVectorialLODTileRasterizer__

#include <iostream>
#include "CanvasTileRasterizer.hpp"
#include "IDownloader.hpp"

class GEORasterSymbol;


class GEOVectorialLODTileRasterizer : public CanvasTileRasterizer {
private:
    IDownloader *_downloader;
    
public:
    GEOVectorialLODTileRasterizer()
    {
    }
    
    std::string getId() const {
        return "GEOVectorialLODTileRasterizer";
    }
    
    void initialize(const G3MContext* context);
    
    void rawRasterize(const IImage* image,
                      const TileRasterizerContext& trc,
                      IImageListener* listener,
                      bool autodelete) const;
    
//    void addSymbol(const GEORasterSymbol* symbol);
    
    void clear();
    
    ICanvas* getCanvas(int width, int height) const {
        return CanvasTileRasterizer::getCanvas(width, height);
    }
    
    TileRasterizer_AsyncTask* getRawRasterizeTask(const IImage* image,
                                                          const TileRasterizerContext& trc,
                                                          IImageListener* listener,
                                                          bool autodelete) const;
};


#endif /* defined(__G3MiOSSDK__GEOVectorialLODTileRasterizer__) */