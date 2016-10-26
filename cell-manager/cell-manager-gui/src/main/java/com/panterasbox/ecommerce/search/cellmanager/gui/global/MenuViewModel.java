package com.panterasbox.ecommerce.search.cellmanager.gui.global;

import org.zkoss.bind.annotation.Command;
import org.zkoss.zk.ui.Executions;
import org.zkoss.zul.Window;

/**
 * ViewModel for handing the main menu bar.
 * 
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class MenuViewModel {

    /**
     * Display the version summary window.
     */
    @Command
    public void helpSummary() {
        Window window = (Window) Executions.createComponents("summaryWindow.zul", null, null);
        window.doModal();
    }
}
