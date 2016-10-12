package com.panterasbox.restsample;

import static org.hamcrest.Matchers.is;
import static org.hamcrest.Matchers.notNullValue;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.delete;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.get;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.post;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.put;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.content;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.jsonPath;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.status;

import com.panterasbox.restsample.controller.JetCharterController;
import com.panterasbox.restsample.dao.PlaneDao;
import com.panterasbox.restsample.model.Plane;
import com.panterasbox.restsample.spring.AppConfig;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.web.servlet.WebMvcTest;
import org.springframework.http.MediaType;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit4.SpringRunner;
import org.springframework.test.web.servlet.MockMvc;
import org.springframework.util.Assert;

import java.nio.charset.Charset;

/**
 * JetCharterController unit tests. Uses Spring's MockMvc support for testing REST calls.
 * 
 * @author Bobby Schaetzle
 */
@RunWith(SpringRunner.class)
@ContextConfiguration(classes = {AppConfig.class})
@WebMvcTest(JetCharterController.class)
public class JetCharterControllerTest {

  private static final double TEST_AIRSPEED = 500.0;
  private static final double TEST_RANGE = 2000.0;
  private static final double TEST_DISTANCE = 5000.0;
  private static final double TEST_FLIGHT_TIME = 660.0;
  
  private MediaType contentType = new MediaType(MediaType.APPLICATION_JSON.getType(),
                                                MediaType.APPLICATION_JSON.getSubtype(),
                                                Charset.forName("utf8"));

  private String testPlaneId;

  @Autowired
  private MockMvc mockMvc;

  @Autowired
  private PlaneDao planeDao;

  /**
   * Setup tests by adding a test plane to the repository.
   */
  @Before
  public void setup() {
    Plane plane = new Plane();
    plane.setAirspeed(TEST_AIRSPEED);
    plane.setRange(TEST_RANGE);
    plane = planeDao.create(plane);
    this.testPlaneId = plane.getId();
  }

  /**
   * Test GET /planes/{testPlaneId} returns our test plane.
   * 
   * @throws Exception if something goes wrong
   */
  @Test
  public void getTestPlane() throws Exception {
    mockMvc.perform(get("/planes/" + testPlaneId))
      .andExpect(status().isOk())
      .andExpect(content().contentType(contentType))
      .andExpect(jsonPath("$.id", is(testPlaneId)))
      .andExpect(jsonPath("$.airspeed", is(TEST_AIRSPEED)))
      .andExpect(jsonPath("$.range", is(TEST_RANGE)));
  }

  /**
   * Test GET /planes/{missingId} returns 404.
   * 
   * @throws Exception if something goes wrong
   */
  @Test
  public void getPlaneNotFound() throws Exception {
    mockMvc.perform(get("/planes/" + planeDao.generateUniqueId()))
      .andExpect(status().isNotFound());
  }

  /**
   * Test POST /planes creates a new plane with the provided payload and a unique id.
   * 
   * @throws Exception if something goes wrong
   */
  @Test
  public void createPlane() throws Exception {
    String planeJson = "{ \"airspeed\": " + TEST_AIRSPEED + ", \"range\": " + TEST_RANGE + " }";
    mockMvc.perform(post("/planes")
      .contentType(contentType)
      .content(planeJson))
      .andExpect(status().isCreated())
      .andExpect(jsonPath("$.id", notNullValue()))
      .andExpect(jsonPath("$.airspeed", is(TEST_AIRSPEED)))
      .andExpect(jsonPath("$.range", is(TEST_RANGE)));

    Plane plane = planeDao.getById(testPlaneId);
    Assert.notNull(plane, "Plane not found in repository");
    Assert.notNull(plane.getId(), "Plane id not assigned");
    Assert.isTrue((plane.getAirspeed() == TEST_AIRSPEED), "Plane airspeed not assigned");
    Assert.isTrue((plane.getRange() == TEST_RANGE), "Plane range not assigned");
  }

  /**
   * Test POST /planes prevents creating a new plane with an existing id.
   * 
   * @throws Exception if something goes wrong
   */
  @Test
  public void createPlaneConflict() throws Exception {
    String id = planeDao.generateUniqueId();
    String planeJson = "{ \"id\": \"" + id 
                       + "\", \"airspeed\": " + TEST_AIRSPEED 
                       + ", \"range\": " + TEST_RANGE + " }";
    mockMvc.perform(post("/planes")
      .contentType(contentType)
      .content(planeJson))
      .andExpect(status().isCreated())
      .andExpect(jsonPath("$.id", is(id)));
    mockMvc.perform(post("/planes")
      .contentType(contentType)
      .content(planeJson))
      .andExpect(status().isConflict());
  }

  /**
   * Test PUT /planes/{testPlaneId} updates a plane with the provided payload and id.
   * 
   * @throws Exception if something goes wrong
   */
  @Test
  public void updatePlane() throws Exception {
    String planeJson = "{ \"airspeed\": " + (TEST_AIRSPEED * 2) 
                       + ", \"range\": " + (TEST_RANGE * 2) + " }";
    mockMvc.perform(put("/planes/" + testPlaneId)
      .contentType(contentType)
      .content(planeJson))
      .andExpect(status().isOk())
      .andExpect(jsonPath("$.id", is(testPlaneId)))
      .andExpect(jsonPath("$.airspeed", is(TEST_AIRSPEED * 2)))
      .andExpect(jsonPath("$.range", is(TEST_RANGE * 2)));
 
    Plane plane = planeDao.getById(testPlaneId);
    Assert.notNull(plane, "Plane not found in repository");
    Assert.isTrue(testPlaneId.equals(plane.getId()), "Plane id does not match");
    Assert.isTrue((plane.getAirspeed() == (TEST_AIRSPEED * 2)), "Plane airspeed not updated");
    Assert.isTrue((plane.getRange() == (TEST_RANGE * 2)), "Plane range not updated");
  }

  /**
   * Test PUT /planes/{missingId} returns 404.
   * 
   * @throws Exception if something goes wrong
   */
  @Test
  public void updatePlaneNotFound() throws Exception {
    String planeJson = "{ \"airspeed\": " + (TEST_AIRSPEED * 2) 
        + ", \"range\": " + (TEST_RANGE * 2) + " }";
    mockMvc.perform(put("/planes/" + planeDao.generateUniqueId())
      .contentType(contentType)
      .content(planeJson))
      .andExpect(status().isNotFound());
  }

  /**
   * Test PUT /planes/{testPlaneId} fails if payload contains mismatched id.
   * 
   * @throws Exception if something goes wrong
   */
  @Test
  public void updatePlaneBadRequest() throws Exception {
    String planeJson = "{ \"id\": \"" + planeDao.generateUniqueId() 
                       + "\", \"airspeed\": " + (TEST_AIRSPEED * 2) 
                       + ", \"range\": " + (TEST_RANGE * 2) + " }";
    mockMvc.perform(put("/planes/" + testPlaneId)
      .contentType(contentType)
      .content(planeJson))
      .andExpect(status().isBadRequest());
  }

  /**
   * Test DELETE /planes/{testPlaneId} deletes a plane with the provided id.
   * 
   * @throws Exception if something goes wrong
   */
  public void deletePlane() throws Exception {
    mockMvc.perform(delete("/planes/" + testPlaneId))
      .andExpect(status().isOk())
      .andExpect(content().contentType(contentType))
      .andExpect(jsonPath("$.id", is(testPlaneId)))
      .andExpect(jsonPath("$.airspeed", is(TEST_AIRSPEED)))
      .andExpect(jsonPath("$.range", is(TEST_RANGE)));

    Plane plane = planeDao.getById(testPlaneId);
    Assert.isNull(plane, "Plane not deleted from repository");
  }

  /**
   * Test DELETE /planes/{missingId} returns 404.
   * 
   * @throws Exception if something goes wrong
   */
  @Test
  public void deletePlaneNotFound() throws Exception {
    mockMvc.perform(delete("/planes/" + planeDao.generateUniqueId())
      .contentType(contentType))
      .andExpect(status().isNotFound());
  }

  /**
   * Test GET /computeFlightTime returns the expected flight time given the provided plane and
   * coordinates.
   * 
   * @throws Exception if something goes wrong
   */
  @Test
  public void computeFlightTime() throws Exception {
    mockMvc.perform(get("/computeFlightTime")
      .param("planeId", testPlaneId)
      .param("fromX", String.valueOf(0.0))
      .param("toX", String.valueOf(TEST_DISTANCE))
      .param("fromY", String.valueOf(0.0))
      .param("toY", String.valueOf(0.0)))
      .andExpect(status().isOk())
      .andExpect(content().contentType(contentType))
      .andExpect(jsonPath("$", is(TEST_FLIGHT_TIME)));
  }
}
