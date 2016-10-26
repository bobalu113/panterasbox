package com.panterasbox.ecommerce.search.cellmanager.service;

import java.util.Map;

import com.wsgc.ecommerce.context.request.model.RequestContext;

/**
 * This service is used to retrieve generation numbers for the 
 * subsystems which the Endeca applications listen to.  Since
 * generations are configured in different places across the
 * application stack, this service can be used to make sure they
 * are all in sync.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public interface GenerationQueryService {

    /**
     * Query the active generation from the Ecom database all known
     * subsystems.  Theoretically, any Endeca cells in active use should
     * match the active generation.
     * @param rc request context
     * @param site site id
     * @param installation installation
     * @return a map of subsystems to generation ids
     */
    Map<String, Long> queryActiveEcomGenerations(RequestContext rc, int site, String installation);
   
    /**
     * Query the generation returned a by Dgraph's metadata dimension for all known
     * subsystems.  This value is configured during the extract process.
     * @param rc request context
     * @param hostname the hostname of the Dgraph to query
     * @param port the port of the Dgraph to query
     * @return a map of subsystems to generation ids
     */
    Map<String, Long> queryMetadataGenerations(RequestContext rc, String hostname, int port);
   
    /**
     * Query the generation returned a by Dgraph's key properties for all known
     * subsystems.  This value is configured during the extract process.
     * @param rc request context
     * @param hostname the hostname of the Dgraph to query
     * @param port the port of the Dgraph to query
     * @return a map of subsystems to generation ids
     */
    Map<String, Long> queryKeyPropGenerations(RequestContext rc, String hostname, int port);
}
