package com.panterasbox.ecommerce.search.cellmanager.model;

/**
 * Model object to represent a tab/tabpanel in the workspace area.
 * 
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class WorkspaceTab {

    private TabType type;

    private String name;
    
    private String content;
    
    private String image;
    
    /**
     * Constructor without image.
     * @param name tab name
     * @param content URI of content which should be included into the panel
     */
    public WorkspaceTab(String name, String content) {
        this.type = null;
        this.name = name;
        this.content = content;
    }
    
    /**
     * Constructor with image.
     * @param name tab name
     * @param content URI of content which should be included into the panel
     * @param image URI of image to accompany tab name
     */
    public WorkspaceTab(String name, String content, String image) {
        this(name, content);
        this.image = image;
    }

    /**
     * Constructor using tab type.
     * @param type the tab type
     */
    public WorkspaceTab(TabType type) {
        this(type.getName(), type.getContent(), type.getImage());
        this.type = type;
    }

    /**
     * Get the tab type.
     * @return the tab type
     */
    public TabType getType() {
        return type;
    }
    
    /**
     * Get the tab name.
     * @return the tab name
     */
    public String getName() {
        return name;
    }

    /**
     * Get the URI of the content which should be included into the panel.
     * @return the tabpanel URI
     */
    public String getContent() {
        return content;
    }

    /**
     * Get the tab image.
     * @return the URI of tab image
     */
    public String getImage() {
        return image;
    }    
}
