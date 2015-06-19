<?hh
include "hidden/MaudeDbConfig.php";
/*
  The MDR Report Key is not always unique in the foitext file.  The Text Key, the second column, is unique. Since it isn't really clear why there
  occasionally (and very infrequency) are extra, additional reports, I decided to just take the first report, where the text key has the lowest value.
  The query for this is:
*/ 

 $MaudeDbConfig = MaudeDbConfig::getDbConfig(); 

$delete_stmt = <<<EOD
delete foitext
   from foitext
  inner join (
     select min(id) as firstId, mdr_report_key
       from foitext
      group by mdr_report_key
     having count(*) > 1) duplic on duplic.mdr_report_key = foitext.mdr_report_key
  where foitext.id > duplic.firstId;
EOD;

try {

        
    $LogFile = new \SplFileObject("fotext-delete-log.txt", "w");
    $LogFile->setFlags(SplFileObject::READ_AHEAD | SplFileObject::SKIP_EMPTY);
  
    $dbh = new PDO("mysql:host=localhost;dbname=" . $MaudeDbConfig['dbname'], $MaudeDbConfig['dbuser'], $MaudeDbConfig['passwd']);  
   
    $dbh->setAttribute( PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION );  
   
    $count = $dbh->exec($delete_stmt);
   
    // Return number of rows that were deleted.
    echo "Deleted $count rows.\n"; 
   
} catch(PDOException $e) {
       
     $errors = "Rolling back....\n";
     $errors .= "PDO Error: " . $e->getMessage() . "\n";  
     echo $errors;
     $LogFile->fwrite($errors);
      
} catch(Exception $e) {
       
     $errors = "Exception Thrown in " . $e->getFile() . " at line " . $e->getLine() . "\n";
     $errors .= "Stack Trace as string:\n" . $e->getTraceAsString() . "\n";
     $errors .= "Error message is: " .   $e->getMessage() . "\n";
     echo $errors;
     $LogFile->fwrite($errors);
     return;
}
?>
