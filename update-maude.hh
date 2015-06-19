<?php
require_once("loader/SplClassLoader.php");

function boot_strap() 
{
 date_default_timezone_set("America/Chicago"); // Workaround for PHP5 

 $spl_loader = new SplClassLoader('Maude', 'src');

 $spl_loader->register();
}
