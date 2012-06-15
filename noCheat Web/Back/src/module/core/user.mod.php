<?php
/**
 * User mod
 * 
 * Handles user functions (registration, login, auth, etc.)
 * 
 * core.user
 */

final class core_user {
    
    public function __construct() {
        // Get conf
        require_once("conf/user.conf.php");
    }
    
    public function user_register($email, $pass_pt) {
        // TODO: Implement
        
        // Return Success
        return N_REG_SUCCESS;
    }
    
    public function hash_pass($pass, $usalt) {
        $k1 = $pass . $pass;
        $r = hash("haval192,4", $k1);
        $r1 = $r;
        $k2 = $usalt . $r . $usalt;
        $r = hash("snefru", $k2);
        $k3 = N_GLOB_PASSHASH . $r . $usalt . $pass;
        $r = hash("sha512", $k3);
        $k4 = N_GLOB_PASSHASH . $r . $pass . $k2 . $r1 . $usalt . $k1 . $k3;
        $r = hash("whirlpool", $k4);
        return $r;
    }
    
    public function auth_user($email, $pass_pt) {
        // Setup MySQL
        n::ml("core.mysql");
        
        // Get JSON
        $a = array("email"=>$email);
        $ajs = json_encode($a);
        
        // Get user salt
        $q = "SELECT `usalt`, `id` FROM `id`,`account` WHERE `id`.`authaccount` IN ( SELECT `account_id` FROM `account` WHERE `value`='{$ajs}' );";
        $res = n::my_query(N_DB_ID, $q);
        if($res->num_rows == 0) {
            // Return Failed
            return N_AUTH_NO_EMAIL;
        }
        $ass = $res->fetch_assoc();
        
        // Store usalt
        $usalt = $ass['usalt'];
        
        // Store ID
        $id = $ass['id'];
        
        // Hash pass
        $pass = $this->hash_pass($pass_pt, $usalt);
        
        // Check password
        $q = "SELECT `id` FROM `id` WHERE `id`={$id} AND `pass`='{$pass}'";
        $res = n::my_query(N_DB_ID, $q);
        
        // Check
        if($res->num_rows == 0) {
            // Return bad pass
            return N_AUTH_BAD_PASS;
        }
        
        // Return Success
        return intval(id);
    }
}

?>
