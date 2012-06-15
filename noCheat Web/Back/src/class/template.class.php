<?php

/*
 * Template class
 * 
 * Handles template information and is used by the core.template mod
 */

class n_template {
    private $_filename;
    private $_data;
    private $_ldelim = N_TEMPLATE_LDELIM;
    private $_rdelim = N_TEMPLATE_RDELIM;
    
    /**
     * Constructs the template object
     * @param filename The FULL PATH of the template file to be used
     */
    public function __construct($filename) {
        // Store
        $this->_filename = $filename;
        
        // Check if it exists
        if(!file_exists($this->_filename)) {
            // Warn
            trigger_error("Could not find template file: {$filename}", E_USER_WARNING);
        }
        
        // Get file contents
        $this->_data = file_get_contents($filename);
    }
    
    /**
     * Replaces a template tag with a string
     * @param tag The tag to replace
     * @param string The string to replace the tag with
     */
    public function replace_tag($tag, $string) {
        // Format tag
        $ftag = $this->_get_tag($tag);
        
        // Check that the tag exists
        if(stripos($this->_data, $ftag) === false) {
            // Notice and return
            trigger_error("Tag '{$ftag}' not found in template '" . $this->_filename . "'.", E_USER_NOTICE);
            return;
        }
        
        // Replace!
        $this->_data = str_replace($ftag, $string, $this->_data);
    }
    
    /**
     * Returns the data that is currently being held in memory for the loaded template
     * @return The currently held template data
     */
    public function get_data() {
        // Return!
        return $this->_data;
    }
    
    /**
     * Sets the delimiters for tags
     * @param ldelim The left delimiter
     * @param rdelim The right delimiter
     */
    public function set_delims($ldelim, $rdelim) {
        // Store
        $this->_ldelim = $ldelim;
        $this->_rdelim = $rdelim;
    }
    
    /**
     * Populates a template with tag=>value pairs
     * @param arr The array of keys=>values with which to populate the template
     */
    public function populate($arr) {
        // Replace all
        foreach($arr as $tag=>$val) {
            // Replace
            $this->replace_tag($tag, $val);
        }
    }
    
    /**
     * Returns a tag with its delimiters
     * @param tag The tag name to format
     * @return The formatted tag (with delimiters)
     */
    private function _get_tag($tag) {
        return $this->_ldelim . $tag . $this->_rdelim;
    }
}

?>
