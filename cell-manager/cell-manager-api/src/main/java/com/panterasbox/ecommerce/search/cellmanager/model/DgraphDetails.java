package com.panterasbox.ecommerce.search.cellmanager.model;

import java.util.List;

/**
 * Model object representing an cell's mdex.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class DgraphDetails {
    private String hostname;
    private int port;
    private List<SubsystemDetails> subsystems;
    
    /**
     * Get the configured hostname for this dgraph.
     * @return the hostname
     */
    public String getHostname() {
        return hostname;
    }
    
    /**
     * Set the configured hostname for this dgraph.
     * @param hostname the hostname
     */
    public void setHostname(String hostname) {
        this.hostname = hostname;
    }

    /**
     * Get the configured port for this dgraph.
     * @return the port
     */
    public int getPort() {
        return port;
    }

    /**
     * Set the configured port for this dgraph.
     * @param port the port
     */
    public void setPort(int port) {
        this.port = port;
    }
    
    /**
     * Get the generation info for all subsystems this mdex should
     * be watching.
     * @return the list of subsystems for this mdex
     */
    public List<SubsystemDetails> getSubsystems() {
        return subsystems;
    }

    /**
     * Set the generation info for all subsystems this mdex should
     * be watching.
     * @param subsystems the list of subsystems for this mdex
     */
    public void setSubsystems(List<SubsystemDetails> subsystems) {
        this.subsystems = subsystems;
    }
    
    /** {@inheritDoc} */
    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("DgraphDetails(");
        sb.append(String.valueOf(hostname));
        sb.append(":");
        sb.append(port);
        sb.append(")");
        return sb.toString();
    }
    
    /** {@inheritDoc} */
    @Override
    public boolean equals(Object other) {
        boolean result = false;
        if (other instanceof DgraphDetails) {
            DgraphDetails dgraph = (DgraphDetails) other;
            result = ((hostname == null ? dgraph.getHostname() == null : hostname.equals(dgraph.getHostname()))
                   && port == dgraph.getPort()
                   && (subsystems == null ? dgraph.getSubsystems() == null : subsystems.equals(dgraph.getSubsystems())));
        }
        return result;
    }
    
    /** {@inheritDoc} */
    @Override
    public int hashCode() {
        int hash = 11;
        hash = 31 * hash + (hostname == null ? 0 : hostname.hashCode());
        hash = 31 * hash + port;
        hash = 31 * hash + (subsystems == null ? 0 : subsystems.hashCode());
        return hash;
    }
    
}
