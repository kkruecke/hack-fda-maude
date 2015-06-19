<?hh
namespace Maude;
/*
  IMPORTANT: According to FDA Maude database instructions for the FOIDEV.zip files:

 "... you may need to put in an extra character at the end of the first record prior to importing the file,
  otherwise the last column of data may be lost."

 This code assumes the file is in sorted order and that it contains only unique rows.
 */

class DeviceFileInserter extends AbstractFileInserter implements DatabaseUpdateInterface {

   private  $device_product_code;
   private  $device_key;
   //--private  $prior_mdr_report_keys = array();     
   private  $max_mdr_report_keys;
   private  $mdr_report_key;          // value in current line
   private  $regex;
   private  $hit_count;
   private  $insert_count;
   private  $insert_stmt;
   private  $LogFile;
   private  $first_insert;

   public function __construct($file_name, \PDO $pdo_handle)
   {
      
      parent::__construct($file_name, $pdo_handle);

      $this->first_insert = false;

      // Create error log
      $this->LogFile = new \SplFileObject("error-log.txt", "w");
      
      $this->LogFile->setFlags(\SplFileObject::READ_AHEAD | \SplFileObject::SKIP_EMPTY);

      $this->max_mdr_report_key = (int) 0;        
                                    
      $this->device_product_code = (string) "";
                                    
      $this->device_key = (int) 0;
      
      $this->hit_count = 0;
      
      $this->insert_count = 0; 

      $this->regex = "/([^|]*)\|/";

      // In case the table is empty and has not maxium mdr_report_key was use a default of -1
      $this->max_mdr_report_key = -1; 

      // SQL statement with named placeholders 
      $this->insert_stmt = $this->getPDO()->prepare("INSERT INTO foi_device(mdr_report_key, device_product_code) values
	      (:mdr_report_key, :device_product_code )");

      // bind the parameters in each statement
      $this->insert_stmt->bindParam(':mdr_report_key', $this->mdr_report_key, \PDO::PARAM_INT);
        
      $this->insert_stmt->bindParam(':device_product_code', $this->device_product_code, \PDO::PARAM_STR);
   }
  
    protected function setUp()
    { 
     /*
        $select_stmt = $this->getPDO()->query("SELECT mdr_report_key FROM foi_device ORDER BY mdr_report_key ASC");
        $this->prior_mdr_report_keys = $select_stmt->fetchAll(\PDO::FETCH_COLUMN, 0);
         
        $this->prior_mdr_report_keys = array_unique($this->prior_mdr_report_keys, SORT_NUMERIC); 
        return;
      */   
        // New code
        $select_stmt = $this->getPDO()->query("SELECT max(mdr_report_key) as max_mdr_report_key FROM foi_device");
        $row = $select_stmt->fetch();
        $this->max_mdr_report_keys = $row['max_mdr_report_key']; 
    }

    protected function processLine($text, $line_number)
    {
       $matches = array();

       if (!($line_number %10000)) { 
           echo "processing line $line_number\n"; 
       }

       try {
      
        $lineNumber = $line_number + 1;
         
        $hit_count = preg_match_all($this->regex, $text, $matches);
      
        if ($hit_count === FALSE) { // This should never happen
              
              throw new Exception("no hits, exiting... line number is $this->lineNumber");
        }
      
        if (!isset($matches[1]) ) {
      
            echo "matches[1] is not set on line $line_number \n";
      
            $this->LogFile->fwrite("$line_number : $matches[1] not set\n");
            return;
          
         } else if (count($matches[1]) < 44) {
      
            echo "matches[1] count is " . count($matches[1]) . " on line $line_number \n";
      
            $this->LogFile->fwrite("$line_number : match count not equal to 44. match count equals " . count($matches[1]) . "\n");
            return;
         }
      
         $this->mdr_report_key = (int) $matches[1][0];
         
         $this->device_product_code = strtoupper($matches[1][25]);
      
         
         // We only want the line where the device product codes are "HNO" or "LZS"
         if ((strpos($this->device_product_code, "HNO") !== FALSE) || (strpos($this->device_product_code, "LZS") !== FALSE)) {
                
      
         } else { // Skip the line. It is not a laser-related report
      
             return;
         } 
         // It is lasik-related
  
         // New code: Add if new. Remember input file has been sorted.
         if ($this->mdr_report_key <= max_mdr_report_key) {

                return;    
         } 

         /* prior code
         if (array_search($this->mdr_report_key, $this->prior_mdr_report_keys)) { // if already in table, ignore.

                return; 
         }
         */     

        /* Save first new mdr_report_key in registry. This assumes that the input file is in sorted order with respect to the mdr_report_key. */
        if ($this->first_insert) {

            Registry::register('newest_mdr_report_key', $this->mdr_report_key);  
            $this->first_insert = false;
         } 

        // insert it into the data base 
        $bRc = $this->insert_stmt->execute();
        
        // Add it to the array of mdr_report_keys. Note: We assume the input file was in sorted order, and only more recent data is being inserted.
        // Therefore we do not resort the array.
        // TODO: Make this an associative array that maps mdr_report_key to Device Code?
        $this->prior_mdr_report_keys[] = $this->mdr_report_key;
        
        if (++$this->insert_count % 1000 == 0) {
  
             echo "{$this->insert_count} number of records written so far out of $lineNumber lines read ...\n";
        }
         
        }  catch(PDOException $e) {  // catch any PDO exceptions first
          
              $this->getPDO()->rollBack();
          
              $errors = "Rolling back....\n";
         
              $errors .= "PDO Error: " . $e->getMessage() . "\n";  
          
              $errors .= "mdr_report_key is $mdr_report_key\n";
                  
              echo $errors;
             
              $this->LogFile->fwrite($errors);
    
              throw $e;
        }          
    } // end method

}  // end class
?>