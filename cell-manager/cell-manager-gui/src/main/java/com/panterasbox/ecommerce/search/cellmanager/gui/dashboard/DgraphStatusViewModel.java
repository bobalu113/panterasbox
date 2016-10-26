package com.panterasbox.ecommerce.search.cellmanager.gui.dashboard;

import java.util.ArrayList;
import java.util.List;

import org.zkoss.bind.annotation.BindingParam;
import org.zkoss.bind.annotation.Command;
import org.zkoss.bind.annotation.DependsOn;
import org.zkoss.bind.annotation.GlobalCommand;
import org.zkoss.bind.annotation.Init;
import org.zkoss.bind.annotation.NotifyChange;
import org.zkoss.zk.ui.select.annotation.VariableResolver;
import org.zkoss.zk.ui.select.annotation.WireVariable;

import com.wsgc.ecommerce.context.request.model.RequestContext;
import com.panterasbox.ecommerce.search.cellmanager.gui.util.FoundationUtil;
import com.panterasbox.ecommerce.search.cellmanager.model.CellDetails;
import com.panterasbox.ecommerce.search.cellmanager.model.DgraphDetails;
import com.panterasbox.ecommerce.search.cellmanager.service.CellInformationService;

/**
 * ViewModel to control the cell summary listbox on the dashboard.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
@VariableResolver(org.zkoss.zkplus.spring.DelegatingVariableResolver.class)
public class DgraphStatusViewModel {
    //private Logger log = LoggerFactory.getLogger(getClass());
    
    private CellDetails cell;
    
    private List<DgraphDetails> dgraphs;
    
    private DgraphDetails selectedDgraph;
        
    @WireVariable
    private CellInformationService cellInformationService;
    
    /**
     * Initialize the dgraph list (must be non-null).
     */
    @Init
    public void init() {
        dgraphs = new ArrayList<DgraphDetails>();
    }
    
    /**
     * The CellDetails instance for the cell whose dgraphs we are viewing.
     * @return the currently selected cell, or null if no cell is selected
     */
    public CellDetails getCell() {
        return cell;
    }

    /**
     * The currently selected dgraph.  This is used in conjunction with the
     * selectDgraph command to keep track of which accordion tab is selected.
     * @return the selected dgraph
     * @see selectDgraph
     */
    public DgraphDetails getSelectedDgraph() {
        return selectedDgraph;
    }
    
    /**
     * The list of Dgraphs configured for the current cell.
     * @return the dgraph list
     */
    public List<DgraphDetails> getDgraphs() {
        return dgraphs;
    }
    
    /**
     * Get the caption label for the Dgraph Status groupbox.
     * @return the string to use as the label
     */
    @DependsOn("cell")
    public String getDgraphStatusLabel() {
        if (cell == null) {
            return "Dgraph Status";
        } else {
            return "Dgraph Status - " + cell.getName();
        }
    }
    
    /**
     * Perform refresh of CellDetails data.
     */
    @Command
    @NotifyChange("dgraphs")
    public void refreshDgraphs() {
        if (cell != null) {
            RequestContext rc = FoundationUtil.getRequestContext();
            CellDetails freshCell = cellInformationService.getCellDetails(rc, cell.getName());
            cellInformationService.refreshDgraphs(dgraphs, freshCell.getDgraphs());
        } else {
            dgraphs.clear();
        }
    }

    /**
     * This command fires when a user selects a cell from the cell summary,
     * managed by {@link CellSummaryViewModel}.  This should refresh the current
     * cell and refresh the list of Dgraphs we are displaying.
     * @param selectedCell the newly selected cell
     */
    @GlobalCommand
    @NotifyChange({"cell", "dgraphs", "selectedDgraph"})
    public void dashboardSelectCell(@BindingParam("cell") CellDetails selectedCell) {
        cell = selectedCell;
        refreshDgraphs();
        if (dgraphs != null && !dgraphs.isEmpty()) {
            selectedDgraph = dgraphs.get(0);
        }
    }
    
    /**
     * A command to update the selected Dgraph, assigned to a dgraph tab's onSelect
     * attribute.  This is necessary because there is no selected item binding
     * on tabboxes.
     * @param dgraph the newly selected dgraph
     */
    @Command
    @NotifyChange("selectedDgraph")
    public void selectDgraph(@BindingParam("dgraph") DgraphDetails dgraph) {
        selectedDgraph = dgraph;
    }
}
