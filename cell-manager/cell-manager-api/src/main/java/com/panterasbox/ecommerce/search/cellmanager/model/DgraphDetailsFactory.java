package com.panterasbox.ecommerce.search.cellmanager.model;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import com.wsgc.ecommerce.context.request.model.RequestContext;
import com.panterasbox.ecommerce.search.cellmanager.service.GenerationQueryService;


/**
 * Factory for creating DgraphDetails instances.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class DgraphDetailsFactory {

    private GenerationQueryService generationQueryService;
    
    /**
     * Create a new DgraphDetails instance for a given host/port, and list of subsystems.
     * @param rc request context
     * @param cell the cell this dgraph belongs to
     * @param hostname the dgraph's hostname
     * @param port the dgraph's port
     * @param subsystemsVal a comma separated list of subsystems
     * @return a DgraphDetails instance for the given host/port
     */
    public DgraphDetails getDgraphDetails(RequestContext rc, CellDetails cell, String hostname, int port, String subsystemsVal) {
        Map<String, Long> activeEcomGenerations = generationQueryService.queryActiveEcomGenerations(rc, cell.getSite(), cell.getInstallation());
        Map<String, Long> metadataGenerations = generationQueryService.queryMetadataGenerations(rc, hostname, port);
        Map<String, Long> keyPropGenerations = generationQueryService.queryKeyPropGenerations(rc, hostname, port);
        
        DgraphDetails dgraphDetails = new DgraphDetails();

        dgraphDetails.setHostname(hostname);
        dgraphDetails.setPort(port);
        
        List<SubsystemDetails> subsystems = new ArrayList<SubsystemDetails>();
        String[] subsystemNames = subsystemsVal.split(",");
        for (String subsystemName : subsystemNames) {
            subsystemName = subsystemName.trim();
            SubsystemDetails subsystemDetails = new SubsystemDetails();
            subsystemDetails.setName(subsystemName);
            if (activeEcomGenerations.get(subsystemName) != null) {
                subsystemDetails.setActiveEcomGeneration(activeEcomGenerations.get(subsystemName));
            }
            if (metadataGenerations.get(subsystemName) != null) {
                subsystemDetails.setMetadataGeneration(metadataGenerations.get(subsystemName));
            }
            if (keyPropGenerations.get(subsystemName) != null) {
                subsystemDetails.setKeyPropertyGeneration(keyPropGenerations.get(subsystemName));
            }
            subsystems.add(subsystemDetails);
        }
        dgraphDetails.setSubsystems(subsystems);
        return dgraphDetails;
    }

    /**
     * Set the generation query service (injected by Spring).
     * @param generationQueryService the generation query service to set
     */
    public void setGenerationQueryService(GenerationQueryService generationQueryService) {
        this.generationQueryService = generationQueryService;
    }

}
