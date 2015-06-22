<?hh
namespace Maude;
/*
 * Implements the key() method of KeyedIterator<Tk, Tv>, so SplFileObject can be used in foreach loops
 * The constructor automatically sets the flags: \SplFileObject::READ_AHEAD | \SplFileObject::SKIP_EMPTY
 */ 
class SplFileObjectExt extends \SplFileObject implements KeyedIterator<int, string>  {

    private int $position;

    public function __construct(string $filename, string $mode = 'r')
    {
       parent::__construct($filename, $mode);

       parent::setFlags(\SplFileObject::READ_AHEAD | \SplFileObject::SKIP_EMPTY);

       $this->position = 1;
    }

    public function rewind() : void 
    {
        parent::rewind();
        $this->position = 1;
    }

    public function key() : int 
    {
        return $this->position;
    }

    public function next() : void
    {
        parent::next();

        if (parent::valid()) {

            ++$this->position; 
        }
    }
}
