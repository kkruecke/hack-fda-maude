<?hh

abstract class test  {

    protected \SplFileObject $spl_file;
 
    public function __construct($file_name) : void 
    {
       $this->spl_file = new \SplFileObject($file_name);

       $this->spl_file->setFlags(\SplFileObject::READ_AHEAD | \SplFileObject::SKIP_EMPTY);
    }

    public function updateDatabase() : void
    {
      foreach ($this->spl_file as $line_number => $text) {

           echo $text . "\n" . $line_number;
      } 

      $this->spl_file->rewind();
    
      while ($this->spl_file->valid()) {

          $key = $this->spl_file->key(); // 
          $value = $this->spl_file->current();
      
          // ...
      
          $this->spl_file->next();
      }       
     
    }     
}
