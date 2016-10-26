package com.panterasbox.ecommerce.search.cellmanager.model;

/**
 * A utility class for passing around site and installation values
 * together.  Used to look up active ecom generations.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class SiteAndInstallation {

    private int site;
    
    private String installation;
    
    /**
     * Default constructor, site and installation values both required.
     * @param site the site
     * @param installation the installation
     */
    public SiteAndInstallation(int site, String installation) {
        this.site = site;
        this.installation = installation;
    }

    /**
     * Get the site.
     * @return the site
     */
    public int getSite() {
        return site;
    }

    /**
     * Get the installation.
     * @return the installation
     */
    public String getInstallation() {
        return installation;
    }
    
    /** {@inheritDoc} */
    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("SiteAndInstallation(site=");
        sb.append(site);
        sb.append(", installation=");
        sb.append(installation);
        sb.append(")");
        return sb.toString();
    }
    
    /** {@inheritDoc} */
    @Override
    public boolean equals(Object other) {
        boolean result = false;
        if (other instanceof SiteAndInstallation) {
            SiteAndInstallation siteAndInstallation = (SiteAndInstallation) other;
            result = ((site == siteAndInstallation.getSite()
                   && installation == null ? siteAndInstallation.getInstallation() == null : installation.equals(siteAndInstallation.getInstallation())));
        }
        return result;
    }
    
    /** {@inheritDoc} */
    @Override
    public int hashCode() {
        int hash = 11;
        hash = 31 * hash + site;
        hash = 31 * hash + (installation == null ? 0 : installation.hashCode());
        return hash;
    }
}
