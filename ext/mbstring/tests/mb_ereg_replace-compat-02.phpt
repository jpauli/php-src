--TEST--
mb_ereg_replace() compat test 2
--SKIPIF--
extension_loaded('mbstring') or die('skip');
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/002.phpt) */
  $a="abc123";
  echo mb_ereg_replace("123","",$a);
?>
--EXPECT--
abc
