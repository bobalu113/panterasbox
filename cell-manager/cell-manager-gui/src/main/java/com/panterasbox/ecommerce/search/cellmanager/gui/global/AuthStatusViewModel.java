package com.panterasbox.ecommerce.search.cellmanager.gui.global;

import org.zkoss.bind.annotation.Command;
import org.zkoss.zk.ui.Execution;
import org.zkoss.zk.ui.Executions;
import org.zkoss.zul.Window;

import com.wsgc.ecommerce.context.request.model.RequestContext;
import com.panterasbox.ecommerce.search.cellmanager.gui.util.FoundationUtil;
import com.wsgc.ecommerce.security.utilities.SecurityUtility;

/**
 * ViewModel for displaying login status, username, and sign in/sign out links.
 * Note that this ViewModel is a little different in that the commands force a
 * full-page refresh, so no NotifyChange annonations are necessary.
 * 
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class AuthStatusViewModel {
    
    /** 
     * Check user's login status.
     * @return true if the user is signed in 
     */
    public boolean isSignedIn() {
        RequestContext rc = FoundationUtil.getRequestContext();
        return SecurityUtility.isLoggedIn(rc);
    }
    
    /**
     * Get username of currently signed in user.
     * @return username, or null if not signed in
     */
    public String getUsername() {
        RequestContext rc = FoundationUtil.getRequestContext();
        return SecurityUtility.getUserName(rc);
    }

    /**
     * Command to open the login dialog.  
     */
    @Command
    public void signIn() {
        Window window = (Window) Executions.createComponents("loginWindow.zul", null, null);
        window.doModal();
    }
    
    /** 
     * Command to sign the user out.
     */
    @Command
    public void signOut() {
        SecurityUtility.logout(FoundationUtil.getRequestContext());
        Execution current = Executions.getCurrent();
        current.sendRedirect(current.getScheme() + "://" + current.getServerName() + ":" + current.getServerPort());
    }
    
}
