package com.panterasbox.restsample;

import static org.hamcrest.Matchers.is;
import static org.hamcrest.Matchers.notNullValue;
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

@RunWith(SpringRunner.class)
@ContextConfiguration(classes = {AppConfig.class})
@WebMvcTest(JetCharterController.class)
public class JetCharterControllerTest {

  private MediaType contentType = new MediaType(MediaType.APPLICATION_JSON.getType(),
                                                MediaType.APPLICATION_JSON.getSubtype(),
                                                Charset.forName("utf8"));

  private String testPlaneId;

  @Autowired
  private MockMvc mockMvc;

  @Autowired
  private PlaneDao planeDao;

  @Before
  public void setup() {
    Plane plane = new Plane();
    plane.setAirspeed(1.0);
    plane.setRange(3.0);
    this.testPlaneId = planeDao.create(plane).getId();
  }

  @Test
  public void getTestPlane() throws Exception {
    mockMvc.perform(get("/planes/" + testPlaneId))
      .andExpect(status().isOk())
      .andExpect(content().contentType(contentType))
      .andExpect(jsonPath("$.id", is(testPlaneId)))
      .andExpect(jsonPath("$.airspeed", is(1.0)))
      .andExpect(jsonPath("$.range", is(3.0)));
  }

  @Test
  public void getPlaneNotFound() throws Exception {
    mockMvc.perform(get("/planes/" + Plane.uniqueId()))
      .andExpect(status().isNotFound());
  }

  @Test
  public void createPlane() throws Exception {
    String planeJson = "{ \"airspeed\": 1.0, \"range\": 3.0 }";
    mockMvc.perform(post("/planes")
      .contentType(contentType)
      .content(planeJson))
      .andExpect(status().isCreated())
      .andExpect(jsonPath("$.id", notNullValue()))
      .andExpect(jsonPath("$.airspeed", is(1.0)))
      .andExpect(jsonPath("$.range", is(3.0)));

    Plane plane = planeDao.getById(testPlaneId);
    Assert.notNull(plane, "Plane not found in repository");
    Assert.notNull(plane.getId(), "Plane id not assigned");
    Assert.isTrue((plane.getAirspeed() == 1.0), "Plane airspeed not assigned");
    Assert.isTrue((plane.getRange() == 3.0), "Plane range not assigned");
  }

  @Test
  public void updatePlane() throws Exception {
    String planeJson = "{ \"airspeed\": 2.0, \"range\": 6.0 }";
    mockMvc.perform(put("/planes/" + testPlaneId)
      .contentType(contentType)
      .content(planeJson))
      .andExpect(status().isOk())
      .andExpect(jsonPath("$.id", is(testPlaneId)))
      .andExpect(jsonPath("$.airspeed", is(2.0)))
      .andExpect(jsonPath("$.range", is(6.0)));
 
    Plane plane = planeDao.getById(testPlaneId);
    Assert.notNull(plane, "Plane not found in repository");
    Assert.isTrue(testPlaneId.equals(plane.getId()), "Plane id does not match");
    Assert.isTrue((plane.getAirspeed() == 2.0), "Plane airspeed not updated");
    Assert.isTrue((plane.getRange() == 6.0), "Plane range not updated");
  }

  @Test
  public void updatePlaneNotFound() throws Exception {
    String planeJson = "{ \"airspeed\": 2.0, \"range\": 6.0 }";
    mockMvc.perform(put("/planes/" + Plane.uniqueId())
      .contentType(contentType)
      .content(planeJson))
      .andExpect(status().isNotFound());
  }

  @Test
  public void updatePlaneBadRequest() throws Exception {
    String planeJson = "{ \"id\": \"" + Plane.uniqueId() 
                       + "\", \"airspeed\": 2.0, \"range\": 6.0 }";
    mockMvc.perform(put("/planes/" + testPlaneId)
      .contentType(contentType)
      .content(planeJson))
      .andExpect(status().isBadRequest());
  }

  @Test
  public void computeFlightTime() throws Exception {
    mockMvc.perform(get("/computeFlightTime")
      .param("planeId", testPlaneId)
      .param("fromX", String.valueOf(0.0))
      .param("toX", String.valueOf(10.0))
      .param("fromY", String.valueOf(0.0))
      .param("toY", String.valueOf(0.0)))
      .andExpect(status().isOk())
      .andExpect(content().contentType(contentType))
      .andExpect(jsonPath("$", is(100.0)));
  }
}
