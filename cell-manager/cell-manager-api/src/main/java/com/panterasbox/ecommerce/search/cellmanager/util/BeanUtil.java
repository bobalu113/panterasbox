package com.panterasbox.ecommerce.search.cellmanager.util;

import java.lang.reflect.InvocationTargetException;
import java.util.List;

import org.apache.commons.beanutils.PropertyUtils;

/**
 * Some utility methods for working with beans and their constituent
 * properties.
 * 
 * @author Bobby Schaetzle
 * @version $Id: $
 */
public final class BeanUtil {

    /**
     * Utility class shouldn't be instantiated.
     */
    private BeanUtil() { 
        throw new AssertionError();
    }
    
    /**
     * Refresh a scalar bean property with a new value.  This will preserve the original
     * reference in the staleBean if the freshBean has no changes.
     * 
     * @param staleBean the bean to refresh
     * @param freshBean the bean with the refreshed values
     * @param property the property to refresh
     * @param <T> the type of bean to update
     * @throws IllegalAccessException see {@link PropertyUtils#setProperty(Object, String, Object)}
     * @throws InvocationTargetException see {@link PropertyUtils#setProperty(Object, String, Object)}
     * @throws NoSuchMethodException see {@link PropertyUtils#setProperty(Object, String, Object)}
     */
    public static <T> void refreshProperty(T staleBean, T freshBean, String property) 
            throws IllegalAccessException, InvocationTargetException, NoSuchMethodException {
        Object staleValue = PropertyUtils.getProperty(staleBean, property);
        Object freshValue = PropertyUtils.getProperty(freshBean, property);
        if (staleValue == null ? freshValue != null : !staleValue.equals(freshValue)) {
            PropertyUtils.setProperty(staleBean, property, freshValue);
        }
    }
    
    /**
     * Refresh a list bean property with new elements.  This will preserve original references
     * in the staleList if the value has not changed in freshList (i.e. the individual elements
     * will be modified in-place).  If freshList contains more elements than staleList, they
     * will be appended; if it contains few elements, staleList will be truncated to match.
     * 
     * @param staleList the list of beans to update
     * @param freshList the list of beans with refreshed values
     * @param refresher a {@link Refresher} instance to actually update bean properties
     * @param <T> the type of bean to update
     * @throws IllegalAccessException see {@link PropertyUtils#setProperty(Object, String, Object)}
     * @throws InvocationTargetException see {@link PropertyUtils#setProperty(Object, String, Object)}
     * @throws NoSuchMethodException see {@link PropertyUtils#setProperty(Object, String, Object)}
     */
    public static <T> void refreshList(List<T> staleList, List<T> freshList, Refresher<T> refresher) 
            throws IllegalAccessException, InvocationTargetException, NoSuchMethodException {
        int i = 0;
        for (; i < staleList.size() && i < freshList.size(); i++) {
            refresher.refresh(staleList.get(i), freshList.get(i));
        }
        for (; i < freshList.size(); i++) {
            staleList.add(freshList.get(i));
        }
        if (staleList.size() > i) {
            staleList.subList(i, staleList.size()).clear();
        }
    }

    /**
     * Simple interface for refreshing a bean's stale properties with property
     * values from a fresh instance.  This interface is used to encapsulate
     * logic for refreshing individual elements of a list of beans. 
     *
     * @param <T> the type of the bean to update
     * @see StandardCellInformatService#refreshList
     */
    public static interface Refresher<T> {
        
        /**
         * Update a stale instance from value of a fresh instance.
         * 
         * @param staleValue the value to update
         * @param freshValue the fresh value from which to update
         * @throws IllegalAccessException see {@link PropertyUtils#setProperty(Object, String, Object)}
         * @throws InvocationTargetException see {@link PropertyUtils#setProperty(Object, String, Object)}
         * @throws NoSuchMethodException see {@link PropertyUtils#setProperty(Object, String, Object)}
         */
        void refresh(T staleValue, T freshValue) 
                throws IllegalAccessException, InvocationTargetException, NoSuchMethodException;
    }
    

}
