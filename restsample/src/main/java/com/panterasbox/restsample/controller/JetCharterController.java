package com.panterasbox.restsample.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.PutMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import com.panterasbox.restsample.dao.PlaneDao;
import com.panterasbox.restsample.model.Plane;

@RestController
public class JetCharterController {

	private static final long REFUEL_TIME = 30;
	
	@Autowired
	private PlaneDao planeDao;
	
	@GetMapping("/planes/{id}")
	public ResponseEntity<?> getPlane(@PathVariable("id") String id) {
		Plane plane = planeDao.getById(id);
		if (plane == null) {
			return new ResponseEntity<String>("No plane found for id " + id, HttpStatus.NOT_FOUND);
		} 
		return new ResponseEntity<Plane>(plane, HttpStatus.OK);
	}	
	
	@PostMapping("/planes")
	public ResponseEntity<?> createPlane(@RequestBody Plane plane) {
		plane = planeDao.create(plane);
		return new ResponseEntity<Plane>(plane, HttpStatus.CREATED);
	}
	
	@PutMapping("/planes/{id}")
	public ResponseEntity<?> updatePlane(@PathVariable String id, @RequestBody Plane plane) {
		if (plane.getId() == null) {
			plane.setId(id);
		} else if(!id.equals(plane.getId())) {
			return new ResponseEntity<String>("Plane id does not match request " + id, HttpStatus.BAD_REQUEST);
		}

		plane = planeDao.update(id, plane);

		if (plane == null) {
			return new ResponseEntity<String>("No plane found for id " + id, HttpStatus.NOT_FOUND);
		}

		return new ResponseEntity<Plane>(plane, HttpStatus.OK);	
	}
	
	@GetMapping("/computeFlightTime")
	public ResponseEntity<?> computeFlightTime(@RequestParam String planeId, 
                                     		   @RequestParam double fromX, @RequestParam double fromY,
                                               @RequestParam double toX, @RequestParam double toY) {
		Plane plane = planeDao.getById(planeId);
		if (plane == null) {
			return new ResponseEntity<String>("No plane found for id " + planeId, HttpStatus.BAD_REQUEST);
		}
		
		double distance = Math.sqrt(Math.pow(fromX - toX, 2) + Math.pow(fromY - toY, 2));
		double time = distance / plane.getAirspeed();
		long stops = (long) (distance / plane.getRange());
		time += (stops * REFUEL_TIME);
		
		return new ResponseEntity<Double>(time, HttpStatus.OK);
	}
	
}
