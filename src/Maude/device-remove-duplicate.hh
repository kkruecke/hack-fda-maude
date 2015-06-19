<?hh
/*
 Save only unique mdr_report_keys. Get rid of the device code. 
  */

include "hidden/MaudeDbConfig.php";

    $MaudeDbConfig = MaudeDbConfig::getDbConfig(); 
    
try {
       
    $dbh = new PDO("mysql:host=localhost;dbname=" . $MaudeDbConfig['dbname'], $MaudeDbConfig['dbuser'], $MaudeDbConfig['passwd']);  

    $dbh->setAttribute( PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION );  
    
    $insert_stmt = $dbh->prepare("INSERT INTO foi_device_nodups(mdr_report_key) values (:mdr_report_key)");

    $mdr_report_key = (int) 0;

    $insert_stmt = $dbh->prepare("INSERT INTO foi_device_nodups(mdr_report_key) values (:mdr_report_key)");
    
    $insert_stmt->bindParam(':mdr_report_key', $mdr_report_key, PDO::PARAM_INT);

    $sql = "SELECT distinct mdr_report_key FROM foi_device";

    $dbh->beginTransaction();
    $rec_count = 1;

    foreach ($dbh->query($sql) as $results) {

        $mdr_report_key = $results['mdr_report_key'];

        $count = $insert_stmt->execute(); 

        if ($count == 0) {

           throw new Exception("could not insert mdr_report_key of $mdr_report_key\n");
        }
        $rec_count++;
    }     
    
    $dbh->commit();

    $dbh = null; // closes db connection
    echo "$insert_count records were added to foi_device_dup. $txt_line_count records were read.\n";

}  catch(PDOException $e) {  // catch any PDO exceptions first

    $dbh->rollBack();

    $errors = "Rolling back....\n";
 
    $errors .= "PDO Error: " . $e->getMessage() . "\n";  

    $errors .= "mdr_report_key is $mdr_report_key\n";
        
    echo $errors;
   
}  catch(Exception $e) {
    
    $errors = "Non_PDO Error message: " .   $e->getMessage() . "\n";

    echo $errors;
}
?>
