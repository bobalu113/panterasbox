package com.panterasbox.ecommerce.search.cellmanager.gui.global;

import java.util.ArrayList;
import java.util.List;

import org.zkoss.bind.annotation.BindingParam;
import org.zkoss.bind.annotation.Command;
import org.zkoss.bind.annotation.GlobalCommand;
import org.zkoss.bind.annotation.Init;
import org.zkoss.bind.annotation.NotifyChange;
import org.zkoss.zk.ui.Executions;

import com.panterasbox.ecommerce.search.cellmanager.model.CellDetails;
import com.panterasbox.ecommerce.search.cellmanager.model.DgraphDetails;
import com.panterasbox.ecommerce.search.cellmanager.model.TabType;
import com.panterasbox.ecommerce.search.cellmanager.model.WorkspaceTab;

/**
 * ViewModel for handing the top-level workspace area.
 * 
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class WorkspaceViewModel {

    private List<WorkspaceTab> tabs;
    
    private WorkspaceTab selectedTab;
    
    /**
     * Initialize the workspace.  A dashboard tab is displayed by default.
     */
    @Init
    public void init() {
        WorkspaceTab newDashboardTab = new WorkspaceTab(TabType.DASHBOARD);
        tabs = new ArrayList<WorkspaceTab>();
        tabs.add(newDashboardTab);
        selectedTab = newDashboardTab;
    }
    
    /**
     * Get the list of open tabs.
     * @return list of tabs
     */
    public List<WorkspaceTab> getTabs() {
        return tabs;
    }
    
    /**
     * Set the list of open tabs.
     * @param tabs the list of tabs
     */
    public void setTabs(List<WorkspaceTab> tabs) {
        this.tabs = tabs;
    }
    
    /**
     * Get the selected tab.
     * @return the selected tab
     */
    public WorkspaceTab getSelectedTab() {
        return selectedTab;
    }
    
    /**
     * Set the selected tab.
     * @param tab the selected tab
     */
    public void setSelectedTab(WorkspaceTab tab) {
        selectedTab = tab;
    }
    
    /**
     * Select the dashboard tab, or create a new one if necessary.
     */
    @GlobalCommand
    @NotifyChange({"tabs", "selectedTab"})
    public void viewDashboard() {
        WorkspaceTab dashboardTab = null;
        
        for (WorkspaceTab tab : tabs) {
            if (tab.getType() == TabType.DASHBOARD) {
                dashboardTab = tab;
                break;
            }
        }
        
        if (dashboardTab == null) {
            dashboardTab = new WorkspaceTab(TabType.DASHBOARD);
            tabs.add(dashboardTab);
        }

        setSelectedTab(dashboardTab);
    }
    
    /**
     * Navigate to a cell's workbench app in new tab.
     * @param cell the workbench enabled cell
     */
    @GlobalCommand
    @NotifyChange({"tabs", "selectedTab"})
    public void viewWorkbench(@BindingParam("cell") CellDetails cell) {
        throw new UnsupportedOperationException("workbench button not yet implemented");
//        if (!cell.isWorkbenchEnabled()) {
//            throw new IllegalArgumentException("Cell " + cell.getName() + " not workbench enabled");
//        }
//        Executions.getCurrent().sendRedirect("http://" + cell.getWorkbenchHost() + ":" + cell.getWorkbenchPort(), "_blank");
    }
    
    /**
     * Navigate to the Reference application for the dgraph in a new tab.
     * @param dgraph the dgraph for which to open reference app
     */
    @GlobalCommand
    @NotifyChange({"tabs", "selectedTab"})
    public void viewReferenceApp(@BindingParam("dgraph") DgraphDetails dgraph) {
        throw new UnsupportedOperationException("reference app button not yet implemented");
    }
        
    /**
     * Navigate to the Dgraph statistics screen in a new tab.
     * @param dgraph the dgraph for which to view stats
     */
    @GlobalCommand
    @NotifyChange({"tabs", "selectedTab"})
    public void viewDgraphStats(@BindingParam("dgraph") DgraphDetails dgraph) {
        throw new UnsupportedOperationException("dgraph stats button not yet implemented");
    }

    /**
     * Restart the Dgraph by making an HTTP request to its admin page,
     * and report the result.
     * @param dgraph the dgraph to restart
     */
    @GlobalCommand
    @NotifyChange({"tabs", "selectedTab"})
    public void doRestartDgraph(@BindingParam("dgraph") DgraphDetails dgraph) {
        throw new UnsupportedOperationException("restart dgraph button not yet implemented");
    }


}
