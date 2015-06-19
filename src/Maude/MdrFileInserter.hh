<?hh
namespace Maude;
/*
 * Purpose: Insert records into foi_device table whose schema is:
 * 
 CREATE TABLE IF NOT EXISTS `mdrfoi` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `mdr_report_key` int(11) unsigned NOT NULL,
  `report_source_code` CHAR(1) NOT NULL,
  `date_received` DATE NOT NULL,
  PRIMARY KEY (`id`), 
  UNIQUE KEY(`mdr_report_key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;
 
 The variables used to insert values into the table are:  
  id 
  mdr_report_key      is in regex return value of $matches[1][0]
  report_source_code  is in regex return value of $matches[1][3]
  date_received       is in regex return value of $matches[1][7]. 
  
Note: date_reported can be empty, which stores 0000-00-00 in the database. So it isn't really of value. 
 */
/*
 *  Change date format from MM/DD/YYYY to MySQL format of YYYY-MM-DD    
 */
function create_date($input)
{
   $date_parts = explode("/", $input); // date is in format of MM/DD/YYYY      

   $mysql_date_format = "";

   if (count($date_parts) == 3) {
       
       $mysql_date_format = sprintf("%d-%02d-%02d", $date_parts[2], $date_parts[0], $date_parts[1]);
   } 
   
   return $mysql_date_format;  
}

class MdrFileInserter extends AbstractFileInserter implements DatabaseUpdateInterface { 

   private \SplFileObject $LogFile;
   private \ImmSet<int> $foidev_mdr_report_keys;
   //--private $max_of_foidev_mdr_report_keys = array();
   private \PDOStatement  $insert_stmt;
   private string $regex;
   private int $insert_count;

  public function __construct($file_name, \PDO $pdo_handle)
  {
       parent::__construct($file_name, $pdo_handle);
                 
       $LogFile = new \SplFileObject("device-log.txt", "w");
       $LogFile->setFlags(\SplFileObject::READ_AHEAD | \SplFileObject::SKIP_EMPTY);
       
        // Set to a default values for now, so bindParam can be called.
       $this->mdr_report_key = (int) 0;        
       $this->report_source_code = "";      
       $this->date_received = "";  
       
       // Regular expression to capture each "|" delimited field. 
       $this->regex = "/([^|]*)\|/";
       
       $this->insert_count = 0;
 }
 
  public function setUp()
  {           
      // SQL statements with named placeholders 
      $select_sql = "SELECT DISTINCT mdr_report_key FROM foi_device ORDER BY mdr_report_key ASC";
        
      if ($select_sql === FALSE) {
            
           throw new \Exception("'SELECT DISTINCT mdr_report_key FROM foi_device ORDER BY mdr_report_key' returned false\n"
                   . "Run DeviceFileInserter.php first\n");
      }

      $select_stmt = $this->getPDO()->query($select_sql);
    
      // bind the parameters in each stateme"SELECT DISTINCT mdr_report_key FROM foi_device ORDER BY mdr_report_key nt
      $this->foidev_mdr_report_keys = $select_stmt->fetchAll(\PDO::FETCH_COLUMN, 0);
      
      sort($this->foidev_mdr_report_keys);

      $this->max_of_foidev_mdr_report_keys = isset($this->foidev_mdr_report_keys) 
                        ? $this->foidev_mdr_report_keys[count($this->foidev_mdr_report_keys) - 1] : 0;
      
      $dbh = $this->getPDO();
           
      $this->insert_stmt = $dbh->prepare("INSERT INTO mdrfoi(mdr_report_key, report_source_code, date_received) values
                       (:mdr_report_key, :report_source_code, :date_received)");
    
      $this->insert_stmt->bindParam(':mdr_report_key', $this->mdr_report_key, \PDO::PARAM_INT);
    
      $this->insert_stmt->bindParam(':report_source_code', $this->report_source_code, \PDO::PARAM_STR);
        
      $this->insert_stmt->bindParam(':date_received', $this->date_received, \PDO::PARAM_STR);
  } 
  
  public function processLine($text, $line_number)
  {
      $matches = array();
      
      $line_count = $line_number + 1;
       
      $hit_count = preg_match_all ($this->regex, $text, $matches);

      if ($hit_count === FALSE) { // debug test. 

           throw new Exception("no hits found on line: $line_count\n");
      }

      if (count($matches[1]) < 8)  {

         echo "matches[1] count is " . count($matches[1]) . " on line $line_count \n";

         $this->LogFile->fwrite("$line_count : match count less than 8 (8th column is data_received). Match count equals " . count($matches[1]) . "\n");
         return;
      }

      $this->mdr_report_key =  (int) $matches[1][0]; 
      
      $this->report_source_code = (string) $matches[1][3]; 
      
      $this->date_received = create_date($matches[1][7]); // date is in format of MM/DD/YYYY        

      if ($this->mdr_report_key > $this->max_of_foidev_mdr_report_keys) { // optimization: ignore if the value exceeds max value in array.

           return;
 
      } else if (array_search($this->mdr_report_key, $this->foidev_mdr_report_keys) === FALSE) {

           return;
      }
              
     // We found a lasik-related report, so save report source and date.

     $event_key =  (int) $matches[1][1];                       

     $date_received  = create_date($matches[1][7]);   // date is in format of MM/DD/YYYY      

     // Make sure the mdrfoi.mdr_report_key was not already inserted.
     
     $exist_already_stmt = $this->getPDO()->query("SELECT mdr_report_key FROM mdrfoi WHERE mdr_report_key={$this->mdr_report_key}");

     $result = $exist_already_stmt->fetch(\PDO::FETCH_COLUMN, 0); //TODO <--- Check this logic!!!!
          
     if ($result !== FALSE) { 

          return; // It already exists, so skip it
     }
           
     // insert it into the data base 
     $bRc = $this->insert_stmt->execute();
     
     if (++$this->insert_count % 10000 == 0) {

          echo "$insert_count number of records written so far out of $line_count lines read ...\n";
     }
      
  } // end method
  
  // Change format from   MM/DD/YYYY to MySQL format of YYYY-MM-DD    
  protected function create_date($input)
  {
     $date_parts = explode("/", $input); // date is in format of MM/DD/YYYY      
  
     $mysql_date_format = "";
  
     if (count($date_parts) == 3) {
         
         $mysql_date_format = sprintf("%d-%02d-%02d", $date_parts[2], $date_parts[0], $date_parts[1]);
     } 
     
     return $mysql_date_format;  
  }

} // end class

