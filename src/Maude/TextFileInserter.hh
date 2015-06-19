<?hh
namespace Maude;
/*
 * Purpose: To save patient text report for lasik reports
 * Note: The mdr_report_key, the first field in foitextXXXX.txt, is not necessarily unique.  Not sure why.
 */ 

class TextFileInserter extends AbstractFileInserter implements DatabaseUpdateInterface {
   
   private $FileIter;
   private $file_name;
   private $LogFile;
   private $mdr_report_key;
   private $text_report;
   private $insert_stmt;
   private $mdr_report_keys = array();
   private $max_mdr_report_key_in_foitext; 

   private $prior_mdr_report_key;

   private $insert_count = 0;
   private $txt_line_count = 0;
   private $hit_count = 0;
   private $regxex;
       
   public function __construct($file_name, \PDO $db_handle)
   {
       parent::__construct($file_name, $db_handle);
           
       $this->LogFile = new \SplFileObject("text-log.txt", "w");
       $this->LogFile->setFlags(\SplFileObject::READ_AHEAD | \SplFileObject::SKIP_EMPTY);
         
       $this->text_report = (string) "";
       $this->mdr_report_key = (int) 0;

       // Regular expression to capture each "|" delimited field. 
       $this->regex = "/^([^|]*)\|([^|]*)\|([^|]*)\|([^|]*)\|([^|]*)\|(.*)$/";  
         
       // SQL statements with named placeholders 
       $this->insert_stmt = $db_handle->prepare("INSERT INTO foitext(mdr_report_key, text_report)
                                   values (:mdr_report_key, :text_report)"); 
     
       // bind the parameters in each statement
       $this->insert_stmt->bindParam(':mdr_report_key', $this->mdr_report_key, \PDO::PARAM_INT);
  
       $this->insert_stmt->bindParam(':text_report',  $this->text_report,  \PDO::PARAM_STR);

       $this->insert_count = 0;
       $this->txt_line_count = 0;
   }

   protected function setUp()
   {
    // TODO: Add the same code that currently is commented out with "TODO..." in MdrFileInserter.php.
 
       // Get array of mdr_report_keys from mdrfoi
       $select_query = "SELECT DISTINCT mdr_report_key from mdrfoi ORDER BY mdr_report_key";

       $select_stmt = $this->getPDO()->query($select_query);

       if ($select_stmt === FALSE) {

            throw new Exception("SELECT DISTINCT mdr_report_key from mdrfoi ORDER BY mdr_report_key failed. Has MdrFileInserter.php been run?\n");
            return;
       } 
      
       // array of mdr_report_keys from mdrfoi table 
       $this->mdrfoi_mdr_report_keys = $select_stmt->fetchAll(\PDO::FETCH_COLUMN, 0);
      
       sort($this->mdrfoi_mdr_report_keys);
      
       $this->prior_mdr_report_key = $this->mdrfoi_mdr_report_keys[0]; // use the first in the array as the "prior" 

       /*
        * Make make sure the prospective mdr_report_keys in the foitext files ( and its text) are not already in the foitext table. Since we know that the 
        * mdr_report_key is unique in foitext. Therefore simply query it for the max mdr_report_key. Any more recent mdr_report_keys
        * will be greater than prior, existing mdr report keys already in foitext.
        */ 

       $max_select_stmt = $this->getPDO()->query("SELECT MAX(mdr_report_key) from foitext"); 
       $this->max_mdr_report_key_in_foitext = (int) $max_select_stmt->fetch(\PDO::FETCH_COLUMN, 0); 
   }

 
   public function processLine($text, $line_number)
   {
          /*
           * FOI TEXT files contains the following 6 fields, delimited by pipe (|), one record per line:
           * 
              1. MDR Report Key -- all digits.
              2. MDR Text Key   -- all digits. The primary key of the report's text; ie, the row identifier of the foitextXXX.txt file.
              3. Text Type Code (D=B5, E=H3, N=H10 from mdr_text table)
              4. Patient Sequence Number (from mdr_text table) -- digit(s).
              5. Date Report (from mdr_text table)  -- it seems this can be empty, but in the current year file only.
              6. Text (B5, or H3 or H10 from mdr_text table) 
      
           *  We only save the MDR Report Key and the Text
           */
                  
          $matches = array();
            
          $this->line_count = $line_number + 1; 
               
           //  $arr = explode("|", $text); failed because there was one line with | in the narrative text. So a regular expression is preferred
      
          $hit_count = preg_match($this->regex, $text, $matches);
          
          if (count($matches) != 7) {
      
               $error_msg = "Hit count is $hit_count rather than 7 on line number $this->line_count of file $filename\n";
               echo $error_msg;
      
               $LogFile->fwrite($error_message);     
      
               return;
          }
                
          /* 
            We only keep text_type_code == 'D' where the sequence number is empty or 1. NOTE: This does guarantee that the mdr_report_key will only occur
            once. There can be instances where the mdr_report_key occurs more than once when the text_type is 'D' and the sequence number is 1.
            So the foitext files don't seem to be consistent.
          */
      
          $bool = ($matches[3] == 'D' && ($matches[4] == 1 || $matches[4] == ""));  
          
          if ($bool == 0) { // Per comment above, skip this line. 
              
               return;
          }
      
          $mdr_report_key = (int) $matches[1];
          
          // If the mdr_report_key is not in the set mdr_report_keys from the mdrfoi table, or that mdr_report_key has not changed,
          // then skip it and continue.
          if ( (array_search($mdr_report_key, $this->mdrfoi_mdr_report_keys) === FALSE) || ($mdr_report_key ==  $this->prior_mdr_report_key) ) {
              
              return;
          }
          
          $this->prior_mdr_report_key = $this->mdr_report_key = $mdr_report_key;

          //  Make sure that the mdr_report_key is not foitext already.
          if ($this->mdr_report_key <= $this->max_mdr_report_key_in_foitext) {
              
              return;
          }
                
          /*
             Description of fields in foitextXXX.txt file:
             1. MDR Report Key
             2. MDR Text Key 
             3. Text Type Code (D=B5, E=H3, N=H10 from mdr_text table) 
             4. Patient Sequence Number (from mdr_text table) 
             5. Date Report (from mdr_text table) 
             6. Text (B5, or H3 or H10 from mdr_text table) 
          */
             
          $text_key = $matches[2];
          $text_type_code = $matches[3];
          $patient_seq_no = $matches[4];
          
          $date_parts = explode("/", $matches[5]); // date is in format of MM/DD/YYYY      
        
          // Change format from   MM/DD/YYYY to  YYYY-MM-DD    
          if (count($date_parts) == 3) {
              
              $event_date = sprintf("%d-%02d-%02d", $date_parts[2], $date_parts[0], $date_parts[1]);
              
          } else {
              
              $event_date = "";
          }     
      
          $text_report = trim($matches[6]);
      
          $this->text_report = $this->fixText($text_report);
      
          if (strlen($this->text_report) == 0) {
              
             throw new Exception("Foi Text file. Line foune where report has no text. Line is $this->line_count\n");
          }
          /*
           * Debug code
           */
          echo "Inserting key {$this->mdr_report_key}. With text starting with " . substr($this->text_report, 0, 30) , "\n";
        
          $bRc = $this->insert_stmt->execute();
      
          $this->insert_count++; 
      
          if ($this->line_count % 10000 == 0) {
        
               $inserted = number_format($insert_count); 
        
               $lines_read = number_format($this->line_count);
         
               echo "$inserted number of records inserted out of $lines_read lines read ...\n";
          }
      
   }

   protected function fixText($text)
   {
      $str = strtolower($text);
      
      $str = trim($str);
        
      // Next, capitalize first word of each sentence. 
      $str = preg_replace_callback(
          '/\.\s+([a-z])/',
          function($matches) { return strtoupper($matches[0]); },
          $str
          );
      
       // Also capitalize the first character of the text.       
       $str = preg_replace_callback(
          '/^([a-z])/',
          function($matches) { return strtoupper($matches[0]); },
          $str
          );
  
        //  Capitalize the pronoun ' i '
        $str = preg_replace_callback(
          '/(\s[i]\s)/',
          function($matches) { return strtoupper($matches[0]); },
          $str
          );
      
       return $str; 
    }
}
?>
