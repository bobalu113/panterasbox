package com.panterasbox.ecommerce.search.cellmanager.service.impl;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.HashMap;
import java.util.Map;

import javax.sql.DataSource;

import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.core.ResultSetExtractor;

import com.wsgc.ecommerce.specification.ApplicationSpecification;

/**
 * Query functions for the Ecom generations table. Most of this logic was ported
 * over from {@link com.wsgc.ecommerce.admin.management.generation.dao.DatabaseGenerationManagementDAO}
 * in baselogic's admin service. Should consider consolidating the code as a component.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class EcomGenerationQueryDAO {

    private ApplicationSpecification appSpecification;
    private JdbcTemplate jdbcTemplate;
    private String queryBySiteAndInstallation;

    /**
     * A result set extractor that expects a result set with five columns,
     * the subsystem, generation, refresh, site, and installation, and returns a Map suitable for
     * return from the getGenerations method.
     *
     * @author Daniel Stephens
     * @version $Id: DatabaseGenerationManagementDAO.java 223905 2013-02-18 18:40:47Z dstephens $
     */
    @SuppressWarnings("rawtypes")
    private static class GenerationMapExtractor implements ResultSetExtractor {
        /** {@inheritDoc} */
        @Override
        public Object extractData(ResultSet rs) throws SQLException {
            Map<String, Long> result = new HashMap<String, Long>();
            while (rs.next()) {
                String subsystem = rs.getString(1);
                long generation = rs.getLong(2);
                result.put(subsystem.intern(), generation);
            }
            return result;
        }
    }
    
    /**
     * Singleton instance of the extractor
     */
    private static final GenerationMapExtractor EXTRACTOR = new GenerationMapExtractor();

    /**
     * Gets the query string for this data based on the application
     * specification.
     *
     * @return the current query string for the data
     */
    private synchronized String getQueryBySiteAndInstallation() {
        if (queryBySiteAndInstallation == null) {
            StringBuffer buf = new StringBuffer();
            buf.append("select SUBSYSTEM, GENERATION from ");
            buf.append(appSpecification.getTable("WW_ACTIVE_GENERATIONS"));
            buf.append(" where SITE=? and INSTALLATION=?");
            queryBySiteAndInstallation = buf.toString();
        }
        return queryBySiteAndInstallation;
    }

    /** 
     * Get the active generations for the give site and installation.
     * 
     * @param site the site id
     * @param installation the installation for which to obtain generations
     * @return a map of subsystem to generation id
     */
    @SuppressWarnings("unchecked")
    public Map<String, Long> getGenerations(int site, String installation) {
        if (installation == null) {
            throw new NullPointerException("Installation is null!");
        }
        return (Map<String, Long>) jdbcTemplate.query(getQueryBySiteAndInstallation(),
                new Object[] {new Integer(site), installation}, EXTRACTOR);
    }
    
    /**
     * Sets the datasource to be used for queries.
     * 
     * @param dataSource DataSource
     */
    public void setDataSource(DataSource dataSource) {
        this.jdbcTemplate = new JdbcTemplate(dataSource);
    }

    /**
     * Sets the application specification with which to configure this DAO.
     * 
     * @param appSpecification Application Specification
     */
    public void setAppSpecification(ApplicationSpecification appSpecification) {
        this.appSpecification = appSpecification;
    }
    
}
