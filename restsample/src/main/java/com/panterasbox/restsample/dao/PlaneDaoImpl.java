package com.panterasbox.restsample.dao;

import com.panterasbox.restsample.model.Plane;

import org.springframework.stereotype.Repository;

import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

/**
 * An implementation of PlaneDao backed by in-memory data store. This is a Spring repository bean.
 * 
 * @author Bobby Schaetzle
 */
@Repository
public class PlaneDaoImpl implements PlaneDao {

  private ConcurrentMap<String, Plane> planes;

  /**
   * Default constructor.
   */
  public PlaneDaoImpl() {
    planes = new ConcurrentHashMap<String, Plane>();
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public Plane getById(String id) {
    return planes.get(id);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public Plane create(Plane plane) {
    if (plane.getId() == null) {
      plane.setId(generateUniqueId());
    }
    if (planes.putIfAbsent(plane.getId(), plane) != null) {
      return null;
    }
    return plane;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public Plane update(String id, Plane plane) {
    if (planes.replace(id, plane) == null) {
      return null;
    }
    return plane;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public Plane delete(String id) {
    return planes.remove(id);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public String generateUniqueId() {
    return UUID.randomUUID().toString();
  }

}
