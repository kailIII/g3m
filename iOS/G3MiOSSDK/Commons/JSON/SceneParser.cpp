//
//  SceneParser.cpp
//  G3MiOSSDK
//
//  Created by Eduardo de la Montaña on 15/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "SceneParser.hpp"

#include "IJSONParser.hpp"
#include "IStringBuilder.hpp"
#include "IStringUtils.hpp"

#include "JSONBaseObject.hpp"
#include "JSONNumber.hpp"
#include "JSONObject.hpp"
#include "JSONArray.hpp"
#include "JSONString.hpp"
#include "JSONBoolean.hpp"

#include "WMSLayer.hpp"
#include "LayerSet.hpp"

#include <stdio.h>
#include <string.h>

#include "MarksRenderer.hpp"
#include "GEOJSONDownloadListener.hpp"

using namespace std;

const std::string SceneParser::LAYERS = "layers";
const std::string SceneParser::TYPE = "type";
const std::string SceneParser::DATASOURCE = "datasource";
const std::string SceneParser::VERSION = "version";
const std::string SceneParser::ITEMS = "items";
const std::string SceneParser::STATUS = "status";
const std::string SceneParser::NAME = "name";
const std::string SceneParser::ICON = "icon";


const std::string SceneParser::WMS110 = "1.1.0";
const std::string SceneParser::WMS111 = "1.1.1";
const std::string SceneParser::WMS130 = "1.3.0";

SceneParser* SceneParser::_instance = NULL;

SceneParser* SceneParser::instance(){
  if (_instance == NULL){
    _instance = new SceneParser();
  }
  return _instance;
}

SceneParser::SceneParser(){
  _mapLayerType["WMS"] = WMS;
  _mapLayerType["THREED"] = THREED;
  _mapLayerType["PANO"] = PANO;
  _mapLayerType["GEOJSON"] = GEOJSON;  

}

void SceneParser::parse(LayerSet* layerSet, IDownloader* downloader, std::vector<Renderer*>* renderers, std::string namelessParameter){
  
  JSONObject* json = IJSONParser::instance()->parse(namelessParameter)->getObject();
  parserJSONLayerList(layerSet, downloader, renderers, json->getObjectForKey(LAYERS)->getObject());
  IJSONParser::instance()->deleteJSONData(json);
}

void SceneParser::parserJSONLayerList(LayerSet* layerSet, IDownloader* downloader, std::vector<Renderer*>* renderers, JSONObject* jsonLayers){
  for (int i = 0; i < jsonLayers->getObject()->getSize(); i++) {
    IStringBuilder* isb = IStringBuilder::newStringBuilder();
    isb->addInt(i);
    JSONObject* jsonLayer = jsonLayers->getObjectForKey(isb->getString())->getObject();
    const layer_type layerType = _mapLayerType[jsonLayer->getObjectForKey(TYPE)->getString()->getValue()];
    
    switch (layerType) {
      case WMS:
        parserJSONWMSLayer(layerSet, jsonLayer);
        break;
      case THREED:
        parserJSON3DLayer(layerSet, jsonLayer);
        break;
      case PANO:
        parserJSONPanoLayer(layerSet, jsonLayer);
        break;
      case GEOJSON:
        parserGEOJSONLayer(layerSet, downloader, renderers, jsonLayer);
        break;
    }
    
  }
}

void SceneParser::parserJSONWMSLayer(LayerSet* layerSet, JSONObject* jsonLayer){
  cout << "Parsing WMS Layer " << jsonLayer->getObjectForKey(NAME)->getString()->getValue() << "..." << endl;
  
  const std::string jsonDatasource = jsonLayer->getObjectForKey(DATASOURCE)->getString()->getValue();
  const int lastIndex = IStringUtils::instance()->indexOf(jsonDatasource,"?");
  const std::string jsonURL = IStringUtils::instance()->substring(jsonDatasource, 0, lastIndex+1);
  const std::string jsonVersion = jsonLayer->getObjectForKey(VERSION)->getString()->getValue();
  
  JSONArray* jsonItems = jsonLayer->getObjectForKey(ITEMS)->getArray();
  IStringBuilder *layersName = IStringBuilder::newStringBuilder();
    
  for (int i = 0; i<jsonItems->getSize(); i++) {
      if (jsonItems->getElement(i)->getObject()->getObjectForKey(STATUS)->getBoolean()->getValue()) {
      layersName->addString(jsonItems->getElement(i)->getObject()->getObjectForKey(NAME)->getString()->getValue());
      layersName->addString(",");
    }    
  }
  std::string layersSecuence = layersName->getString();
  if (layersName->getString().length() > 0) {
    layersSecuence = IStringUtils::instance()->substring(layersSecuence, 0, layersSecuence.length()-1);
  }
  
  //TODO check if wms 1.1.1 is neccessary to have it in account
  WMSServerVersion wmsVersion = WMS_1_1_0;
  if (jsonVersion.compare(WMS130)==0) {
    wmsVersion = WMS_1_3_0;
  }  
  
  WMSLayer* wmsLayer = new WMSLayer(URL::escape(layersSecuence),
                                      URL(jsonURL, false),
                                      wmsVersion,
                                      Sector::fullSphere(),
                                      "image/png",
                                      "EPSG:4326",
                                      "",
                                      true,
                                      NULL);
  layerSet->addLayer(wmsLayer);
}

void SceneParser::parserJSON3DLayer(LayerSet* layerSet, JSONObject* jsonLayer){
  cout << "Parsing 3D Layer " << jsonLayer->getObjectForKey(NAME)->getString()->getValue() << "..." << endl;
}

void SceneParser::parserJSONPanoLayer(LayerSet* layerSet, JSONObject* jsonLayer){
  cout << "Parsing Pano Layer " << jsonLayer->getObjectForKey(NAME)->getString()->getValue() << "..." << endl;
}

void SceneParser::parserGEOJSONLayer(LayerSet* layerSet, IDownloader* downloader, std::vector<Renderer*>* renderers, JSONObject* jsonLayer){
    cout << "Parsing GEOJSON Layer " << jsonLayer->getObjectForKey(NAME)->getString()->getValue() << "..." << endl;
    
    const bool readyWhenMarksReady = false;
    MarksRenderer* marksRenderer = new MarksRenderer(readyWhenMarksReady);
    renderers->push_back(marksRenderer);
    
    const std::string geojsonDatasource = jsonLayer->getObjectForKey(DATASOURCE)->getString()->getValue();
    
    JSONArray* jsonItems = jsonLayer->getObjectForKey(ITEMS)->getArray();
    for (int i = 0; i<jsonItems->getSize(); i++) {
        
        const std::string namefile = jsonItems->getElement(i)->getObject()->getObjectForKey(NAME)->getString()->getValue();
        const std::string icon = jsonItems->getElement(i)->getObject()->getObjectForKey(ICON)->getString()->getValue();
        
        IStringBuilder *url = IStringBuilder::newStringBuilder();
        url->addString(geojsonDatasource);
        url->addString("/");
        url->addString(namefile);
        
        cout << "Downloading " << namefile << " file" << endl;
        
        downloader->requestBuffer(URL(url->getString(), false), 100000000L, new GEOJSONDownloadListener(marksRenderer, icon), true);

    }
    
}

