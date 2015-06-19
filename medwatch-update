#!/usr/bin/php
<?php
use Maude\Registry as Registry;
        
require_once("class_loader.php"); 

boot_strap();

try {

   $insert_count = 0;

   // The select query immediately below is the same as the second query that is commented out following it.
   // This is because mdr_report_key is a unique in each of the tables and because the foitext and mdrfoi text have
   // exactly the same mdr_report_keys (actually, their count most recently was one different, not sure why).
   $select_query = "SELECT foitext.mdr_report_key, foitext.text_report, mdrfoi.date_received, mdrfoi.report_source_code 
                     FROM foitext 
                              INNER JOIN
                           mdrfoi on foitext.mdr_report_key=mdrfoi.mdr_report_key
                              LEFT OUTER JOIN
                           medwatch_report 
                               ON foitext.mdr_report_key=medwatch_report.mdr_report_key
                           WHERE medwatch_report.mdr_report_key IS NULL ORDER BY foitext.mdr_report_key"; 
/*
   $select_query = "SELECT inner_join.* from (SELECT foitext.mdr_report_key, foitext.text_report, mdrfoi.date_received, mdrfoi.report_source_code 
            FROM foitext 
                   INNER JOIN
                 mdrfoi on foitext.mdr_report_key=mdrfoi.mdr_report_key
                  ) as inner_join
                 LEFT OUTER JOIN
                      medwatch_report 
                  ON inner_join.mdr_report_key=medwatch_report.mdr_report_key WHERE medwatch_report.mdr_report_key IS NULL ORDER BY inner_join.mdr_report_key";
*/

   $pdo = new \PDO(

   // SQL statement with named placeholders 
   $insert_medwatch_report_stmt = $pdo->prepare("INSERT INTO medwatch_report(mdr_report_key, text_report, date_received, report_source_code) values
                                (:mdr_report_key, :text_report, :date_received, :report_source_code )");

   // bind the parameters in each statement
   $insert_medwatch_report_stmt->bindParam(':mdr_report_key', $mdr_report_key, \PDO::PARAM_INT);
        
   $insert_medwatch_report_stmt->bindParam(':text_report', $text_report, \PDO::PARAM_STR);

   $insert_medwatch_report_stmt->bindParam(':date_received', $date_received, \PDO::PARAM_STR);

   $insert_medwatch_report_stmt->bindParam(':report_source_code', $report_source_code, \PDO::PARAM_STR);
   
   $insert_count = 0;

   foreach ($pdo->query($select_query) as $row) {

       $mdr_report_key = $row['mdr_report_key'];

       $text_report =  $row['text_report'];

       $date_received =  $row['date_received'];

       $report_source_code =  $row['report_source_code'];

       $insert_medwatch_report_stmt->execute();
       ++$insert_count;
   }
    
   
} catch (Exception $e) {

   $errors = "\nException Thrown in " . $e->getFile() . " at line " . $e->getLine() . "\n";
      
   $errors .= "Stack Trace as string:\n" . $e->getTraceAsString() . "\n";
          
   $errors .= "Error message is: " .   $e->getMessage() . "\n";
      
   echo $errors;

} // end try
