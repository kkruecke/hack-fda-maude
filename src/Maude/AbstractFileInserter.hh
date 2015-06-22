<?hh
namespace Maude;
use Maude\SplFileObjectExt;

abstract class AbstractFileInserter  {

    protected \PDO $pdo_handle;
    protected SplFileObjectExt $spl_file;
    protected string $filename;
  
    public function __construct($file_name, \PDO $handle)
    {
      
       $this->pdo_handle = $handle;
       $this->filename = $file_name;

       $this->spl_file = new SplFileObjectExt($file_name);
    }

    protected function getFileName() : string
    {
       return $this->filename;
    }

    protected function getPDO() : \PDO
    {
       return $this->pdo_handle;
    }
    
    protected function getsplext_file() : SplFileObjectExt
    {
        return $this->spl_file;
    } 

    abstract protected function setUp() : void;
    
    abstract protected function processLine($text, $line_number) : void;
    
    public function updateDatabase() : void
    {
       try {

          $this->setUp();

          $this->pdo_handle->beginTransaction();
      
          foreach ($this->spl_file as $line_number => $text) {

               $this->processLine($text, $line_number);
          } 
       
          $this->pdo_handle->commit();

        }  catch(\PDOException $e) {  // catch any PDO exceptions 
      
          $this->pdo_handle->rollBack();
      
          $errors = "Rolling back....\n";
     
          $errors .= "PDO Error: " . $e->getMessage() . "\n";  
      
          echo $errors;
         
          throw $e;
          
      } catch (\Exception $e) {

          $errors = "Error: " . $e->getMessage() . "\n";  
      
          echo $errors;
         
          throw $e;
      }
      
    }     
}
