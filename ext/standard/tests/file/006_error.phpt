--TEST--
Test fileperms(), chmod() functions: error conditions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not on Windows');
}
elseif (get_current_user() == 'root') {
 die( "skip Do not run with root permissions" );
}
?>
--FILE--
<?php
/*
  Prototype: int fileperms ( string $filename )
  Description: Returns the permissions on the file, or FALSE in case of an error

  Prototype: bool chmod ( string $filename, int $mode )
  Description: Attempts to change the mode of the file specified by 
    filename to that given in mode
*/

echo "*** Testing error conditions for fileperms(), chmod() ***\n";

/* With standard files and dirs */
var_dump( chmod("/etc/passwd", 0777) );
printf("%o", fileperms("/etc/passwd") );
echo "\n";
clearstatcache();

var_dump( chmod("/etc", 0777) );
printf("%o", fileperms("/etc") );
echo "\n";
clearstatcache();

/* With non-existing file or dir */
var_dump( chmod("/no/such/file/dir", 0777) );
var_dump( fileperms("/no/such/file/dir") );
echo "\n";

/* With args less than expected */
$fp = fopen(dirname(__FILE__)."/006_error.tmp", "w");
fclose($fp);
var_dump( chmod(dirname(__FILE__)."/006_error.tmp") );
var_dump( chmod("nofile") );
var_dump( chmod() );
var_dump( fileperms() );

/* With args greater than expected */
var_dump( chmod(dirname(__FILE__)."/006_error.tmp", 0755, TRUE) );
var_dump( fileperms(dirname(__FILE__)."/006_error.tmp", 0777) );
var_dump( fileperms("nofile", 0777) );

echo "\n*** Done ***\n";
?>
--CLEAN--
<?php
unlink( dirname(__FILE__)."/006_error.tmp");
?>
--EXPECTF--
*** Testing error conditions for fileperms(), chmod() ***

Warning: chmod(): Operation not permitted in %s on line %d
bool(false)
100644

Warning: chmod(): Operation not permitted in %s on line %d
bool(false)
40755

Warning: chmod(): No such file or directory in %s on line %d
bool(false)

Warning: fileperms(): stat failed for /no/such/file/dir in %s on line %d
bool(false)


Warning: Wrong parameter count for chmod() in %s on line %d
NULL

Warning: Wrong parameter count for chmod() in %s on line %d
NULL

Warning: Wrong parameter count for chmod() in %s on line %d
NULL

Warning: Wrong parameter count for fileperms() in %s on line %d
NULL

Warning: Wrong parameter count for chmod() in %s on line %d
NULL

Warning: Wrong parameter count for fileperms() in %s on line %d
NULL

Warning: Wrong parameter count for fileperms() in %s on line %d
NULL

*** Done ***
