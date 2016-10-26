package com.panterasbox.ecommerce.search.cellmanager.caching;

import java.net.InetSocketAddress;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.sf.ehcache.Element;

import com.endeca.navigation.DimValIdList;
import com.endeca.navigation.ENEQueryException;
import com.endeca.navigation.ENEQueryResults;
import com.endeca.navigation.HttpENEConnection;
import com.endeca.navigation.UrlENEQuery;
import com.endeca.navigation.UrlENEQueryParseException;
import com.endeca.navigation.UrlGen;
import com.wsgc.ecommerce.caching.AbstractElementFactory;
import com.wsgc.ecommerce.context.request.model.RequestContext;
import com.panterasbox.ecommerce.search.cellmanager.model.CellManagerConstants;
import com.panterasbox.ecommerce.search.cellmanager.model.CellManagerException;

/**
 * This class runs a Endeca metadata query against a specified host and and port,
 * and returns an element object which will be kept in cache for future use.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class MetadataQueryResultsElementFactory extends AbstractElementFactory<InetSocketAddress, ENEQueryResults> {
    private Logger log = LoggerFactory.getLogger(getClass());

    @Override
    public Element createElement(long ttl, RequestContext context, InetSocketAddress key) {
        log.debug("refresh metadata cache");
        if (key == null) {
            throw new NullPointerException("MetadataQueryResults cache key cannot be null");
        }
        ENEQueryResults queryResults = null;
        try {
            queryResults = executeMetadataQuery(key.getHostName(), key.getPort());
        } catch (ENEQueryException e) {
            throw new CellManagerException("Caught exception querying Dgraph " + key, e);
        }
        if (queryResults == null) {
            throw new CellManagerException("Unable to get metadata query results");
        }
        return new Element(key, queryResults);
    }

    
    /**
     * Execute a metadata query to the Endeca Dgraph on the given hostname
     * and port.  
     * @param hostname the hostname of the dgraph to query
     * @param port the port of the dgraph to query
     * @return an Endeca query results instance containing metadata values
     * @throws ENEQueryException if the query could not be completed
     */
    private ENEQueryResults executeMetadataQuery(String hostname, int port) throws ENEQueryException {
        UrlENEQuery query = null;

        UrlGen metadataUrl = new UrlGen("", "UTF-8");

        StringBuilder sb = new StringBuilder("AND(");
        sb.append(CellManagerConstants.ENDECA_RECORD_TYPE).append(":");
        sb.append("metadata");
        sb.append(")");
        try {
            query = new UrlENEQuery(metadataUrl.toString(), "UTF-8");
            query.setNavRecordFilter(sb.toString());
        } catch (UrlENEQueryParseException e) {
            throw new CellManagerException("Unable to construct metadata URL query", e);
        }
        
        query.setNavDescriptors(new DimValIdList("0"));
        query.setNavKeyProperties("all");

        HttpENEConnection httpENEConnection = new HttpENEConnection(hostname, port);
        if (log.isDebugEnabled()) {
            log.debug("Connecting to " + hostname + ":" + port + " for metadata query");
        }
        
        return httpENEConnection.query(query);
    }
    
}
