--TEST--
Test token_get_all() function : $options flag 
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
echo "*** Testing token_get_all() : \$options flag ***\n";

$source = '<?php echo "Hello World"; ?>';
var_dump( token_get_all($source, TOKENIZER_TOKEN_NAMES) );
var_dump( token_get_all($source, TOKENIZER_TOKEN_NAMES | TOKENIZER_SHOW_VALUE) );
echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() : $options flag ***
array(6) {
  [0]=>
  array(2) {
    [0]=>
    string(10) "T_OPEN_TAG"
    [1]=>
    int(1)
  }
  [1]=>
  array(2) {
    [0]=>
    string(6) "T_ECHO"
    [1]=>
    int(1)
  }
  [2]=>
  array(2) {
    [0]=>
    string(12) "T_WHITESPACE"
    [1]=>
    int(1)
  }
  [3]=>
  array(2) {
    [0]=>
    string(26) "T_CONSTANT_ENCAPSED_STRING"
    [1]=>
    int(1)
  }
  [4]=>
  array(2) {
    [0]=>
    string(12) "T_WHITESPACE"
    [1]=>
    int(1)
  }
  [5]=>
  array(2) {
    [0]=>
    string(11) "T_CLOSE_TAG"
    [1]=>
    int(1)
  }
}
array(7) {
  [0]=>
  array(3) {
    [0]=>
    string(10) "T_OPEN_TAG"
    [1]=>
    string(6) "<?php "
    [2]=>
    int(1)
  }
  [1]=>
  array(3) {
    [0]=>
    string(6) "T_ECHO"
    [1]=>
    string(4) "echo"
    [2]=>
    int(1)
  }
  [2]=>
  array(3) {
    [0]=>
    string(12) "T_WHITESPACE"
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [3]=>
  array(3) {
    [0]=>
    string(26) "T_CONSTANT_ENCAPSED_STRING"
    [1]=>
    string(13) ""Hello World""
    [2]=>
    int(1)
  }
  [4]=>
  string(1) ";"
  [5]=>
  array(3) {
    [0]=>
    string(12) "T_WHITESPACE"
    [1]=>
    string(1) " "
    [2]=>
    int(1)
  }
  [6]=>
  array(3) {
    [0]=>
    string(11) "T_CLOSE_TAG"
    [1]=>
    string(2) "?>"
    [2]=>
    int(1)
  }
}
Done