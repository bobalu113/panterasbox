package com.panterasbox.ecommerce.search.cellmanager.model;

import java.util.Date;
import java.util.List;

/**
 * Model object representing an Endeca cell.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class CellDetails {
    private String name;
    private int site;
    private String installation;
    private Date lastUpdate;
    private boolean workbenchEnabled;
    private CellStatus status;
    private String workbenchHost;
    private int workbenchPort;
    private List<DgraphDetails> dgraphs;

    /** 
     * Get the cell name.
     * @return cell name 
     */
    public String getName() {
        return name;
    }

    /** 
     * Set the cell name.
     * @param name the cell name
     */
    public void setName(String name) {
        this.name = name;
    }

    /** 
     * Get the cell's site.
     * @return the cell's site property 
     */
    public int getSite() {
        return site;
    }

    /**
     * Set the cell site property.
     * @param site the cell's site
     */
    public void setSite(int site) {
        this.site = site;
    }
    
    /** 
     * Get the cell's installation property.
     * @return the cell's installation property 
     */
    public String getInstallation() {
        return installation;
    }
    
    /**
     * Set the cell's installation property.
     * @param installation the installation
     */
    public void setInstallation(String installation) {
        this.installation = installation;
    }
    
    /**
     * Get the cell's last update time. 
     * @return the last time the cell was indexed 
     */
    public Date getLastUpdate() {
        return lastUpdate;
    }
    
    /** 
     * Set the last update time for this cell.
     * @param lastUpdate the time the cell was last indexed
     */
    public void setLastUpdate(Date lastUpdate) {
        this.lastUpdate = lastUpdate;
    }
    
    /** 
     * Check whether the cell is workbench enabled
     * @return true if this is a workbench cell 
     */
    public boolean isWorkbenchEnabled() {
        return workbenchEnabled;
    }
    
    /**
     * Set whether or not this is a workbench enabled cell.
     * @param workbenchEnabled true if this is a workbench cell
     */
    public void setWorkbenchEnabled(boolean workbenchEnabled) {
        this.workbenchEnabled = workbenchEnabled;
    }
    
    /** 
     * Get the current cell state.
     * @return the current cell state 
     */
    public CellStatus getStatus() {
        return status;
    }

    /**
     * Set the cell's current state.
     * @param status cell status
     */
    public void setStatus(CellStatus status) {
        this.status = status;
    }
    
    /** 
     * Get the hostname running this cell's workbench application.
     * @return the workbench host
     */
    public String getWorkbenchHost() {
        return workbenchHost;
    }

    /**
     * Set the hostname running this cell's workbench application.
     * @param workbenchHost the workbench host
     */
    public void setWorkbenchHost(String workbenchHost) {
        this.workbenchHost = workbenchHost;
    }

    /** 
     * Get the port number this cell's workbench application is running on.
     * @return the workbench port
     */
    public int getWorkbenchPort() {
        return workbenchPort;
    }

    /**
     * Set the port number this cell's workbench application is running on.
     * @param workbenchPort workbench port
     */
    public void setWorkbenchPort(int workbenchPort) {
        this.workbenchPort = workbenchPort;
    }

    /** 
     * Get the list of all configured dgraphs for this cell.
     * @return the list of dgraphs
     */
    public List<DgraphDetails> getDgraphs() {
        return dgraphs;
    }

    /** 
     * Set the list of all configured dgraphs for this cell.
     * @param dgraphs the list of dgraphs
     */
    public void setDgraphs(List<DgraphDetails> dgraphs) {
        this.dgraphs = dgraphs;
    }

    /** {@inheritDoc} */
    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("CellDetails(name=");
        sb.append(String.valueOf(name));
        sb.append(", site=");
        sb.append(String.valueOf(site));
        sb.append(", installation=");
        sb.append(String.valueOf(installation));
        sb.append(", lastUpdate=");
        sb.append(String.valueOf(lastUpdate));
        sb.append(", workbenchEnabled=");
        sb.append(String.valueOf(workbenchEnabled));
        sb.append(", workbenchHost=");
        sb.append(String.valueOf(workbenchHost));
        sb.append(", workbenchPort=");
        sb.append(String.valueOf(workbenchPort));
        sb.append(")");
        return sb.toString();
    }
    
    /** {@inheritDoc} */
    @Override
    public boolean equals(Object other) {
        boolean result = false;
        if (other instanceof CellDetails) {
            CellDetails cell = (CellDetails) other;
            result = ((name == null ? cell.getName() == null : name.equals(cell.getName()))
                   && (site == cell.getSite())
                   && (installation == null ? cell.getInstallation() == null : installation.equals(cell.getInstallation()))
                   && (lastUpdate == null ? cell.getLastUpdate() == null : lastUpdate.equals(cell.getLastUpdate()))
                   && (workbenchEnabled == cell.isWorkbenchEnabled())
                   && (status == cell.getStatus())
                   && (workbenchHost == null ? cell.getWorkbenchHost() == null : workbenchHost.equals(cell.getWorkbenchHost()))
                   && (workbenchPort == cell.getWorkbenchPort())
                   && (dgraphs == null ? cell.getDgraphs() == null : dgraphs.equals(cell.getDgraphs())));
        }
        return result;
    }
    
    /** {@inheritDoc} */
    @Override
    public int hashCode() {
        int hash = 11;
        hash = 31 * hash + (name == null ? 0 : name.hashCode());
        hash = 31 * hash + site;
        hash = 31 * hash + (installation == null ? 0 : installation.hashCode());
        hash = 31 * hash + (lastUpdate == null ? 0 : lastUpdate.hashCode());
        hash = 31 * hash + (workbenchEnabled ? 1231 : 1237);
        hash = 31 * hash + (status == null ? 0 : status.hashCode());
        hash = 31 * hash + (workbenchHost == null ? 0 : workbenchHost.hashCode());
        hash = 31 * hash + workbenchPort;
        hash = 31 * hash + (dgraphs == null ? 0 : dgraphs.hashCode());
        return hash;
    }
}
