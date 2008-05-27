--TEST--
Test ctype_digit() function : usage variations - octal and hexadecimal values
--SKIPIF--
<?php
if( ini_get("unicode.semantics") == "1") {
    die('skip do not run when unicode on');
}
?>
--FILE--
<?php
/* Prototype  : bool ctype_digit(mixed $c)
 * Description: Checks for numeric character(s) 
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass octal and hexadecimal values as $c argument to ctype_digit() to test behaviour
 */

echo "*** Testing ctype_digit() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

$octal_values = array(061,  062,  063,  064);
$hex_values = array  (0x31, 0x32, 0x33, 0x34);

echo "\n-- Octal Values --\n";
$iterator = 1;
foreach($octal_values as $c) {
	echo "-- Iteration $iterator --\n";
	var_dump(ctype_digit($c));
	$iterator++;
}

echo "\n-- Hexadecimal Values --\n";
$iterator = 1;
foreach($hex_values as $c) {
	echo "-- Iteration $iterator --\n";
	var_dump(ctype_digit($c));
	$iterator++;
}

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_digit() : usage variations ***

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_digit_variation4.php on line 13

-- Octal Values --
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(true)
-- Iteration 3 --
bool(true)
-- Iteration 4 --
bool(true)

-- Hexadecimal Values --
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(true)
-- Iteration 3 --
bool(true)
-- Iteration 4 --
bool(true)

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_digit_variation4.php on line 34
===DONE===
