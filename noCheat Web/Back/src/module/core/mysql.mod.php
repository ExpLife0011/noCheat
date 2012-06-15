<?php

/*
 * MySQL Mod
 * 
 * Handles MySQL operations
 * 
 * core.mysql
 */

final class core_mysql {
    private $_con;
    
    public function __construct() {
        // Include configuration
        require_once("conf/mysql.conf.php");
        
        // Attempt to connect
        $this->_con = new mysqli(N_MYSQL_HOST,
                            N_MYSQL_USER,
                            N_MYSQL_PASS);
        
        // Check for error
        if($this->_con->errno != 0) {
            trigger_error ("Could not connect to the database. Error : " . $this->_con->connect_error, E_USER_WARNING);
            $this->_con->errno = 0;
        }
        
        $this->_con->set_charset("utf8");
    }
    
    public function my_query($db, $query) {
        // Switch if not null
        if($db != null) $this->_con->select_db($db);
        
        // Set names
        $this->_con->query("SET NAMES 'utf8'");
        
        // Check for error
        if($this->_con->errno != 0) {
            trigger_error ("Could not switch to database '{$db}'. Error number: " . $this->_con->errno, E_USER_WARNING);
            $this->_con->errno = 0;
            return null;
        }
        
        // Execute query
        if(!($res = $this->_con->query($query))) {
            trigger_error("MySQL query failed: " . $query, E_USER_WARNING);
            return null;
        }
        
        // Return result
        return $res;
    }
}
?>
