<?php
/*
 * Natoga Framework PHPDOCS
 * 
 * This script IS NOT UPLOADED, and contains only the PHPDocs for each
 * module for the site.
 * 
 * Ref:
 * https://sourceforge.net/p/natoga/home/Uploading_Process
 */

# Natoga phpdocs
final class n {    
    /**
     * Returns the current locale tag
     * @return string Local in loc-reg format (or just loc if no reg is found)
     */
    public static function locale_get();
    
    /**
     * Queries the server on a specific database
     * @param db The database to user
     * @param query The MySQL query to execute
     * @return mysqli_result The MySQL query result
     */
    public static function my_query($db, $query);
    
    /**
     * Get's a string according to the visitor's locale
     * @param name The name (tag)'d string to retrieve
     */
    public static function get_string($name);
    
    /**
     * Creates and returns a new template instance
     * @param filename The filename to load
     * @param append Whether or not to append the filename ($filename) onto the end of the N_TEMPLATE_PATH constant
     * @return n_template A template object
     */
    public static function template_load($filename, $append = true);
    
    /**
     * Registers a user within the Natoga system
     * @param email The email of the user account
     * @param pass_pt The plain-text password to register the account with
     * @return int A Registration return code
     */
    public static function user_register($email, $pass_pt);
    
    /**
     * Hashes a password
     * @param pass The password to hash
     * @param usalt The user salt to hash with
     * @return string The hash'd password
     */
    public static function hash_pass($pass, $usalt);
    
    /**
     * Authorizes a user (checks an email and a password)
     * @param email The email to authenticate
     * @param pass_pt The password to authenticate
     * @return int An integer: positives = success [user id], negative = failure [N_AUTH_*]
     */
    public static function auth_user($email, $pass_pt);
}

?>