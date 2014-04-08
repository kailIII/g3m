//
//  GEOVectorialLODTileRasterizer.cpp
//  G3MiOSSDK
//
//  Created by fpulido on 17/03/14.
//
//

#include "GEOVectorialLODTileRasterizer.hpp"

#include "GEORasterSymbol.hpp"
#include "Tile.hpp"
#include "IImageListener.hpp"
#include "IImage.hpp"
#include "ICanvas.hpp"
#include "Color.hpp"
#include "GFont.hpp"
#include "IStringBuilder.hpp"
#include "GEORasterProjection.hpp"
#include "IBufferDownloadListener.hpp"
#include "GEORenderer.hpp"
#include "GEORasterSymbolizer.hpp"
#include "GEOJSONParser.hpp"
#include "IThreadUtils.hpp"




void GEOVectorialLODTileRasterizer::initialize(const G3MContext* context) {
    _downloader = context->getDownloader();
    //TODO: necesito conseguir el GEORenderer, para poder hacerle luego
    // el add addGEOObject().
}

void GEOVectorialLODTileRasterizer::clear() {
    //TODO
    notifyChanges();
}

//void GEOVectorialLODTileRasterizer::addSymbol(const GEORasterSymbol* symbol) {
//    
//    const Sector* sector = symbol->getSector();
//    
//    if (sector == NULL) {
//        //    ILogger::instance()->logError("Symbol %s has not sector, can't symbolize",
//        //                                  symbol->description().c_str());
//        delete symbol;
//    }
//    else {
//        //const bool added = _quadTree.add(*sector, symbol);
//        const bool added = true;
//        
//        if (added) {
//            notifyChanges();
//        }
//        else {
//            delete symbol;
//        }
//    }
//}


void GEOVectorialLODTileRasterizer::rawRasterize(const IImage* image,
                                     const TileRasterizerContext& trc,
                                     IImageListener* listener,
                                     bool autodelete) const {
    
//    _downloader->requestBuffer(<#const URL &url#>, <#long long priority#>, <#const TimeInterval &timeToCache#>, <#bool readExpired#>, <#IBufferDownloadListener *listener#>, true);
    
    
    
    //GEORenderer gr = trc->geoRenderer;
    
}

TileRasterizer_AsyncTask* GEOVectorialLODTileRasterizer::getRawRasterizeTask(const IImage* image,
                                                                       const TileRasterizerContext& trc,
                                                                       IImageListener* listener,
                                                                       bool autodelete) const {
    
    return NULL;
}


//---------------

class GEOVectorialLODParserAsyncTask : public GAsyncTask {
private:
#ifdef C_CODE
    const URL          _url;
#endif
#ifdef JAVA_CODE
    public final URL _url;
#endif
    
    IByteBuffer*   _buffer;
    GEORenderer*   _geoRenderer;
    GEORasterSymbolizer* _symbolizer;
    
    const bool _isBSON;
    
    GEOObject* _geoObject;
    
public:
    GEOVectorialLODParserAsyncTask(const URL& url,
                                   IByteBuffer* buffer,
                                   GEORenderer* geoRenderer,
                                   GEORasterSymbolizer* symbolizer,
                                   bool isBSON) :
    _url(url),
    _buffer(buffer),
    _geoRenderer(geoRenderer),
    _symbolizer(symbolizer),
    _isBSON(isBSON),
    _geoObject(NULL)
    {
    }
    
    ~GEOVectorialLODParserAsyncTask() {
        delete _buffer;
        //    delete _geoObject;
    }
    
    void runInBackground(const G3MContext* context) {
        //    ILogger::instance()->logInfo("Parsing GEOObject buffer from \"%s\" (%db)",
        //                                 _url.getPath().c_str(),
        //                                 _buffer->size());
        
        if (_isBSON) {
            _geoObject = GEOJSONParser::parseBSON(_buffer);
        }
        else {
            _geoObject = GEOJSONParser::parseJSON(_buffer);
        }
        
        delete _buffer;
        _buffer = NULL;
    }
    
    void onPostExecute(const G3MContext* context) {
        if (_geoObject == NULL) {
            ILogger::instance()->logError("Error parsing GEOJSON from \"%s\"", _url.getPath().c_str());
        }
        else {
            //      ILogger::instance()->logInfo("Adding GEOObject to _geoRenderer");
            _geoRenderer->addGEOObject(_geoObject, _symbolizer);
            _geoObject = NULL;
        }
    }
};



class GEOVectorialLODBufferDownloadListener : public IBufferDownloadListener {
private:
    GEORenderer*        _geoRenderer;
    GEORasterSymbolizer*      _symbolizer;
    const IThreadUtils* _threadUtils;
    const bool          _isBSON;
    
public:
    GEOVectorialLODBufferDownloadListener(GEORenderer* geoRenderer,
                                                GEORasterSymbolizer* symbolizer,
                                                const IThreadUtils* threadUtils,
                                                bool isBSON) :
    _geoRenderer(geoRenderer),
    _symbolizer(symbolizer),
    _threadUtils(threadUtils),
    _isBSON(isBSON)
    {
    }
    
    void onDownload(const URL& url,
                    IByteBuffer* buffer,
                    bool expired) {
        ILogger::instance()->logInfo("Downloaded GEOObject buffer from \"%s\" (%db)",
                                     url.getPath().c_str(),
                                     buffer->size());
        
        _threadUtils->invokeAsyncTask(new GEOVectorialLODParserAsyncTask(url,
                                                                               buffer,
                                                                               _geoRenderer,
                                                                               _symbolizer,
                                                                               _isBSON),
                                      true);
    }
    
    void onError(const URL& url) {
        ILogger::instance()->logError("Error downloading \"%s\"", url.getPath().c_str());
    }
    
    void onCancel(const URL& url) {
        ILogger::instance()->logInfo("Canceled download of \"%s\"", url.getPath().c_str());
    }
    
    void onCanceledDownload(const URL& url,
                            IByteBuffer* buffer,
                            bool expired) {
        // do nothing
    }
};


