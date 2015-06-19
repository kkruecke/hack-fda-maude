<?hh

include "hidden/MaudeDbConfig.php";

/* Purpose: Updates the medwatch_report.text with the value in narrative_report.text by using results from this query
 * 
 * select m.report_id, n.narrative from medwatch_report m inner join narrative_report n on m.mdr_report_key=n.mdr_report_key;
 * 
 */
try {

    $MaudeDbConfig = MaudeDbConfig::getDbConfig();

    $pdo = new PDO('mysql:host=localhost;dbname=' . $MaudeDbConfig['dbname'], $MaudeDbConfig['dbuser'], $MaudeDbConfig['passwd']);     
 
    $sql_select = "SELECT m.report_id, n.narrative 
                      FROM 
                   medwatch_report m INNER JOIN narrative_report n ON m.mdr_report_key=n.mdr_report_key";
        
    $stmt = $pdo->query($sql_select);
    
    $results = $stmt->fetchAll(PDO::FETCH_ASSOC); 
        
    $sql_update = "UPDATE medwatch_report SET text=:narrative WHERE report_id=:report_id";
    
    $stmt = $pdo->prepare($sql_update);
    
    $pdo->beginTransaction();
    
    foreach($results as $result) {
        
       $rc = $stmt->execute(array(':narrative' => $result['narrative'], ':report_id' => $result['report_id']));
       
       if ($rc === FALSE) {
           
           $x = "UPDATE medwatch_report SET text=" . $result['narative'] . " WHERE report_id=" . $result['report_id'];    
           
           echo "This UPDATE failed:\n" . $x ."\n"; 
       }
    }
    
    $pdo->commit();
    $dbh = null; // closes connection

    
} catch(PDOException $e)  {
    
    $pdo->rollBack();
    
    echo $e->getMessage();
    echo "Exception Thrownin " . $e->getFile() . " at line " . $e->getLine() . "\n";
    echo "Stack Trace as string:" . $e->getTraceAsString() . "\n";
    echo "Stack Trace:" . $e->getTrace() . "\n";
    
    return;
}
echo "Done.\n";
?>
