<?hh
include "hidden/MaudeDbConfig.php";
/*
 * Purpose: Insert records into foi_device table whose schema is:
 * 
 CREATE TABLE IF NOT EXISTS `mdrfoi_distinct` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `mdr_report_key` int(11) unsigned NOT NULL,
  `report_source_code` CHAR(1) NOT NULL,
  `date_received` DATE NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;
 
 The variables used to insert values into the table are:  
  id 
  mdr_report_key      is in regex return value of $matches[1][0]
  report_source_code  is in regex return value of $matches[1][3]
  date_received       is in regex return value of $matches[1][7]. 
  
Note: date_reported can be empty, which stores 0000-00-00 in the database. So it isn't really of value. 
 */

try {

    $MaudeDbConfig = MaudeDbConfig::getDbConfig(); 
         
    $LogFile = new \SplFileObject("new-mdrfoi-distinct-log.txt", "w");
    $LogFile->setFlags(SplFileObject::READ_AHEAD | SplFileObject::SKIP_EMPTY);
  
    $dbh = new PDO("mysql:host=localhost;dbname=" . $MaudeDbConfig['dbname'], $MaudeDbConfig['dbuser'], $MaudeDbConfig['passwd']);  

    $dbh->setAttribute( PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION );  

    // Set to a default values for now, so bindParam can be called.
    $mdr_report_key = 0; 
    $report_source_code = "" ; 
    $date_received = "";    
    
    $insert_stmt = $dbh->prepare("INSERT INTO mdrfoi_distinct(mdr_report_key, report_source_code, date_received) values
                   (:mdr_report_key, :report_source_code, :date_received)");

    // bind the parameters in each statement
    $insert_stmt->bindParam(':mdr_report_key', $mdr_report_key, PDO::PARAM_INT);

    $insert_stmt->bindParam(':report_source_code', $report_source_code, PDO::PARAM_STR);
    
    $insert_stmt->bindParam(':date_received', $date_received, PDO::PARAM_STR);
    
    $dbh->beginTransaction();

    $insert_count = 0;   
    $select_string = "SELECT DISTINCT mdr_report_key, report_source_code, date_received FROM mdrfoi WHERE 1";
        
    foreach ( $dbh->query($select_string, PDO::FETCH_ASSOC) as $row) { 
    
      $mdr_report_key = $row['mdr_report_key'];  
      $report_source_code = $row['report_source_code'];  // <--This is missing in mdrfoi table
      $date_received = $row['date_received'];  
      
      ++$insert_count;          
                
      $insert_rc = $insert_stmt->execute();
           
    }  // end foreach
     
    $dbh->commit();
    /*    
    echo "Commiting $insert_count total inserts into mdrfoi_distinct \n";
    
     * Now that we have mdrfoi_distinct, with only the distinct records from mdrfoi, we will delete all the rows of 
     * mdrfoi, and insert all the rows of mdrfoi_distinct into it. Thus copy mdrfoi_distinct to mdrfoi. 
    
    $dbh->beginTransaction();

    $delete_string = "DELETE FROM MDRFOI WHERE 1";
    $delete_stmt = $dbh->query($select_string);
    $delete_count $delete_stmt->rowCount();
    if (!($delete_count > 0)) {
        throw new Exception("no rows were deleted t
    }

    $dbh->commit();
    */
    $dbh = null; // closes db connection

    echo "$insert_count records were added to mdrfoi. $insert_count records were read.\n";

}  catch(PDOException $e) {  // catch any PDO exceptions first

    $dbh->rollBack();

    $errors = "Rolling back....\n";
 
    $errors .= "PDO Error:  {$e->getMessage()} \n";  

    $errors .= "mdr_report_key is $mdr_report_key \n";

    $errors .= "$report_source_code is $report_source_code \n";

    echo $errors;

    $LogFile->fwrite($errors);


}  catch(Exception $e) {
    
    $errors = "Non_PDO Error message: " .   $e->getMessage() . "\n";

    echo $errors;

    $LogFile->fwrite($errors);
}

// Change format from   MM/DD/YYYY to MySQL format of YYYY-MM-DD    
function create_date($input)
{
   $date_parts = explode("/", $input); // date is in format of MM/DD/YYYY      

   $mysql_date_format = "";

   if (count($date_parts) == 3) {
       
       $mysql_date_format = sprintf("%d-%02d-%02d", $date_parts[2], $date_parts[0], $date_parts[1]);
   } 
   
   return $mysql_date_format;  
}
?>
