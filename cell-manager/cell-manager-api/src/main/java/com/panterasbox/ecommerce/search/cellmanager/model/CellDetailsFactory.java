package com.panterasbox.ecommerce.search.cellmanager.model;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Properties;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.wsgc.ecommerce.context.request.model.RequestContext;

/**
 * Factory for creating CellDetails instances from properties files.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class CellDetailsFactory {
    private Logger log = LoggerFactory.getLogger(getClass());

    private DgraphDetailsFactory dgraphDetailsFactory;
    
    /**
     * Create a new CellDetails instance from the cellconfig and genconfig
     * properties files.
     * @param rc request context
     * @param cellConfig from cellconfig.properties
     * @param genConfig from genconfig.properties
     * @return a CellDetails instance for the given properties files
     */
    public CellDetails getCellDetails(RequestContext rc, Properties cellConfig, Properties genConfig) {
        CellDetails cellDetails = new CellDetails();
        
        cellDetails.setName(cellConfig.getProperty(CellManagerConstants.CELL_NAME_PROP).trim());
        cellDetails.setInstallation(cellConfig.getProperty(CellManagerConstants.INSTALLATION_PROP).trim());
        cellDetails.setSite(Integer.valueOf(cellConfig.getProperty(CellManagerConstants.SITE_ID_PROP).trim()));
        cellDetails.setWorkbenchEnabled(Boolean.valueOf(cellConfig.getProperty(CellManagerConstants.WORKBENCH_PROP).trim()));
        try {
            Date lastUpdate = new SimpleDateFormat("EEE MMM dd HH:mm:ss z yyyy")
               .parse(genConfig.getProperty(CellManagerConstants.LAST_UPDATE_PROP).trim());
            cellDetails.setLastUpdate(lastUpdate);
        } catch (ParseException e) {
            log.warn("Could not parse last update time for cell: " + cellDetails.getName(), e);
        }
        cellDetails.setStatus(CellStatus.valueOf(genConfig.getProperty(CellManagerConstants.CELL_STATE_PROP).trim()));
        
        if (cellDetails.isWorkbenchEnabled()) {
            cellDetails.setWorkbenchHost(cellConfig.getProperty(CellManagerConstants.WORKBENCH_HOST_PROP).trim());
            cellDetails.setWorkbenchPort(Integer.valueOf(cellConfig.getProperty(CellManagerConstants.WORKBENCH_PORT_PROP).trim()));
        }
        
        List<DgraphDetails> dgraphs = new ArrayList<DgraphDetails>();
        int dgraphIndex = 1;
        while (true) {
            String hostKey = CellManagerConstants.DGRAPH_PROP_PREFIX + dgraphIndex + "." + CellManagerConstants.DGRAPH_HOST_PROP;
            String portKey = CellManagerConstants.DGRAPH_PROP_PREFIX + dgraphIndex + "." + CellManagerConstants.DGRAPH_PORT_PROP;
            if (!cellConfig.containsKey(hostKey) || !cellConfig.containsKey(portKey)) { break; }
            DgraphDetails dgraph = dgraphDetailsFactory.getDgraphDetails(rc, cellDetails,
                                                                         cellConfig.getProperty(hostKey).trim(), 
                                                                         Integer.valueOf(cellConfig.getProperty(portKey).trim()),
                                                                         cellConfig.getProperty(CellManagerConstants.SUBSYSTEMS_PROP).trim());
            dgraphs.add(dgraph);
            dgraphIndex++;
        }
        cellDetails.setDgraphs(dgraphs);
        
        return cellDetails;
    }
    
    /**
     * Set the DgraphDetailsFactory (injected by spring).
     * @param dgraphDetailsFactory the dgraphDetailsFactory
     */
    public void setDgraphDetailsFactory(DgraphDetailsFactory dgraphDetailsFactory) {
        this.dgraphDetailsFactory = dgraphDetailsFactory;
    }
}
