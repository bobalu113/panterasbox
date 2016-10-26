package com.panterasbox.ecommerce.search.cellmanager.gui.util;

import org.zkoss.zk.ui.Executions;

import com.wsgc.ecommerce.context.request.model.RequestContext;

/**
 * Some utility methods to interact with WSGC foundation.
 * 
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public final class FoundationUtil {
    
    /**
     * Utility class shouldn't be instantiated.
     */
    private FoundationUtil() { 
        throw new AssertionError();
    }
    
    /**
     * Get the request context from the request attributes.
     * @return the request context
     */
    public static RequestContext getRequestContext() {
        return (RequestContext) Executions.getCurrent().getAttribute(RequestContext.KEY);
    }

}
