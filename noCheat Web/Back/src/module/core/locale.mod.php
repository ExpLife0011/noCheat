<?php

/*
 * Local mod
 * 
 * Gets strings based on the user's locale
 * 
 * core.locale
 */

final class core_locale {
    private $_loc;
    private $_reg;
    
    public function __construct() {
        // Update our locale
        $this->_updateLocale();
        
        // Update cookie (2 years)
        setcookie("lang", $this->locale_get(), time() + (3600 * 24 * 265 * 2));
        
        // Include MySQL
        n::ml("core.mysql");
    }
    
    public function locale_get() {
        // Check if we have a region
        if($this->_reg == null)
                return $this->_loc;
        else
                return $this->_loc . "-" . $this->_reg;
    }
    
    public function get_string($name) {
        // Build query
        $l = strtolower($this->_loc);
        $r = ($this->_reg == null ? "NULL" : "'" . strtolower($this->_reg) . "'");
        $q = "SELECT locale_get_string('{$name}',  '{$l}',  {$r}) AS  'lstr'";
        
        // Query
        $res = n::my_query(N_DB_LOCALE, $q)->fetch_assoc();
        return ($res['lstr'] == "" ? null : $res['lstr']);
    }
    
    /**
     * Updates the locale in order:
     * 
     * - URL GET var
     * - Cookie
     * - SERVER Var
     * - Default to English
     */
    private function _updateLocale() {
        // Check URL
        if(isset($_GET['lang'])) {
            // Validate lang
            $l = locale_get_primary_language($_GET['lang']);
            $r = locale_get_region($_GET['lang']);
            if($l != null) {
                // Check region/store
                if($r != null) $this->_reg = $r;
                
                // Store and return
                $this->_loc = $l;
                return;
            }
        }
        
        // Check cookie
        if(isset($_COOKIE['lang'])) {
            // Validate lang
            $l = locale_get_primary_language($_COOKIE['lang']);
            $r = locale_get_region($_COOKIE['lang']);
            if($l != null) {
                // Check region/store
                if($r != null) $this->_reg = $r;
                
                // Store and return
                $this->_loc = $l;
                return;
            }
        }
        
        // Check server
        /*if(isset($_SERVER[HTTP_ACCEPT_LANGUAGE])) {
            // Validate lang
            $l = locale_get_primary_language($_SERVER[HTTP_ACCEPT_LANGUAGE]);
            $r = locale_get_region($_SERVER[HTTP_ACCEPT_LANGUAGE]);
            if($l != null) {
                // Check region/store
                if($r != null) $this->_reg = $r;
                
                // Store and return
                $this->_loc = $l;
                return;
            }
        }*/
        
        // Default :P
        $this->_loc = "en";
        $this->_reg = "us";
    }
}

?>
