package com.panterasbox.ecommerce.search.cellmanager.model;

/**
 * Some useful constants for dealing with Endeca cells.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public final class CellManagerConstants {

    /**
     * Default constructor, prevent instantiation.
     */
    private CellManagerConstants() { 
        throw new AssertionError();
    }
    
    /** cell name from cellconfig.properties */
    public static final String CELL_NAME_PROP = "endeca.app.name";
    
    /** site id prop cellconfig.properties */
    public static final String SITE_ID_PROP = "siteid";
    
    /** installation from cellconfig.properties */
    public static final String INSTALLATION_PROP = "installation";

    /** last update date from genconfig.properties */
    public static final String LAST_UPDATE_PROP = "baseline.lastupdate.time";

    /** workbench flag from cellconfig.properties */
    public static final String WORKBENCH_PROP = "workbench";

    /** cell status from genconfig.properties */
    public static final String CELL_STATE_PROP = "cell.lifecyclestate";

    /** workbench host from cellconfig.properties */
    public static final String WORKBENCH_HOST_PROP = "endeca.webstudio.hostname";

    /** workbench port from cellconfig.properties */
    public static final String WORKBENCH_PORT_PROP = "endeca.webstudio.port";
    
    /** subsystems prop from cellconfig.properties */
    public static final String SUBSYSTEMS_PROP = "subsystems";

    /** dgraph setting prefix from cellconfig.properties */
    public static final String DGRAPH_PROP_PREFIX = "endeca.mdex.dgraph";
    
    /** dgraph hostname prop from cellconfig.properties */
    public static final String DGRAPH_HOST_PROP = "hostname";
    
    /** dgraph port prop from cellconfig.properties */
    public static final String DGRAPH_PORT_PROP = "port";

    /** the record type key for Endeca record filter */
    public static final String ENDECA_RECORD_TYPE = "Record_Type";
    
    /** the generations key property */
    public static final String GENERATIONS_KEY_PROP = "Generations";
}
