package com.panterasbox.restsample.model;

import java.util.UUID;

public class Plane {

  private String id;
  private double airspeed;
  private double range;

  public Plane() {
  }

  public Plane(double airspeed, double range) {
    this(uniqueId(), airspeed, range);
  }

  public Plane(String id, double airspeed, double range) {
    this.id = id;
    this.airspeed = airspeed;
    this.range = range;
  }

  public void setId(String id) {
    this.id = id;
  }

  public String getId() {
    return id;
  }

  public void setAirspeed(double airspeed) {
    this.airspeed = airspeed;
  }

  public double getAirspeed() {
    return airspeed;
  }

  public void setRange(double range) {
    this.range = range;
  }

  public double getRange() {
    return range;
  }

  public static String uniqueId() {
    return UUID.randomUUID().toString();
  }

}
