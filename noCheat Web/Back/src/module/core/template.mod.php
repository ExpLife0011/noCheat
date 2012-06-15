<?php

/*
 * Template handling
 * 
 * Handles templates and page layouts
 * 
 * This class is relatively simple, since it returns an instance of a template 
 * (instead of the old central-stored templates, which seemed clumbsy to me)
 * 
 * core.template
 */

final class core_template {
    public function __construct() {
        // Include configuration
        require_once("conf/template.conf.php");
        
        // Include template class
        require_once("class/template.class.php");
    }
    
    public function template_load($filename, $append = true) {
        // Build path if needed
        if($append) $filename = N_TEMPLATE_DIR . (substr($filename, 0, 1) == "/" ? "" : "/") . $filename;
        
        // Check that the file exists
        if(!file_exists($filename)) {
            // Warn and return null
            trigger_error("Could not find template file: {$filename} (Append? " . ($append ? "YES" : "NO") . ")", E_USER_WARNING);
            return null;
        }
        
        // Return new object
        return new n_template($filename);
    }
}


?>
