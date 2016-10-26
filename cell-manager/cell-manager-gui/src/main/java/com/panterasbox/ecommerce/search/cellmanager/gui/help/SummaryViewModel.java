package com.panterasbox.ecommerce.search.cellmanager.gui.help;

import org.zkoss.zk.ui.select.annotation.VariableResolver;
import org.zkoss.zk.ui.select.annotation.WireVariable;

/**
 * ViewModel for displaying the application summary window.
 * 
 * @author Bobby Schaetzle
 * @version $Id: $
 */
@VariableResolver(org.zkoss.zkplus.spring.DelegatingVariableResolver.class)
public class SummaryViewModel {

    @WireVariable
    private String cellManagerVersion;

    @WireVariable
    private String foundationVersion;
    
    @WireVariable
    private String zkVersion;
    
    @WireVariable
    private String zkSpringVersion;

    /**
     * Cell Manager version.
     * @return version number
     */
    public String getCellManagerVersion() {
        return cellManagerVersion;
    }

    /**
     * WSGC foundation library version.
     * @return version number
     */
    public String getFoundationVersion() { 
        return foundationVersion;
    }
    
    /**
     * ZK version.
     * @return version number
     */
    public String getZkVersion() {
        return zkVersion;
    }

    /**
     * ZK-Spring version.
     * @return version number
     */
    public String getZkSpringVersion() {
        return zkSpringVersion;
    }
}
