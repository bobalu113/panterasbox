package com.panterasbox.ecommerce.search.cellmanager.caching;

import java.util.Map;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import net.sf.ehcache.Element;

import com.wsgc.ecommerce.caching.AbstractElementFactory;
import com.wsgc.ecommerce.context.request.model.RequestContext;
import com.panterasbox.ecommerce.search.cellmanager.model.CellManagerException;
import com.panterasbox.ecommerce.search.cellmanager.model.SiteAndInstallation;
import com.panterasbox.ecommerce.search.cellmanager.service.impl.EcomGenerationQueryDAO;

/**
 * This class runs a map of subsystem name to active generation for a specified
 * site and installation.  All known subsystems for the site will be represented
 * in the map.  The returned element object will be kept in cache for future use.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class ActiveEcomGenerationsElementFactory extends AbstractElementFactory<SiteAndInstallation, Map<String, Long>> {
    private Logger log = LoggerFactory.getLogger(getClass());

    private EcomGenerationQueryDAO ecomGenerationQueryDAO;

    @Override
    public Element createElement(long ttl, RequestContext context, SiteAndInstallation key) {
        log.debug("refresh ecom generations cache");
        Map<String, Long> generationMap = ecomGenerationQueryDAO.getGenerations(key.getSite(), key.getInstallation());
        if (generationMap == null) {
            throw new CellManagerException("Unable to get active ecom generations.");
        }
        return new Element(key, generationMap);
    }

    /**
     * Set the ecom generation query DAO (injected by Spring).
     * @param ecomGenerationQueryDAO the DAO to set
     */
    public void setEcomGenerationQueryDAO(EcomGenerationQueryDAO ecomGenerationQueryDAO) {
        this.ecomGenerationQueryDAO = ecomGenerationQueryDAO;
    }

}
