#!/usr/bin/php
<?php
use Maude\Registry as Registry;

require_once("class_loader.php");

boot_strap();

try {
    
   $db_settings = Registry::registry('database');
   
   $db_handle = new \PDO("mysql:host=localhost;dbname=" . $db_settings['dbname'],
                         $db_settings['dbuser'],
                         $db_settings['dbpasswd']);  
   
  $db_handle->setAttribute( \PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION );  

  $file_handlers = Registry::registry('file_handlers'); 

  $matches = array();

  foreach(Registry::registry('file_names') as $file_type => $file_name) {
 
     // Take the substring between '/' and '.' from the $file_name to get the index into $file_handlers

    if (preg_match("@/([^.]+)@", $file_name, $matches) == 0) {

           new \Exception("File handler cannot be found\n");
    } 

    $className = "Maude\\" .  $file_handlers[$file_type]; 
        
    $databaseUpdater = new $className($file_name, $db_handle);
  
    $databaseUpdater->updateDatabase(); 

    echo "\nupdateDatabase() complete\n"; // debug
  }

} catch (Exception $e) {

   $errors = "\nException Thrown in " . $e->getFile() . " at line " . $e->getLine() . "\n";
      
   $errors .= "Stack Trace as string:\n" . $e->getTraceAsString() . "\n";
          
   $errors .= "Error message is: " .   $e->getMessage() . "\n";
      
   echo $errors;

} // end try

