package com.panterasbox.ecommerce.search.cellmanager.model;

/**
 * Model object representing the subsystems a cell is indexed against.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class SubsystemDetails {
    private String name;
    private long metadataGeneration;
    private long keyPropertyGeneration;
    private long activeEcomGeneration;

    /**
     * Get the name of this subsystem.
     * @return subsystem name
     */
    public String getName() {
        return name;
    }

    /**
     * Set the name of this subsystem.
     * @param name subsystem name
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * Get the generation id saved to cell metadata for this subsystem.
     * @return the generation id
     */
    public long getMetadataGeneration() {
        return metadataGeneration;
    }

    /**
     * Set the generation id saved to cell metadata for this subsystem.
     * @param metadataGen the generation id
     */
    public void setMetadataGeneration(long metadataGen) {
        this.metadataGeneration = metadataGen;
    }
    
    /**
     * Get the generation id saved to cell keyprops for this subsystem.
     * @return the generation id
     */
    public long getKeyPropertyGeneration() {
        return keyPropertyGeneration;
    }
    
    /**
     * Set the generation id saved to cell keyprops for this subsystem.
     * @param keyPropGen the generation id
     */
    public void setKeyPropertyGeneration(long keyPropGen) {
        this.keyPropertyGeneration = keyPropGen;
    }
    
    /**
     * Get the active generation for this subsystem in the Ecom database.
     * @return the generation id
     */
    public long getActiveEcomGeneration() {
        return activeEcomGeneration;
    }
    
    /**
     * Set the active generation for this subsystem in the Ecom database.
     * @param activeGen the generation id
     */
    public void setActiveEcomGeneration(long activeGen) {
        this.activeEcomGeneration = activeGen;
    }

    /** {@inheritDoc} */
    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("SubsystemDetails(");
        sb.append(String.valueOf(name));
        sb.append(": ");
        sb.append(metadataGeneration);
        sb.append(" ");
        sb.append(keyPropertyGeneration);
        sb.append(" ");
        sb.append(activeEcomGeneration);
        sb.append(")");
        return sb.toString();
    }
    
    /** {@inheritDoc} */
    @Override
    public boolean equals(Object other) {
        boolean result = false;
        if (other instanceof SubsystemDetails) {
            SubsystemDetails subsystem = (SubsystemDetails) other;
            result = ((name == null ? subsystem.getName() == null : name.equals(subsystem.getName()))
                   && metadataGeneration == subsystem.getMetadataGeneration()
                   && keyPropertyGeneration == subsystem.getKeyPropertyGeneration()
                   && activeEcomGeneration == subsystem.getActiveEcomGeneration());
        }
        return result;
    }
    
    /** {@inheritDoc} */
    @Override
    public int hashCode() {
        int hash = 11;
        hash = 31 * hash + (name == null ? 0 : name.hashCode());
        hash = 31 * hash + (int) (metadataGeneration ^ (metadataGeneration >>> 32));
        hash = 31 * hash + (int) (keyPropertyGeneration ^ (keyPropertyGeneration >>> 32));
        hash = 31 * hash + (int) (activeEcomGeneration ^ (activeEcomGeneration >>> 32));
        return hash;
    }
}
