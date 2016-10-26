package com.panterasbox.ecommerce.search.cellmanager.model;


/**
 * Generic runtime exception for cell manager.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class CellManagerException extends RuntimeException {
    private static final long serialVersionUID = 1L;

    /**
     * Constructor without chained exception.
     * @param msg a helpful message
     */
    public CellManagerException(String msg) {
        super(msg);
    }

    /**
     * Constructor with chained exception.
     * @param msg a helpful message
     * @param cause original exception
     */
    public CellManagerException(String msg, Exception cause) {
        super(msg, cause);
    }

}
