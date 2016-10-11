package com.panterasbox.restsample.spring;

import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;
import org.springframework.web.servlet.config.annotation.EnableWebMvc;

/**
 * Configure Spring beans using annotation-based configuration.
 * 
 * @author Bobby Schaetzle
 */
@Configuration
@EnableWebMvc
@ComponentScan(basePackages = "com.panterasbox.restsample")
public class AppConfig {

}
