package com.panterasbox.ecommerce.search.cellmanager.gui.dashboard;

import org.zkoss.bind.BindContext;
import org.zkoss.bind.Converter;
import org.zkoss.zul.Tab;

import com.panterasbox.ecommerce.search.cellmanager.model.DgraphDetails;

/**
 * A simple converter which formats a dgraphs index, hostname,
 * and port for display purposes.
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public class DgraphConverter implements Converter<String, DgraphDetails, Tab> {

    /** {@inheritDoc} */
    @Override
    public String coerceToUi(DgraphDetails dgraph, Tab tab, BindContext ctx) {
        final int index = (Integer) ctx.getConverterArg("dgraphIndex");
        StringBuilder sb = new StringBuilder();
        sb.append(index + 1);
        sb.append(". ");
        sb.append(dgraph.getHostname());
        sb.append(":");
        sb.append(dgraph.getPort());
        return sb.toString();
    }

    /** {@inheritDoc} */
    @Override
    public DgraphDetails coerceToBean(String dgraphString, Tab tab, BindContext ctx) {
        // Not implemented as we currently only need one-way binding.
        return null;
    }

}
