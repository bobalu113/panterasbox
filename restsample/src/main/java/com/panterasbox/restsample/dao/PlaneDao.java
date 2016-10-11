package com.panterasbox.restsample.dao;

import com.panterasbox.restsample.model.Plane;

/**
 * Interface for Plane data access objects. Provides the basic CRUD operations.
 * 
 * @author Bobby Schaetzle
 */
public interface PlaneDao {

  /**
   * Look up a plane record by its id.
   * 
   * @param id the id of the plane to get
   * @return a Plane instance for the record, or null if not found 
   */
  Plane getById(String id);

  /**
   * Create a new plane record.
   * 
   * @param plane a Plane instance for the created record
   * @return a Plane instance for the created record, or null if id isn't unique
   */
  Plane create(Plane plane);

  /**
   * Update an existing plane record.
   * 
   * @param id the id of the plane to update
   * @param plane a Plane instance for the updated record
   * @return a Plane instance with the updated data, or null if not found
   */
  Plane update(String id, Plane plane);

  /**
   * Delete a plane record.
   * 
   * @param id the id of the plane to delete
   * @return a Plane instance for the deleted record, or null if not found
   */
  Plane delete(String id);
  
  /**
   * Generate a unique plane id.
   *  
   * @return a random id which is not already taken
   */
  String generateUniqueId();

}
