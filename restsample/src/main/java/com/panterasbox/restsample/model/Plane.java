package com.panterasbox.restsample.model;

/**
 * Model object representing a Plane. 
 * 
 * @author Bobby Schaetzle
 */
public class Plane {

  private String id;
  private double airspeed;
  private double range;

  /**
   * Default constructor.
   */
  public Plane() {
  }

  /**
   * Construct a new Plane with the specified id.
   * 
   * @param id the plane's id
   * @param airspeed the plane's airspeed, in knots
   * @param range the plane's range, in nautical miles
   */
  public Plane(String id, double airspeed, double range) {
    this.id = id;
    this.airspeed = airspeed;
    this.range = range;
  }

  /**
   * Set the plane's id.
   * 
   * @param id the id
   */
  public void setId(String id) {
    this.id = id;
  }

  /**
   * Get the plane's id.
   * 
   * @return the id
   */
  public String getId() {
    return id;
  }

  /**
   * Set the plane's airspeed.
   * 
   * @param airspeed the airspeed, in knots
   */
  public void setAirspeed(double airspeed) {
    this.airspeed = airspeed;
  }

  /**
   * Get the plane's airspeed.
   * 
   * @return the airspeed, in knots
   */
  public double getAirspeed() {
    return airspeed;
  }

  /**
   * Set the plane's range.
   * 
   * @param range the range, in nautical miles
   */
  public void setRange(double range) {
    this.range = range;
  }

  /**
   * Get the plane's range.
   * 
   * @return the range, in nautical miles
   */
  public double getRange() {
    return range;
  }

}
