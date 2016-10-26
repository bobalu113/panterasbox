package com.panterasbox.ecommerce.search.cellmanager.model;

/**
 * Enumeration of known subsystems and their keys in the metadata result.
 * 
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public enum MetadataSubsystem {
    /** CAT subsystem */
    CAT("Catalog_Generation"),

    /** ENDECA subsystem */
    ENDECA("Endeca_Generation"),
    
    /** RECIPE subsystem */
    RECIPE("Recipe_Generation"),
    
    /** TMPL subsystem */
    TMPL("Template_Generation");
    
    private final String generationKey;

    /**
     * Default constructor.
     * @param generationKey the key to the subsystem's generation in the metadata result
     */
    private MetadataSubsystem(String generationKey) {
        this.generationKey = generationKey;
    }
    
    /**
     * Get the generation key.
     * @return the key to the subsystem's generation in the metadata result
     */
    public String getGenerationKey() {
        return generationKey;
    }
}
