<?php
/*
 * Define the engine's constants
 */

# Debug Constants
define("N_D_NONE",                      0);
define("N_D_NOTICE",                    1);
define("N_D_WARN",                      2);
define("N_D_ALL",                       3);

# Define some basic constants (NATOGA.COM)
if(strstr($_SERVER['HTTP_HOST'], "nochea.tk") > -1) {
define("N_DEBUG",                       "N_D_NONE");
define("N_SRC_PATH",                    "/home/natogac1/domains/tk.nochea/src");
define("N_DOMAIN_PATH",                 "/home/natogac1/domains/tk.nochea");
define("N_DOMAINS",                     "/home/natogac1/domains");
define("N_BASE",                        "/home/natogac1");
}

# Define some basic constants (NATOGA.COM)
if(strstr($_SERVER['HTTP_HOST'], "natoga.com") > -1) {
define("N_DEBUG",                       "N_D_NONE");
define("N_SRC_PATH",                    "/home/natogac1/domains/com.natoga/src");
define("N_DOMAIN_PATH",                 "/home/natogac1/domains/com.natoga");
define("N_DOMAINS",                     "/home/natogac1/domains");
define("N_BASE",                        "/home/natogac1");
}

# Define some basic constants (LOCAL DEV)
if(strstr($_SERVER['HTTP_HOST'], "localhost") > -1) {
define("N_DEBUG",                       "N_D_ALL");
define("N_SRC_PATH",                    "C:/Users/Administrator/Documents/Natoga/UniServer/www/src");
}

# Module path
define("N_MOD_PATH",                    N_SRC_PATH . "/module/");   // WITH trailing slash

?>
