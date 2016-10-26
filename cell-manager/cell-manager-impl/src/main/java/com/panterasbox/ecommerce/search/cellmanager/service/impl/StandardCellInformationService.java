package com.panterasbox.ecommerce.search.cellmanager.service.impl;

import java.beans.PropertyDescriptor;
import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;

import org.apache.commons.beanutils.PropertyUtils;
import org.apache.commons.io.FilenameUtils;

import com.wsgc.ecommerce.context.request.model.RequestContext;
import com.panterasbox.ecommerce.search.cellmanager.model.CellDetails;
import com.panterasbox.ecommerce.search.cellmanager.model.CellDetailsFactory;
import com.panterasbox.ecommerce.search.cellmanager.model.CellManagerException;
import com.panterasbox.ecommerce.search.cellmanager.model.DgraphDetails;
import com.panterasbox.ecommerce.search.cellmanager.model.SubsystemDetails;
import com.panterasbox.ecommerce.search.cellmanager.service.CellInformationService;
import com.panterasbox.ecommerce.search.cellmanager.util.BeanUtil;
import com.panterasbox.ecommerce.search.cellmanager.util.BeanUtil.Refresher;

/**
 * Standard implementation of cell information service.
 * This implementation reads info from cell's properties files.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class StandardCellInformationService implements CellInformationService {
    //private final Logger log = LoggerFactory.getLogger(StandardCellInformationService.class);
    
    private String cellRootPath;
    
    private String cellConfigPath;
    
    private String genConfigPath;
    
    private CellDetailsFactory cellDetailsFactory;
    
    private static final String SUBSYTEMS_PROP = "subsystems";

    private static final String DGRAPHS_PROP = "dgraphs";
    
    private static final Refresher<SubsystemDetails> SUBSYSTEM_REFRESHER = new Refresher<SubsystemDetails>() {
        public void refresh(SubsystemDetails staleSubsystem, SubsystemDetails freshSubsystem) 
            throws IllegalAccessException, InvocationTargetException, NoSuchMethodException {
            for (PropertyDescriptor descriptor : PropertyUtils.getPropertyDescriptors(staleSubsystem)) {
                String property = descriptor.getName();
                BeanUtil.refreshProperty(staleSubsystem, freshSubsystem, property);
            }
        }
    };
    
    private static final Refresher<DgraphDetails> DGRAPH_REFRESHER = new Refresher<DgraphDetails>() {
        public void refresh(DgraphDetails staleDgraph, DgraphDetails freshDgraph) 
            throws IllegalAccessException, InvocationTargetException, NoSuchMethodException {
            for (PropertyDescriptor descriptor : PropertyUtils.getPropertyDescriptors(staleDgraph)) {
                String property = descriptor.getName();
                if (SUBSYTEMS_PROP.equals(property)) {
                    BeanUtil.refreshList(staleDgraph.getSubsystems(), freshDgraph.getSubsystems(), SUBSYSTEM_REFRESHER);
                } else {
                    BeanUtil.refreshProperty(staleDgraph, freshDgraph, property);
                }
            }
        }
    };
    
    private static final Refresher<CellDetails> CELL_REFRESHER = new Refresher<CellDetails>() {
        public void refresh(CellDetails staleCell, CellDetails freshCell) 
            throws IllegalAccessException, InvocationTargetException, NoSuchMethodException {
            for (PropertyDescriptor descriptor : PropertyUtils.getPropertyDescriptors(staleCell)) {
                String property = descriptor.getName();
                if (DGRAPHS_PROP.equals(property)) {
                    BeanUtil.refreshList(staleCell.getDgraphs(), freshCell.getDgraphs(), DGRAPH_REFRESHER);
                } else {
                    BeanUtil.refreshProperty(staleCell, freshCell, property);
                }
            }
        }
    };

    
    /** {@inheritDoc} */
    @Override
    public List<String> getAvailableCells() {
        List<String> result = new ArrayList<String>();
        File cellRootFolder = new File(cellRootPath);
        File[] cellFolders = cellRootFolder.listFiles(new FileFilter() {
            @Override
            public boolean accept(File file) {
                return file.isDirectory() && !file.getName().startsWith(".");
            }
        });
        for (File cellFolder : cellFolders) {
            result.add(cellFolder.getName());
        }
        return result;
    }
    
    /** {@inheritDoc} */
    @Override
    public CellDetails getCellDetails(RequestContext rc, String cellName) {
        Properties cellConfig = new Properties();
        try {
            cellConfig.load(new FileInputStream(getConfigPropertiesPath(cellName, cellConfigPath)));
        } catch (FileNotFoundException e) {
            throw new CellManagerException("Couldn't find cellconfig.properties", e);
        } catch (IOException e) {
            throw new CellManagerException("Couldn't load cellconfig.properties", e);
        }

        Properties genConfig = new Properties();
        try {
            genConfig.load(new FileInputStream(getConfigPropertiesPath(cellName, genConfigPath)));
        } catch (FileNotFoundException e) {
            throw new CellManagerException("Couldn't find genconfig.properties", e);
        } catch (IOException e) {
            throw new CellManagerException("Couldn't load genconfig.properties", e);
        }
        return cellDetailsFactory.getCellDetails(rc, cellConfig, genConfig);
    }
    
    /**
     * Normalize the path to one of a cell's properties files.
     * @param cellName the name of the cell to look up
     * @param propertiesPath the relative path to the properties file from the cell root dir
     * @return the absolute path to the cell's properties file
     */
    private String getConfigPropertiesPath(String cellName, String propertiesPath) {
        StringBuilder cellPropertiesPath = new StringBuilder();
        cellPropertiesPath.append(FilenameUtils.normalizeNoEndSeparator(cellRootPath));
        cellPropertiesPath.append(File.separator);
        cellPropertiesPath.append(FilenameUtils.normalizeNoEndSeparator(cellName));
        cellPropertiesPath.append(File.separator);
        cellPropertiesPath.append(FilenameUtils.normalizeNoEndSeparator(propertiesPath));
        return cellPropertiesPath.toString();
    }
    
    /** {@inheritDoc} */
    @Override
    public void refreshCells(RequestContext rc, List<CellDetails> staleCells) {
        List<CellDetails> freshCells = new ArrayList<CellDetails>();
        for (String cellName : getAvailableCells()) {
            freshCells.add(getCellDetails(rc, cellName));
        }

        try {
            BeanUtil.refreshList(staleCells, freshCells, CELL_REFRESHER);
        } catch (IllegalAccessException e) {
            throw new CellManagerException("Couldn't refresh cell", e);
        } catch (InvocationTargetException e) {
            throw new CellManagerException("Couldn't refresh cell", e);
        } catch (NoSuchMethodException e) {
            throw new CellManagerException("Couldn't refresh cell", e);
        }
    }
    
    /** {@inheritDoc} */
    @Override
    public void refreshDgraphs(List<DgraphDetails> staleDgraphs, List<DgraphDetails> freshDgraphs) {
        try {
            BeanUtil.refreshList(staleDgraphs, freshDgraphs, DGRAPH_REFRESHER);
        } catch (IllegalAccessException e) {
            throw new CellManagerException("Couldn't refresh cell", e);
        } catch (InvocationTargetException e) {
            throw new CellManagerException("Couldn't refresh cell", e);
        } catch (NoSuchMethodException e) {
            throw new CellManagerException("Couldn't refresh cell", e);
        }
    }
    
    /**
     * Set the cell root path (injected by spring).
     * @param cellRootPath the root directory containing active cells
     */
    public void setCellRootPath(String cellRootPath) {
        this.cellRootPath = cellRootPath;
    }

    /**
     * Set the cellconfig path, relative to cell root (injected by spring).
     * @param cellConfigPath the root directory containing active cells
     */
    public void setCellConfigPath(String cellConfigPath) {
        this.cellConfigPath = cellConfigPath;
    }

    /**
     * Set the genconfig path, relative to cell root (injected by spring).
     * @param genConfigPath the root directory containing active cells
     */
    public void setGenConfigPath(String genConfigPath) {
        this.genConfigPath = genConfigPath;
    }
    
    /**
     * Set the CellDetailsFactory (injected by spring).
     * @param cellDetailsFactory the cellDetailsFactory
     */
    public void setCellDetailsFactory(CellDetailsFactory cellDetailsFactory) {
        this.cellDetailsFactory = cellDetailsFactory;
    }
}
