package com.panterasbox.ecommerce.search.cellmanager.gui.dashboard;

import java.util.ArrayList;
import java.util.List;

import org.zkoss.bind.annotation.BindingParam;
import org.zkoss.bind.annotation.Command;
import org.zkoss.bind.annotation.DependsOn;
import org.zkoss.bind.annotation.Init;
import org.zkoss.bind.annotation.NotifyChange;
import org.zkoss.zk.ui.Executions;
import org.zkoss.zk.ui.select.annotation.VariableResolver;
import org.zkoss.zk.ui.select.annotation.WireVariable;

import com.wsgc.ecommerce.context.request.model.RequestContext;
import com.panterasbox.ecommerce.search.cellmanager.gui.util.FoundationUtil;
import com.panterasbox.ecommerce.search.cellmanager.model.CellDetails;
import com.panterasbox.ecommerce.search.cellmanager.service.CellInformationService;

/**
 * ViewModel to control the cell summary listbox on the dashboard.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
@VariableResolver(org.zkoss.zkplus.spring.DelegatingVariableResolver.class)
public class CellSummaryViewModel {
    //private Logger log = LoggerFactory.getLogger(getClass());
    
    private List<CellDetails> cells;
    
    private CellDetails selectedCell;
    
    @WireVariable
    private CellInformationService cellInformationService;
    
    /**
     * Initialize the cell list with all known cells from cell
     * directory.
     */
    @Init
    public void init() {
        RequestContext rc = FoundationUtil.getRequestContext();
        cells = new ArrayList<CellDetails>();
        for (String cell : cellInformationService.getAvailableCells()) {
            cells.add(cellInformationService.getCellDetails(rc, cell));
        }
    }

    /**
     * Get the list of known cells.
     * @return the cell list
     */
    public List<CellDetails> getCells() {
        return cells;
    }
    
    /**
     * Get the CellDetails instance for the currently selected cell.
     * @return the currently selected cell, or null if no cell is selected
     */
    public CellDetails getSelectedCell() {
        return selectedCell;
    }

    /**
     * Set the CellDetails instance for the currently selected cell.
     * @param selectedCell the currently selected cell, or null if no cell is selected
     */
    public void setSelectedCell(CellDetails selectedCell) {
        this.selectedCell = selectedCell;
    }
    
    /**
     * Get the caption label for the Dgraph Status groupbox.
     * @return the string to use as the label
     */
    @DependsOn("selectedCell")
    public String getDgraphStatusLabel() {
        if (selectedCell == null) {
            return "Dgraph Status";
        } else {
            return "Dgraph Status - " + selectedCell.getName();
        }
    }
    
    /**
     * Perform refresh of CellDetails data.
     */
    @Command
    @NotifyChange("cells")
    public void refreshCells() {
        RequestContext rc = FoundationUtil.getRequestContext();
        cellInformationService.refreshCells(rc, cells);
    }
    
}
