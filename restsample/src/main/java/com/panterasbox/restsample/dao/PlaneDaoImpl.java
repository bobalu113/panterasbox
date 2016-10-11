package com.panterasbox.restsample.dao;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

import org.springframework.stereotype.Repository;

import com.panterasbox.restsample.model.Plane;

@Repository
public class PlaneDaoImpl implements PlaneDao {

	private ConcurrentMap<String, Plane> planes;
	
	public PlaneDaoImpl() {
		planes = new ConcurrentHashMap<String, Plane>();
	}
	
	@Override
	public Plane getById(String id) {
		return planes.get(id);
	}
	
	@Override
	public Plane create(Plane plane) {
		String id = Plane.uniqueId();
		plane.setId(id);
		planes.put(id, plane);
		return plane;
	}
	
	@Override
	public Plane update(String id, Plane plane) {
		if (planes.replace(id, plane) == null) {
			return null;
		}
		return plane;
	}

}
