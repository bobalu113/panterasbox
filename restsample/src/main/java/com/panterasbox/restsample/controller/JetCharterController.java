package com.panterasbox.restsample.controller;

import com.panterasbox.restsample.dao.PlaneDao;
import com.panterasbox.restsample.model.Plane;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.PutMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

/**
 * SpringMVC RestController for providing the JetCharter REST API. 
 * Provides CRUD operations on Plane records and a function to 
 * compute flight times.
 * 
 * @author Bobby Schaetzle
 */
@RestController
public class JetCharterController {

  /**
   * Amount of time to refuel, in minutes.
   */
  private static final long REFUEL_TIME = 30;

  @Autowired
  private PlaneDao planeDao;

  /**
   * GET handler for retrieving a Plane record by id. Returns status code 200 if found, 404 if not.
   * 
   * @param id the id of the plane to get
   * @return a Spring ResponseEntity with the Plane record or error in the body
   */
  @GetMapping("/planes/{id}")
  public ResponseEntity<?> getPlane(@PathVariable("id") String id) {
    Plane plane = planeDao.getById(id);
    if (plane == null) {
      return new ResponseEntity<String>("No plane found for id " + id, HttpStatus.NOT_FOUND);
    } 
    return new ResponseEntity<Plane>(plane, HttpStatus.OK);
  }

  /**
   * POST handler for creating a new Plane record. The plane object should contain valid values for
   * airspeed and range. If the object does not contain an id, a unique id will be generated for 
   * it. Returns status code 201 for success, or 409 if an object with the provided id already
   * exists.
   * 
   * @param plane the plane to create 
   * @return a Spring ResponseEntity with the newly created Plane record in the body
   */
  @PostMapping("/planes")
  public ResponseEntity<?> createPlane(@RequestBody Plane plane) {
    String id = plane.getId();
    plane = planeDao.create(plane);
    if (plane == null) {
      return new ResponseEntity<String>("Plane already exists with id " + id, HttpStatus.CONFLICT);
    }
    return new ResponseEntity<Plane>(plane, HttpStatus.CREATED);
  }

  /**
   * PUT handler for updating an existing Plane record. The plane object should contain valid 
   * values for airspeed and range. Returns status code 200 for successful update, 404 if the plane
   * record was not found, or 400 if the object provides an id that does not match resource URI.
   * 
   * @param id the id of the plane to update
   * @param plane the updated plane object 
   * @return a Spring ResponseEntity with the updated Plane record in the body
   */
  @PutMapping("/planes/{id}")
  public ResponseEntity<?> updatePlane(@PathVariable String id, @RequestBody Plane plane) {
    if (plane.getId() == null) {
      plane.setId(id);
    } else if (!id.equals(plane.getId())) {
      return new ResponseEntity<String>("Plane id " + plane.getId() + " does not match request id " 
                                        + id, HttpStatus.BAD_REQUEST);
    }

    plane = planeDao.update(id, plane);

    if (plane == null) {
      return new ResponseEntity<String>("No plane found for id " + id, HttpStatus.NOT_FOUND);
    }

    return new ResponseEntity<Plane>(plane, HttpStatus.OK);
  }

  /**
   * DELETE handler for deleting a Plane record by id. Returns status code 200 if found, 404 if 
   * not.
   * 
   * @param id the id of the plane to delete
   * @return a Spring ResponseEntity with the deleted Plane record or error in the body
   */
  @DeleteMapping("/planes/{id}")
  public ResponseEntity<?> deletePlane(@PathVariable("id") String id) {
    Plane plane = planeDao.delete(id);
    if (plane == null) {
      return new ResponseEntity<String>("No plane found for id " + id, HttpStatus.NOT_FOUND);
    } 
    return new ResponseEntity<Plane>(plane, HttpStatus.OK);
  }
  
  /**
   * GET handler which calculates the flight time of a trip across a two-dimensional coordinate 
   * space for a specified plane. The base time is a function of airspeed and distance, as 
   * determined by specified start and finish coordinates (units in nautical miles). The refuel 
   * time is a function of range and distance, with each refueling stop contributing 30 minutes. 
   * The total time is the sum of base time and refuel time.
   * 
   * @param planeId the id of the plane taking the trip
   * @param fromX the X coordinate of the starting point
   * @param fromY the Y coordinate of the starting point
   * @param toX the Y coordinate of the starting point
   * @param toY the Y coordinate of the finish point
   * @return a Spring ResponseEntity with the the total flight time (in minutes) in the body
   */
  @GetMapping("/computeFlightTime")
  public ResponseEntity<?> computeFlightTime(@RequestParam String planeId, 
                                             @RequestParam double fromX, 
                                             @RequestParam double fromY,
                                             @RequestParam double toX, 
                                             @RequestParam double toY) {
    Plane plane = planeDao.getById(planeId);
    if (plane == null) {
      return new ResponseEntity<String>("No plane found for id " + planeId, HttpStatus.BAD_REQUEST);
    }

    double distance = Math.sqrt(Math.pow(fromX - toX, 2) + Math.pow(fromY - toY, 2));
    double time = (distance / plane.getAirspeed()) * 60;
    long stops = (long) (distance / plane.getRange());
    time += (stops * REFUEL_TIME);

    return new ResponseEntity<Double>(time, HttpStatus.OK);
  }

}
