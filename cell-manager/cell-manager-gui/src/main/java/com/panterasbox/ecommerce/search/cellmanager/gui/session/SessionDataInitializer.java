package com.panterasbox.ecommerce.search.cellmanager.gui.session;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;

import javax.sql.DataSource;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.InitializingBean;

import com.wsgc.ecommerce.session.exception.SessionException;

/**
 * Temporary class to initialize some session tables in h2 that
 * aren't in the ecom db yet.  When we create them for real we
 * can get rid of this class.
 * 
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class SessionDataInitializer implements InitializingBean {
    /** Log object */
    private Logger log = LoggerFactory.getLogger(getClass());

    /** Data Source to use */
    private DataSource dataSource;
    
    @Override
    public void afterPropertiesSet() throws Exception {
        Connection con = getConnection();
        
        PreparedStatement createSchemaPS = null;
        PreparedStatement createEmptyBlobFunctionPS = null;
        PreparedStatement createInfoTablePS = null;
        PreparedStatement createDataTablePS = null;

        try {
            String createSchemaQuery = 
                    "CREATE SCHEMA WS_APP_OWNER";
            createSchemaPS = con.prepareStatement(createSchemaQuery);
            createSchemaPS.execute();
            if (createSchemaPS.getUpdateCount() == -1) {
                throw new SessionException("Could not create schema");
            }
            
            String createInfoTableQuery = 
                    "CREATE TABLE WS_APP_OWNER.CELLMANAGER_SESSION_INFO"
                  + "  ("
                  + "    SESSION_KEY  VARCHAR2(32) NOT NULL ,"
                  + "    SECURE_KEY   VARCHAR2(32) ,"
                  + "    TIME_CREATED NUMBER(12, 0) ,"
                  + "    LAST_USED DATE ,"
                  + "    UPDATE_COUNT  NUMBER(9, 0) ,"
                  + "    SESSION_STATE CHAR(1) ,"
                  + "    CONSTRAINT CELLMANAGER_SESINFO_PK PRIMARY KEY ( SESSION_KEY )"
                  + "  )";
            createInfoTablePS = con.prepareStatement(createInfoTableQuery);
            createInfoTablePS.execute();
            if (createInfoTablePS.getUpdateCount() == -1) {
                throw new SessionException("Could not create info table");
            }
            
            String createDataTableQuery = 
                    "CREATE TABLE WS_APP_OWNER.CELLMANAGER_SESSION_DATA"
                  + "  ("
                  + "    SESSION_KEY  VARCHAR2(32) NOT NULL ,"
                  + "    DATA_TYPE    VARCHAR2(12) NOT NULL ,"
                  + "    UPDATE_COUNT NUMBER(9, 0) ,"
                  + "    BLOB_DATA BLOB ,"
                  + "    CONSTRAINT CELLMANAGER_SESDATA_PK PRIMARY KEY ( SESSION_KEY , DATA_TYPE )"
                  + "  )";
            createDataTablePS = con.prepareStatement(createDataTableQuery);
            createDataTablePS.execute();
            if (createDataTablePS.getUpdateCount() == -1) {
                throw new SessionException("Could not create data table");
            }
        } catch (SQLException sqe) {
            throw new SessionException("SQL Exception: " + sqe, sqe);
        } finally {
            close(createSchemaPS);
            close(createEmptyBlobFunctionPS);
            close(createInfoTablePS);
            close(createDataTablePS);
            
            try {
                con.close();
            } catch (SQLException sqe) {
                log.error("Failed to close connection: " + sqe, sqe);
            }
        }
    }
        
    /**
     * Gets a connection to use for session data access.
     * 
     * @return the connection to use
     * 
     * @throws SessionException for problems because of Spring
     */
    protected Connection getConnection() throws SessionException {
        if (dataSource != null) {
            Connection con = null;
            boolean conOk = false;
            try {
                con = dataSource.getConnection();
                con.setAutoCommit(false);
                conOk = true;
                return con;
            } catch (SQLException sqe) {
                throw new SessionException("Failed to obtain database connection: " + sqe, sqe);
            } finally {
                if ((con != null) && (!conOk)) {
                    try {
                        con.close();
                    } catch (SQLException sqe) {
                        log.warn("Unable to close connection during getConnection() failure: " + sqe, sqe);
                    }
                }
            }
        } 
        
        throw new IllegalStateException("Data Source not set in SessionDAO");
    }

    /**
     * Convenience PreparedStatement closing method.
     *
     * @param pStatement the prepared statement to close.
     */
    protected void close(PreparedStatement pStatement) {
       try {
          if (pStatement != null) {
             pStatement.close();
          }
       } catch (SQLException e) {
          log.error("Failed to close prepared statement: " + e, e);
       }
    }

    /**
     * Used to set the data source via the Spring configuration (or manually)
     * 
     * @param dataSourceIn the dataSource to use
     */
    public void setDataSource(DataSource dataSourceIn) {
        dataSource = dataSourceIn;
    }


}
