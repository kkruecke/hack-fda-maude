<?hh
include "hidden/MaudeDbConfig.php";
/*
 * Delete the rows of mdrfoi where the mdr_report_key is not in foi_device and delete them: 

  DELETE `mdrfoi`
    FROM `mdrfoi`
    LEFT JOIN `foi_device` ON `mdrfoi`.`mdr_report_key` = `foi_device`.`mdr_report_key` 
    WHERE `foi_device`.`id` IS  NULL; 
 */
    $MaudeDbConfig = MaudeDbConfig::getDbConfig();
  
try {
    
    $LogFile = new \SplFileObject("mdrfoi-delete-log.txt", "w");
    $LogFile->setFlags(SplFileObject::READ_AHEAD | SplFileObject::SKIP_EMPTY);

       
    $dbh = new PDO("mysql:host=localhost;dbname=" . $MaudeDbConfig['dbname'], $MaudeDbConfig['dbuser'], $MaudeDbConfig['passwd']);  

    $dbh->setAttribute( PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION );  

    $delete_stmt = "DELETE `mdrfoi` FROM `mdrfoi` LEFT JOIN `foi_device` ON `mdrfoi`.`mdr_report_key` = `foi_device`.`mdr_report_key` 
              WHERE `foi_device`.`id` IS  NULL"; 

    $dbh->beginTransaction();
   
    // Do delete query 
    $bRc = $dbh->query($delete_stmt);
       
    $dbh->commit();
    $dbh = null; // closes db connection

}  catch(PDOException $e) {  // catch any PDO exceptions first

    $dbh->rollBack();
    $errors =  "Rolling back....\n";
 
    $errors .= "PDO Error: " . $e->getMessage() . "\n";  
    echo $errors;
    $LogFile->fwrite($errors);
}
?>
