--TEST--
Check type hint compatibility in overrides with array hints.
--FILE--
<?php
Class C { function f(array $a) {} }

echo "No hint, should be array.\n";
Class D extends C { function f($a) {} }
?>
==DONE==
--EXPECTF--
Fatal error: Declaration of D::f() must be compatible with that of C::f() in %s on line 5