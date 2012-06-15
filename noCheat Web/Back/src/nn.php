<?php

/*
 * Natoga Framework
 * 
 * This script starts up the Natoga Framework.
 * 
 * Please ref:
 * https://sourceforge.net/p/natoga/home/Uploading_Process
 */

# Call Base Init
require_once("base_init.php");

# Main Natoga Definition
final class n {
    
    // ['function'] = 'module';
    private static $_funcs = array();
    private static $_mods = array();
    
    /**
     * Loads one or more modules
     * @params Strings, each being a module to load
     */
    public static function ml() {
        // Cycle through args
        foreach(func_get_args() as $arg) {
            // Check for string
            if(!is_string($arg)) {
                // Trigger error and continue
                trigger_error("Module argument is not a string.", E_USER_WARNING);
                continue;
            }
            
            // Lowercase it
            $arg = strtolower($arg);
            
            // Check to see if we've already loaded it
            if(in_array($arg, n::$_mods)) continue;
            
            // Convert module to path
            $apth = N_MOD_PATH . str_replace(".", "/", $arg) . ".mod.php";
            
            // Check that it exists
            if(!file_exists($apth)) {
                // Trigger error and continue
                trigger_error("Module " . $arg . " doesn't exist!", E_USER_WARNING);
                continue;
            }
            
            // Include
            @include_once($apth);
            
            // Get class name
            $nm = str_replace(".", "_", $arg);
            
            // Load function list
            $fncs = get_class_methods($nm);
            
            // Check for error
            if($fncs == null) {
                // Trigger and continue
                trigger_error("Could not load class methods for " . $arg . ".", E_USER_WARNING);
                continue;
            }
            
            // Get instance
            $ind = new $nm();
            
            // Enum methods
            foreach($fncs as $fnc) {
                // Check that it isn't a built-in or ignored method
                if(substr($fnc, 0, 1) == "_") continue;
                
                // Store!
                n::$_funcs[$fnc] = $ind;
            }
            
            // Store that we've loaded a module
            array_push(n::$_mods, $arg);
        }
    }
        
    public static function __callStatic($name, $args) {
        // Check that the function exists
        if(!key_exists($name, n::$_funcs)) {
            // Trigger and return
            trigger_error("Function \"" . $name . "\" not registered!", E_USER_WARNING);
            return;
        }
        
        // Call (hell of arguments here)
	// NOTE: Expanded due to static calls being much faster than
	//        call_user_func_array!
        switch(count($args)) {
            case 0: return n::$_funcs[$name]->$name(); break;
            case 1: return n::$_funcs[$name]->$name($args[0]); break;
            case 2: return n::$_funcs[$name]->$name($args[0],$args[1]); break;
            case 3: return n::$_funcs[$name]->$name($args[0],$args[1],$args[2]); break;
            case 4: return n::$_funcs[$name]->$name($args[0],$args[1],$args[2],$args[3]); break;
            case 5: return n::$_funcs[$name]->$name($args[0],$args[1],$args[2],$args[3],$args[4]); break;
            case 6: return n::$_funcs[$name]->$name($args[0],$args[1],$args[2],$args[3],$args[4],$args[5]); break;
            default: return call_user_func_array(array($_mods[n::$_funcs[$name]], $name), $args);
        }
    }
}

?>
