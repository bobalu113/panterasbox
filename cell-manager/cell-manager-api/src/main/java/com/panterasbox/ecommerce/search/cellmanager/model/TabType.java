package com.panterasbox.ecommerce.search.cellmanager.model;

/**
 * Enumeration of all the possible tab types
 * 
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public enum TabType {
    /** Dashboard tab, only one allowed */
    DASHBOARD("Dashboard", "dashboard.zul", "images/traffic-light-icon.png"),
    /** Workbench tab, one per cell allowed */
    WORKBENCH("Workbench", "workbench.zul", "images/images/wrench.png"),
    /** Reference app tab */
    REFERENCE_APP("Reference App", "reference-app.zul", "images/application-search-result.png"),
    /** Dgraph stats tab */
    DGRAPH_STATS("Dgraph Statistics", "stats.zul", "images/system-monitor.png");
    
    private String name;
    private String content;
    private String image;
    
    /**
     * TabType constructor.
     * @param name tab name
     * @param content tab content
     * @param image tab image, may be null
     */
    TabType(String name, String content, String image) {
        this.name = name;
        this.content = content;
        this.image = image;
    }
    
    /**
     * Get tab name.
     * @return tab name
     */
    public String getName() {
        return name;
    }
    
    /**
     * Get tab content.
     * @return tab content
     */
    public String getContent() {
        return content;
    }
    
    /**
     * Get tab image.
     * @return tab image
     */
    public String getImage() {
        return image;
    }
    
}
