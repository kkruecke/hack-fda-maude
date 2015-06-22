<?hh // partial
namespace Maude;

/*
 * Only one instance of Registry is ever create, as a local static within the static method self::getInstance()
 */

class Registry implements \ArrayAccess {
    
    private static $container = array();

    public static function init() 
    {
         if (count(self::$container) == 0) {	  
	      
            @self::$container = parse_ini_file("maude.ini", true); 
         }
    }
    /*
     * This method is called only by $this->registry($value) and $this->register($key, $value). It ensure that we only create
     * one instance of Registry, which is stored in the local static variable $the_registry.
     */
    protected static function getInstance()
    {
      static $the_registry;

      if (!isset($the_registry)) {

          $the_registry = new Registry();
      }  
      
      return $the_registry;  
    }

    public static function dump()
    {
       self::init();   
       var_dump(self::$container);
       echo "\n";
    }

    public function __construct()
    {
        self::init();
    }

    public static function register($offset, $value)
    {
       $registry = self::getInstance();

       $registry->offsetSet($offset, $value);
    }

    public static function registry($offset)
    {
       $registry = self::getInstance();

       return $registry->offsetGet($offset);
    }

    public function offsetSet($offset, $value) 
    {
        if (isset(self::$container[$offset])) {
            
             /*
              *  This a preventative measure. If the element at $offset is currently a subarray, we can't simply overwrite it.
              */
            unset(self::$container[$offset]);
            
            self::$container[$offset] = $value;

        } else {

            // $value is a scalar   
            self::$container[$offset] = $value;
        }
    }

    public function offsetExists($offset) 
    {
        return isset(self::$container[$offset]);
    }

    public function offsetUnset($offset) 
    {

        unset(self::$container[$offset]);
    }

    public function offsetGet($offset) 
    {

        return isset(self::$container[$offset]) ? self::$container[$offset] : null;
    }
}
