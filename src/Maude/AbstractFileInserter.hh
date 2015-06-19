<?hh
namespace Maude;

abstract class AbstractFileInserter  {

    protected \Object $pdo_handle;
    protected \SplFileObject$spl_file;
    protected string $filename;

    protected function getFileName()
    {
       return $this->filename;
    }

    protected function getPDO()
    {
       return $this->pdo_handle;
    }
    
    protected function getspl_file()
    {
        return $this->spl_file;
    } 
  
    public function __construct($file_name, \PDO $handle)
    {
       $this->pdo_handle = $handle;
       $this->filename = $file_name;

       $this->spl_file = new \SplFileObject($file_name);

       $this->spl_file->setFlags(\SplFileObject::READ_AHEAD | \SplFileObject::SKIP_EMPTY);
    }

    abstract protected function setUp();
    
    abstract protected function processLine($text, $line_number);
    
    public function updateDatabase()
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
          
      }
    }     
}
