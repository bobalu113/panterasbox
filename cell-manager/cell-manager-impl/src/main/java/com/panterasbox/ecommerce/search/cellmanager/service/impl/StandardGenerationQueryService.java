package com.panterasbox.ecommerce.search.cellmanager.service.impl;

import java.net.InetSocketAddress;
import java.util.HashMap;
import java.util.Map;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.endeca.navigation.ENEQueryResults;
import com.endeca.navigation.ERec;
import com.endeca.navigation.KeyProperties;
import com.endeca.navigation.Navigation;
import com.endeca.navigation.PropertyMap;
import com.wsgc.ecommerce.caching.PopulatingCache;
import com.wsgc.ecommerce.context.request.model.RequestContext;
import com.panterasbox.ecommerce.search.cellmanager.model.CellManagerConstants;
import com.panterasbox.ecommerce.search.cellmanager.model.MetadataSubsystem;
import com.panterasbox.ecommerce.search.cellmanager.model.SiteAndInstallation;
import com.panterasbox.ecommerce.search.cellmanager.service.GenerationQueryService;

/**
 * Standard implementation of the generation query service.  Uses the EcomDB
 * for getting the active ecom generation and the Endeca navigation API to
 * get the dgraph metadata.
 * 
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class StandardGenerationQueryService implements GenerationQueryService {
    private final Logger log = LoggerFactory.getLogger(getClass());
    
    private PopulatingCache<SiteAndInstallation, Map<String, Long>> activeEcomGenerationsCache;
    
    private PopulatingCache<InetSocketAddress, ENEQueryResults> metadataQueryResultsCache;
    
    /** {@inheritDoc} */
    @Override
    public Map<String, Long> queryActiveEcomGenerations(RequestContext rc, int site, String installation) {
        return activeEcomGenerationsCache.getValue(rc, new SiteAndInstallation(site, installation));
    }

    /** {@inheritDoc} */
    @Override
    public Map<String, Long> queryMetadataGenerations(RequestContext rc, String hostname, int port) {
        Map<String, Long> result = new HashMap<String, Long>();
        
        ENEQueryResults queryResults = metadataQueryResultsCache.getValue(rc, new InetSocketAddress(hostname, port));
        Map<String, String> metadataMap = getMetadataMap(queryResults);
        
        for (MetadataSubsystem subsystem : MetadataSubsystem.values()) {
            if (metadataMap.containsKey(subsystem.getGenerationKey())) {
                try {
                    long generation = Long.valueOf(metadataMap.get(subsystem.getGenerationKey()));
                    result.put(subsystem.name(), generation);
                } catch (NumberFormatException e) {
                    log.warn("Could not parse " + subsystem.name() + " generation value from metadata entry", e);
                }
            }
        }
        return result;
    }
    
    /** {@inheritDoc} */
    @Override
    public Map<String, Long> queryKeyPropGenerations(RequestContext rc, String hostname, int port) {
        Map<String, Long> result = new HashMap<String, Long>();

        ENEQueryResults queryResults = metadataQueryResultsCache.getValue(rc, new InetSocketAddress(hostname, port));
        
        @SuppressWarnings("unchecked")
        Map<String, KeyProperties> keyProperties = (Map<String, KeyProperties>) queryResults.getNavigation().getKeyProperties();
        if (null == keyProperties) {
            log.warn("Failed to retrieve key properties for metadata query result");
            return result;
        }
        
        KeyProperties keyProp = keyProperties.get(CellManagerConstants.GENERATIONS_KEY_PROP);
        if (keyProp == null) {
            log.warn("No Generations key property found for metadata query result");
            return result;
        }
        
        PropertyMap generations = keyProp.getProperties();
        for (MetadataSubsystem subsystem : MetadataSubsystem.values()) {
            if (generations.containsKey(subsystem.name())) {
                try {
                    long generation = Long.valueOf((String) generations.get(subsystem.name()));
                    result.put(subsystem.name(), generation);
                } catch (NumberFormatException e) {
                    log.warn("Could not parse " + subsystem.name() + " generation value from key properties", e);
                }
            }
        }
        
        return result;
    }

    /**
     * Build a map of all metadata keys to their values from the metadata
     * query result.
     * @param queryResults the Endeca metdata query result
     * @return a map of metadata keys to their values
     */
    private Map<String, String> getMetadataMap(ENEQueryResults queryResults) {
        Map<String, String> metadataMap = new HashMap<String, String>();
        Navigation nav = queryResults.getNavigation();
        if (nav != null && nav.getTotalNumERecs() > 0) {
            ERec metadataERec = (ERec) nav.getERecs().get(0);
            PropertyMap propMap = metadataERec.getProperties();
            metadataMap = new HashMap<String, String>();
            for (Map.Entry<?, ?> entry : ((Map<?, ?>) propMap).entrySet()) {
                metadataMap.put(String.valueOf(entry.getKey()), String.valueOf(entry.getValue()));
            }
        }
        return metadataMap;
    }

    /**
     * Set the active ecom generation cache (injected by Spring).
     * @param activeEcomGenerationsCache the activeEcomGenerationCache to set
     */
    public void setActiveEcomGenerationsCache(PopulatingCache<SiteAndInstallation, Map<String, Long>> activeEcomGenerationsCache) {
        this.activeEcomGenerationsCache = activeEcomGenerationsCache;
    }

    /**
     * Set the metadata query results cache (injected by Spring).
     * @param metadataQueryResultsCache the metadataQueryResultsCache to set
     */
    public void setMetadataQueryResultsCache(PopulatingCache<InetSocketAddress, ENEQueryResults> metadataQueryResultsCache) {
        this.metadataQueryResultsCache = metadataQueryResultsCache;
    }

}
