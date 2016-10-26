package com.panterasbox.ecommerce.search.cellmanager.model;

/**
 * Enum of all possible cell states.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public enum CellStatus {
    /** Cell is up to date and active */
    ACTIVE,
    /** Cell is currently being indexed */
    ACTIVATING,
    /** Cell indexing has failed */
    FAILED
}
