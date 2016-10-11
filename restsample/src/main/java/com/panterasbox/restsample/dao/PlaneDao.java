package com.panterasbox.restsample.dao;

import com.panterasbox.restsample.model.Plane;

public interface PlaneDao {

	Plane getById(String id);

	Plane create(Plane plane);
	
	Plane update(String id, Plane plane);

}
