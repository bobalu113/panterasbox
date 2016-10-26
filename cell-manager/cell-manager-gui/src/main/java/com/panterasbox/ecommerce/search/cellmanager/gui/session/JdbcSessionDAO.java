package com.panterasbox.ecommerce.search.cellmanager.gui.session;

import java.io.ByteArrayInputStream;
import java.sql.Blob;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.HashMap;
import java.util.Map;

import javax.sql.DataSource;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.jdbc.core.JdbcTemplate;

import com.wsgc.ecommerce.session.dao.SessionDAO;
import com.wsgc.ecommerce.session.exception.SessionException;
import com.wsgc.ecommerce.session.exception.SessionNotFoundException;
import com.wsgc.ecommerce.session.exception.SessionUpdateConflictException;
import com.wsgc.ecommerce.session.model.InternalSession;
import com.wsgc.ecommerce.session.model.SessionConstants;
import com.wsgc.ecommerce.session.model.SessionEntry;
import com.wsgc.ecommerce.session.model.impl.StandardSessionEntry;
import com.wsgc.ecommerce.settings.exception.SettingsException;
import com.wsgc.ecommerce.settings.service.SettingsService;
import com.wsgc.ecommerce.specification.ApplicationSpecification;
import com.wsgc.ecommerce.utilities.dao.CacheableQuery;
import com.wsgc.ecommerce.utilities.dao.ImplicitQueryCache;

/**
 * SessionDAO which uses an RDBMS via a JDBC DataSource 
 * (hacked up from foundation)
 *
 * @version $Id: $
 */
public class JdbcSessionDAO implements SessionDAO {

    /** Data Source to use */
    private DataSource dataSource;

    /** JDBCTemplate for non-oracle queries */
    private JdbcTemplate jdbcTemplate;

    /** DB Setting Service object */
    private SettingsService settingsService;

    /** Info table name */
    private String infoTableName;

    /** Data table name */
    private String dataTableName;

    /** injected applicationSpecification object */
    private ApplicationSpecification appSpecification;

    /** Log object */
    private Logger log = LoggerFactory.getLogger(getClass());

    /** Cache of query text */
    private ImplicitQueryCache cachedQueries = new ImplicitQueryCache();

    /**
     * Gets the configured name for the session info table.
     * 
     * @return the session info table's name (suitable for passing into
     *         ApplicationSpecification.getTable())
     */
    private String getInfoTableName() {
        return infoTableName;
    }

    /**
     * Gets the configured name for the session data table.
     * 
     * @return the session data table's name (suitable for passing into
     *         ApplicationSpecification.getTable())
     */
    private String getDataTableName() {
        return dataTableName;
    }

    /**
     * Used to set the data source via the Spring configuration (or manually)
     * 
     * @param dataSourceIn the dataSource to use
     */
    public void setDataSource(DataSource dataSourceIn) {
        dataSource = dataSourceIn;
        jdbcTemplate = new JdbcTemplate(dataSource);
    }

    /**
     * Used to get the data source via the Spring configuration (or manually)
     * 
     * @return dataSource the dataSource to use
     */
    public DataSource getDataSource() {
        return dataSource;
    }

    /**
     * Used to set the ApplicationSpecification object from Spring
     * configuration
     * 
     * @param appSpecificationIn ApplicationSpecification to use
     */
    public synchronized void setAppSpecification(ApplicationSpecification appSpecificationIn) {
        appSpecification = appSpecificationIn;
        cachedQueries.setApplicationSpecification(appSpecification);
    }

    /**
     * Used to set the db settings service via the Spring configuration (or
     * manually)
     * 
     * @param settingsServiceIn settings service to use
     * @throws SettingsException for problems with that service
     */
    public void setSettingsService(SettingsService settingsServiceIn) throws SettingsException {
        settingsService = settingsServiceIn;
    }

    /**
     * Gets a connection to use for session data access.
     * 
     * @return the connection to use
     * 
     * @throws SessionException for problems because of Spring
     */
    protected Connection getConnection() throws SessionException {
        // TODO this code could be refactored to a slightly less interesting flow.
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

    private final CacheableQuery createSessionQuery = new CacheableQuery() {
        public String getQueryString(ApplicationSpecification spec) {
            StringBuilder buf = new StringBuilder();
            buf.append("insert into ");
            buf.append(spec.getTable(getInfoTableName()));
            buf.append(" (SESSION_KEY, LAST_USED, UPDATE_COUNT, SESSION_STATE, TIME_CREATED) values (?, sysdate, 1, 'A', ?)");
            return buf.toString();
        }
    };

    /** {@inheritDoc} */
    @Override
    public boolean create(String sessionId, String dateTimeCreated) throws SessionException {
        // Check the connection
        Connection con = getConnection();
        boolean doRollback = true;
        
        PreparedStatement setInfoPS = null;
        
        try {
            String setInfoQuery = cachedQueries.getQueryString(createSessionQuery);
            setInfoPS = con.prepareStatement(setInfoQuery);
            setInfoPS.setString(1, sessionId);
            setInfoPS.setString(2, dateTimeCreated);
            int infoResult = setInfoPS.executeUpdate();
            if (infoResult != 1) {
                return false;
            }
            doRollback = false;
            con.commit();
            return true;
        } catch (SQLException sqe) {
            log.warn("Failed to create session: " + sqe, sqe);
            return false;
        } finally {
            try {
                if (doRollback) {
                    con.rollback();
                }
            } catch (SQLException sqe) {
                log.error("Failed to rollback connection: " + sqe, sqe);
            }
            
            close(setInfoPS);
            
            
            try {
                con.close();
            } catch (SQLException sqe) {
                log.error("Failed to close connection: " + sqe, sqe);
            }
        }
    }

    private final CacheableQuery lockSessionQuery = new CacheableQuery() {
        public String getQueryString(ApplicationSpecification spec) {
            StringBuilder buf = new StringBuilder();
            buf.append("update ");
            buf.append(spec.getTable(getInfoTableName()));
            buf.append(" set LAST_USED = SYSDATE where SESSION_KEY = ? and TIME_CREATED = ? and SESSION_STATE = 'A'");
            return buf.toString();
        }
    };
    
    private final CacheableQuery lockSessionQueryWithSecureId = new CacheableQuery() {
        public String getQueryString(ApplicationSpecification spec) {
            StringBuilder buf = new StringBuilder();
            buf.append("update ");
            buf.append(spec.getTable(getInfoTableName()));
            buf.append(" set LAST_USED = SYSDATE where SESSION_KEY = ? and TIME_CREATED = ?");
            buf.append(" and SECURE_KEY = ? and SESSION_STATE = 'A'");
            return buf.toString();
        }
    };
    
    private final CacheableQuery getSessionUpdateCountAndSSIDQuery = new CacheableQuery() {
        public String getQueryString(ApplicationSpecification spec) {
            StringBuilder buf = new StringBuilder();
            buf.append("select UPDATE_COUNT, SECURE_KEY from ");
            buf.append(spec.getTable(getInfoTableName()));
            buf.append(" where SESSION_KEY = ? and TIME_CREATED = ? and SESSION_STATE = 'A'");
            return buf.toString();
        }
    };

    private final CacheableQuery getUpdatedDataQuery = new CacheableQuery() {
        public String getQueryString(ApplicationSpecification spec) {
            StringBuilder buf = new StringBuilder();
            buf.append("select DATA_TYPE, UPDATE_COUNT, BLOB_DATA from ");
            buf.append(spec.getTable(getDataTableName()));
            buf.append(" where SESSION_KEY = ? and UPDATE_COUNT > ?");
            return buf.toString();
        }
    };

    /** {@inheritDoc} */
    @Override
    public void sync(InternalSession intSession) throws SessionNotFoundException, SessionException {
        // Check the connection
        Connection con = getConnection();
        
        PreparedStatement updTimePS = null;
        PreparedStatement selDeltasPS = null;
        PreparedStatement getInfoPS = null;
        
        boolean doRollback = true;
        try {
            String sessionId = intSession.getSessionID();
            int intUpdCnt = intSession.getUpdateCount();

            int dbUpdCnt = 0;
            if (log.isDebugEnabled()) {
                log.debug("SessionDAO getInfoLock for session: " + sessionId);
            }
            
            // Lock session and update SSID in database if present
            String intSessionSSID = intSession.getSecureSessionID();
            String updInfoQuery = cachedQueries.getQueryString((intSessionSSID != null) ? lockSessionQueryWithSecureId : lockSessionQuery);
            updTimePS = con.prepareStatement(updInfoQuery);
            updTimePS.setString(1, sessionId);
            updTimePS.setString(2, intSession.getDateTimeCreated());
            if (intSessionSSID != null) {             
                updTimePS.setString(3, intSessionSSID);
            }
            
            int infoResult = updTimePS.executeUpdate();
            if (infoResult != 1) {
                throw new SessionNotFoundException("Could not find session to update");
            }
            String getInfoQuery = cachedQueries.getQueryString(getSessionUpdateCountAndSSIDQuery);
            getInfoPS = con.prepareStatement(getInfoQuery);
            getInfoPS.setString(1, sessionId);
            getInfoPS.setString(2, intSession.getDateTimeCreated());
            ResultSet getInfoRS = getInfoPS.executeQuery();
            if (!getInfoRS.next()) {
                getInfoRS.close();
                throw new SessionNotFoundException("Could not get update from session");
            }
            // get update count
            dbUpdCnt = getInfoRS.getInt(1);
            intSession.setSecureSessionID(getInfoRS.getString(2));
            getInfoRS.close();

            // db update count should never be lower than what we have
            if (dbUpdCnt < intUpdCnt) {
                String warnMsg = "SessionId: " + sessionId + " int update count: " + intUpdCnt + " db update count: " + dbUpdCnt;
                log.warn(warnMsg);
                throw new SessionNotFoundException(warnMsg);
            }

            // if db update count equals internal count we're in sync
            if (dbUpdCnt == intUpdCnt) {
                doRollback = false;
                con.commit();
                return;
            }

            // db update count is higher so it has updates. Grab those
            // and apply to internal session.
            String selDeltasQuery = cachedQueries.getQueryString(getUpdatedDataQuery);
            selDeltasPS = con.prepareStatement(selDeltasQuery);
            selDeltasPS.setString(1, sessionId);
            selDeltasPS.setInt(2, intUpdCnt);
            ResultSet selDeltasRS = selDeltasPS.executeQuery();
            while (selDeltasRS.next()) {
                String curDataType = selDeltasRS.getString(1);
                int newUpdateCount = selDeltasRS.getInt(2);

                if (log.isDebugEnabled()) {
                    log.debug("got the session data row with update count higher than internal count: " + intUpdCnt + " dataType: " + curDataType);
                }

                Blob blobData = selDeltasRS.getBlob(3);
                
                byte[] newData = blobData.getBytes(1, (int) blobData.length());

                intSession.put(curDataType, new StandardSessionEntry(newUpdateCount, newData));
            }
            selDeltasRS.close();
            intSession.setUpdateCount(dbUpdCnt);
            doRollback = false;
            con.commit();
        } catch (SQLException sqe) {
            throw new SessionException("SQL Exception: " + sqe, sqe);
        } finally {
            try {
                if (doRollback) {
                    con.rollback();
                }
            } catch (SQLException sqe) {
                log.error("Failed to rollback connection: " + sqe, sqe);
            }
            
            close(updTimePS);
            close(selDeltasPS);
            close(getInfoPS);
            
            try {
                con.close();
            } catch (SQLException sqe) {
                log.error("Failed to close connection: " + sqe, sqe);
            }
        }
    }
    
    private final CacheableQuery invalidateSessionQuery = new CacheableQuery() {
        public String getQueryString(ApplicationSpecification spec) {
            StringBuilder buf = new StringBuilder();
            buf.append("update  ");
            buf.append(spec.getTable(getInfoTableName()));
            buf.append(" set SESSION_STATE = ? where SESSION_KEY = ? and TIME_CREATED = ? and SESSION_STATE = 'A'");
            return buf.toString();
        }
    };
    
    /** {@inheritDoc} */
    @Override
    public void invalidate(String sessionId, String dateTimeCreated) {
        jdbcTemplate.update(cachedQueries.getQueryString(invalidateSessionQuery), new Object[] {"D", sessionId, dateTimeCreated});
    }
    
    

    private final CacheableQuery incrementUpdateQuery = new CacheableQuery() {
        public String getQueryString(ApplicationSpecification spec) {
            StringBuilder buf = new StringBuilder();
            buf.append("update ");
            buf.append(spec.getTable(getInfoTableName()));
            buf.append(" set UPDATE_COUNT = UPDATE_COUNT + 1, LAST_USED = SYSDATE where SESSION_KEY = ? and TIME_CREATED = ? and SESSION_STATE = 'A'");
            return buf.toString();
        }
    };

    private final CacheableQuery incrementUpdateAndSSIDQuery = new CacheableQuery() {
        public String getQueryString(ApplicationSpecification spec) {
            StringBuilder buf = new StringBuilder();
            buf.append("update ");
            buf.append(spec.getTable(getInfoTableName()));
            buf.append(" set UPDATE_COUNT = UPDATE_COUNT + 1, LAST_USED = SYSDATE, SECURE_KEY = ?");
            buf.append(" where SESSION_KEY = ? and TIME_CREATED = ? and SESSION_STATE = 'A' and (SECURE_KEY is NULL or SECURE_KEY = ?)");
            return buf.toString();
        }
    };

    private final CacheableQuery insertDataQuery = new CacheableQuery() {
        public String getQueryString(ApplicationSpecification spec) {
            StringBuilder buf = new StringBuilder();
            buf.append("insert into ");
            buf.append(spec.getTable(getDataTableName()));
            buf.append(" (SESSION_KEY, DATA_TYPE, UPDATE_COUNT, BLOB_DATA)");
            buf.append(" values(?, ?, -1, ?)");
            return buf.toString();
        }
    };

    private final CacheableQuery updateDataQuery = new CacheableQuery() {
        public String getQueryString(ApplicationSpecification spec) {
            StringBuilder buf = new StringBuilder();
            buf.append("select SESSION_KEY, DATA_TYPE, UPDATE_COUNT, BLOB_DATA from ");
            buf.append(spec.getTable(getDataTableName()));
            buf.append(" where SESSION_KEY = ? and DATA_TYPE = ? for update");
            return buf.toString();
        }
    };

    private final CacheableQuery h2UpdateDataQuery = new CacheableQuery() {
        public String getQueryString(ApplicationSpecification spec) {
            StringBuilder buf = new StringBuilder();
            buf.append("update ");
            buf.append(spec.getTable(getDataTableName()));
            buf.append(" set UPDATE_COUNT = ?, BLOB_DATA = ? ");
            buf.append(" where SESSION_KEY = ? and DATA_TYPE = ?");
            return buf.toString();
        }
    };

    /** {@inheritDoc} */
    @Override
    public void commit(InternalSession intSession, 
            Map<String, SessionEntry> sesEntries, String secureId) throws SessionException, SessionNotFoundException, SessionUpdateConflictException {
        // Check the connection
        boolean doRollback = true;
        Connection con  = getConnection();
        
        PreparedStatement updInfoPS = null;
        PreparedStatement getInfoPS = null;
        PreparedStatement insertEntryPS = null;
        PreparedStatement updateEntryPS = null;
        PreparedStatement h2UpdateEntryPS = null;
        PreparedStatement selDeltasPS = null;

        try {
            String sessionId = intSession.getSessionID();
            int intUpdCnt = intSession.getUpdateCount();
            int dbUpdCnt;

            if (log.isDebugEnabled()) {
                log.debug("SessionDAO commit for session: " + sessionId);
            }
            String updInfoQuery = cachedQueries.getQueryString((secureId == null) ? incrementUpdateQuery : incrementUpdateAndSSIDQuery);

            updInfoPS = con.prepareStatement(updInfoQuery);
           if (secureId == null) {
                updInfoPS.setString(1, sessionId);
                updInfoPS.setString(2, intSession.getDateTimeCreated());
            } else {
                updInfoPS.setString(1, secureId);
                updInfoPS.setString(2, sessionId);
                updInfoPS.setString(3, intSession.getDateTimeCreated());
                updInfoPS.setString(4, secureId);
            }
            
            int infoResult = updInfoPS.executeUpdate();
            if (infoResult != 1) {
                throw new SessionNotFoundException("Could not find session to update");
            }

            String getInfoQuery = cachedQueries.getQueryString(getSessionUpdateCountAndSSIDQuery);
            getInfoPS = con.prepareStatement(getInfoQuery);
            getInfoPS.setString(1, sessionId);
            getInfoPS.setString(2, intSession.getDateTimeCreated());
            ResultSet getInfoRS = getInfoPS.executeQuery();
            if (!getInfoRS.next()) {
                getInfoRS.close();
                throw new SessionNotFoundException("Could not get update from session");
            }
            dbUpdCnt = getInfoRS.getInt(1);
            getInfoRS.close();

            // new db update count should never be lower or equal
            // than what we have already internally
            if (dbUpdCnt <= intUpdCnt) {
                String warnMsg = "SessionId: " + sessionId + " int update count: " + intUpdCnt + " db update count: " + dbUpdCnt;
                log.warn(warnMsg);
                throw new SessionNotFoundException(warnMsg);
            }

            // if db update count is higher than internal value there have
            // been changes to the session. pull them in and compare with
            // the passed in map of changes to the session.
            Map<String, SessionEntry> syncMap = new HashMap<String, SessionEntry>(sesEntries.size());
            /* If the dbUpdCnt is MORE then 1 higher than internal update count then we have changes to account for.
             */
            if (dbUpdCnt > (intUpdCnt + 1)) {
                String selDeltasQuery = cachedQueries.getQueryString(getUpdatedDataQuery);
                selDeltasPS = con.prepareStatement(selDeltasQuery);
                selDeltasPS.setString(1, sessionId);
                selDeltasPS.setInt(2, intUpdCnt);
                ResultSet selDeltasRS = selDeltasPS.executeQuery();
                while (selDeltasRS.next()) {
                    String curDataType = selDeltasRS.getString(1);
                    if (log.isDebugEnabled()) {
                        log.debug("got the session data row with update" + " count higher than internal count: " + intUpdCnt + " dataType: " + curDataType);
                    }

                    if (sesEntries.containsKey(curDataType)) {
                        String warnMsg = "SessionId: " + sessionId + " key: " + curDataType + " int update count: " + intUpdCnt + " db update count: "
                                + dbUpdCnt;
                        log.warn(warnMsg);
                        throw new SessionUpdateConflictException("Session update conflict for '" + curDataType + "'");
                    }
                }
                selDeltasRS.close();
            }

            // TODO Break this method up at this point, Checkstyle is unhappy.
            // okay to update the data now... loop over sesEntries
            for (Map.Entry<String, SessionEntry> ent : sesEntries.entrySet()) {
                String keyValue = ent.getKey();
                SessionEntry theEntry =  ent.getValue();
                // make a new entry with the right update count
                byte[] newData = theEntry.getDataBytes();
                SessionEntry newEntry = new StandardSessionEntry(dbUpdCnt, newData);
                syncMap.put(keyValue, newEntry);

                if (theEntry.getUpdateCount() == 0) {
                    // NEW entry
                    if (insertEntryPS == null) {
                        String insDataQuery = cachedQueries.getQueryString(insertDataQuery);
                        insertEntryPS = con.prepareStatement(insDataQuery);
                    }

                    insertEntryPS.setString(1, sessionId);
                    insertEntryPS.setString(2, keyValue);
                    insertEntryPS.setBlob(3, con.createBlob());
                    int insResult = insertEntryPS.executeUpdate();
                    if (insResult != 1) {
                        throw new SessionException("Failed to insert new data row for session '" + sessionId + "' key '" + keyValue + "'");
                    }
                }
                
                /* H2 doesn't support updatable Blob
                if (updateEntryPS == null) {
                    String updDataQuery = cachedQueries.getQueryString(updateDataQuery);
                    updateEntryPS = con.prepareStatement(updDataQuery, ResultSet.TYPE_FORWARD_ONLY, ResultSet.CONCUR_UPDATABLE);
                }

                updateEntryPS.setString(1, sessionId);
                updateEntryPS.setString(2, keyValue);

                ResultSet selUpdDataRS = updateEntryPS.executeQuery();
                if (!selUpdDataRS.next()) {
                    throw new SessionException("Failed to find existing data row to update  for session '" + sessionId + "' key '" + keyValue + "'");
                }
                if (log.isDebugEnabled()) {
                    log.debug("got the session data row");
                }
                
                selUpdDataRS.updateInt(3, dbUpdCnt);
                Blob blobData = selUpdDataRS.getBlob(4);
                
                int toGo = newData.length;
                int pos = 0;
                while (toGo > 0) {
                    int setCount = blobData.setBytes(pos + 1, newData, pos, toGo);
                    toGo -= setCount;
                }

                selUpdDataRS.updateRow();
                selUpdDataRS.close();
                */

                if (h2UpdateEntryPS == null) {
                    String h2UpdDataQuery = cachedQueries.getQueryString(h2UpdateDataQuery);
                    h2UpdateEntryPS = con.prepareStatement(h2UpdDataQuery, ResultSet.TYPE_FORWARD_ONLY, ResultSet.CONCUR_READ_ONLY);
                }
                h2UpdateEntryPS.setInt(1, dbUpdCnt);
                h2UpdateEntryPS.setBinaryStream(2, new ByteArrayInputStream(newData));
                h2UpdateEntryPS.setString(3, sessionId);
                h2UpdateEntryPS.setString(4, keyValue);
                h2UpdateEntryPS.executeUpdate();
            }

            for (Map.Entry<String, SessionEntry> ent : syncMap.entrySet()) {
                String keyValue =  ent.getKey();
                SessionEntry theEntry = ent.getValue();
                intSession.put(keyValue, theEntry);
            }
            
            if (secureId != null) {
                if (log.isDebugEnabled()) {
                    log.debug("Setting secure id " + secureId + " for session id " + intSession.getSessionID());
                }
                intSession.setSecureSessionID(secureId);
            }
            intSession.setUpdateCount(dbUpdCnt);
            
            doRollback = false;
            con.commit();
        } catch (SQLException sqe) {
            throw new SessionException("SQL Exception: " + sqe, sqe);
        } finally {
            try {
                if (doRollback) {
                    con.rollback();
                }
            } catch (SQLException sqe) {
                log.error("Failed to rollback connection: " + sqe, sqe);
            }
            
            close(updInfoPS);
            close(getInfoPS);
            close(insertEntryPS);
            close(updateEntryPS);
            close(selDeltasPS);
            
            try {
                con.close();
            } catch (SQLException sqe) {
                log.error("Failed to close connection: " + sqe, sqe);
            }
        }
    }

    /**
     * Ensure that a dataSource has been set for this bean. NOTE this is required after the settingsService has been set
     * and before the first SQL query. 
     * 
     * TODO the exception declaration could be a little more specific.
     * @throws Exception if the bean has not been initialized with a dataSource
     */
    public void init() throws Exception {
        if (dataSource == null) {
            throw new SessionException("Must set dataSource property on " + "StandardSessionService instance");
        }
        if (appSpecification == null) {
            throw new SessionException("Must set applicationSpecification property on " + "StandardSessionDAO instance");
        }
        infoTableName = settingsService.getStringSetting(SessionConstants.SESSION_SETTINGS_GROUP, SessionConstants.INFO_TABLENAME_SETTING_NAME);
        dataTableName = settingsService.getStringSetting(SessionConstants.SESSION_SETTINGS_GROUP, SessionConstants.DATA_TABLENAME_SETTING_NAME);
    }

    /**
     * The number of minutes in a day.
     */
    private static final int MINUTES_PER_DAY = 24 * 60;

    private final CacheableQuery markExpiredQuery = new CacheableQuery() {
        public String getQueryString(ApplicationSpecification spec) {
            StringBuilder buf = new StringBuilder();
            buf.append("update ");
            buf.append(spec.getTable(getInfoTableName()));
            buf.append(" set SESSION_STATE = 'D' where SESSION_STATE = 'A' and LAST_USED < (SYSDATE - ( ? / ");
            buf.append(String.valueOf(MINUTES_PER_DAY));
            buf.append(") )");
            return buf.toString();
        }
    };

    private final CacheableQuery deleteExpiredDataQuery = new CacheableQuery() {
        public String getQueryString(ApplicationSpecification spec) {
            StringBuilder buf = new StringBuilder();
            buf.append("delete from (");
            buf.append("select /*+ ordered */ d.SESSION_KEY,d.DATA_TYPE from ");
            buf.append(spec.getTable(getInfoTableName()));
            buf.append(" i, ");
            buf.append(spec.getTable(getDataTableName()));
            buf.append(" d where i.SESSION_STATE = 'D' and d.SESSION_KEY = i.SESSION_KEY");
            buf.append(")");
            return buf.toString();
        }
    };

    private final CacheableQuery deleteExpiredInfoQuery = new CacheableQuery() {
        public String getQueryString(ApplicationSpecification spec) {
            StringBuilder buf = new StringBuilder();
            buf.append("delete from ");
            buf.append(spec.getTable(getInfoTableName()));
            buf.append(" where SESSION_STATE = 'D'");
            return buf.toString();
        }
    };

    private final CacheableQuery deleteOrphanedDataQuery = new CacheableQuery() {
        public String getQueryString(ApplicationSpecification spec) {
            StringBuilder buf = new StringBuilder();
            buf.append("delete from ");
            buf.append(spec.getTable(getDataTableName()));
            buf.append(" where SESSION_KEY not in (");
            buf.append("select SESSION_KEY from ");
            buf.append(spec.getTable(getInfoTableName()));
            buf.append(")");
            return buf.toString();
        }
    };

    /** {@inheritDoc} */
    @Override
    public int expireOld(int maxAgeInMinutes, boolean removeStrandedData) {
        if (maxAgeInMinutes < 1) {
            throw new IllegalArgumentException("Attempted to run expireOld with a max age of " + maxAgeInMinutes + " minute(s)");
        }

        if (log.isDebugEnabled()) {
            log.debug("Expiring old sessions (maxAgeInMinutes=" + maxAgeInMinutes + ")");
        }

        int deletedThings = 0;
        Integer maxAgeInt = new Integer(maxAgeInMinutes);

        // Mark all old 'A'ctive sessions as 'D'eleted
        jdbcTemplate.update(cachedQueries.getQueryString(markExpiredQuery), new Object[] {maxAgeInt});

        // Now remove all data for 'D'eleted sessions
        deletedThings += jdbcTemplate.update(cachedQueries.getQueryString(deleteExpiredDataQuery));

        // Now remove all info for 'D'eleted sessions
        deletedThings += jdbcTemplate.update(cachedQueries.getQueryString(deleteExpiredInfoQuery));

        if (removeStrandedData) {
            log.debug("Expiring orphaned session data...");

            // Now remove all data for missing sessions
            deletedThings += jdbcTemplate.update(cachedQueries.getQueryString(deleteOrphanedDataQuery));
        }

        if (log.isDebugEnabled()) {
            log.debug("Expiration removed " + deletedThings + " rows.");
        }

        return deletedThings;
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

}