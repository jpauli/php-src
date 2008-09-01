--TEST--
finfo_file(): Testing file names
--FILE--
<?php

$fp = finfo_open();
var_dump(finfo_file($fp, "\0"));
var_dump(finfo_file($fp, ''));
var_dump(finfo_file($fp, NULL));
var_dump(finfo_file($fp, '.'));
var_dump(finfo_file($fp, '&'));

?>
--EXPECTF--
bool(false)

Warning: finfo_file(): Empty filename or path in %s on line %d
bool(false)

Warning: finfo_file(): Empty filename or path in %s on line %d
bool(false)
string(9) "directory"

Warning: finfo_file(&): failed to open stream: No such file or directory in %s on line %d
bool(false)
