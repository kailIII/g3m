package org.glob3.mobile.generated; 
//
//  IG3MJSONBuilder.hpp
//  G3MiOSSDK
//
//  Created by Eduardo de la Montaña on 29/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//




public abstract class IG3MJSONBuilder
{

	protected String _jsonSource;


	public IG3MJSONBuilder(String jsonSource)
	{
		_jsonSource = jsonSource;
	}

	public abstract void initWidgetWithCameraConstraints (java.util.ArrayList<ICameraConstrainer> cameraConstraints, LayerSet layerSet, java.util.ArrayList<Renderer> renderers, UserData userData, GTask initializationTask, java.util.ArrayList<PeriodicalTask> periodicalTasks);
	public void dispose()
	{
	}

}