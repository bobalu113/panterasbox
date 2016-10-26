package com.panterasbox.ecommerce.search.cellmanager.service;

import java.util.List;

import com.wsgc.ecommerce.context.request.model.RequestContext;
import com.panterasbox.ecommerce.search.cellmanager.model.CellDetails;
import com.panterasbox.ecommerce.search.cellmanager.model.DgraphDetails;

/**
 * This service is used to retrieve configuration information
 * for Endeca applications running on this host.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public interface CellInformationService {

    /**
     * Retrieve a list of all available cell names.
     * @return the list of available cells
     */
    List<String> getAvailableCells();
    
    /**
     * Model the given cell based on it's cell configuration.
     * @param rc request context
     * @param cellName cell name to model
     * @return the cell details object
     */
    CellDetails getCellDetails(RequestContext rc, String cellName);

    /**
     * Refresh all properties of a list of CellDetails instances 
     * with any values that have changed since its last refresh.
     * The size of the list may grow or shrink if cells have been
     * removed or added.
     * @param rc request context
     * @param staleCells the list of current cell data to refresh
     */
    void refreshCells(RequestContext rc, List<CellDetails> staleCells);

    /**
     * Refresh all properties of list of DgraphDetails instances 
     * with property values from a "fresh" list.  The size of the 
     * list may grow or shrink if cells have been removed or 
     * added in the fresh list.
     * @param staleDgraphs the list of dgraph data to refresh
     * @param freshDgraphs the list of dgraphs from which refreshed data will come
     */
    void refreshDgraphs(List<DgraphDetails> staleDgraphs, List<DgraphDetails> freshDgraphs);

}
