<?hh
/*
 * Purpose: Takes text and puts it in standard English sentence form. 
 */
function fixText($text)
{
    $str = strtolower($text);
    
    $str = trim($str);
      
    /* 
       Next, capitalize first word of each sentence. Look for dot/period followed by whitespace, followed by a lowercase letter.
     */ 
    $str = preg_replace_callback(
        '/\.\s+([a-z])/',
        function($matches) { return strtoupper($matches[0]); },
        $str
        );
    
     // Capitalize the first character of the text.       
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
?>
