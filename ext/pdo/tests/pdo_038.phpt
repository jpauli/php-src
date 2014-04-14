--TEST--
PDO debug_info handler
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) die ("skip Need PDO_SQlite support");
?>
--FILE--
<?php
$db = new PDO('sqlite::memory:');
var_dump($db);
?>
--EXPECTF--
object(PDO)#1 (21) {
  ["persistent"]=>
  bool(false)
  ["driver_name"]=>
  string(6) "sqlite"
  ["dsn_string"]=>
  string(8) ":memory:"
  ["autocommit"]=>
  NULL
  ["prefetch"]=>
  NULL
  ["timeout"]=>
  NULL
  ["errmode"]=>
  NULL
  ["server_version"]=>
  string(%d) "%s"
  ["client_version"]=>
  string(%d) "%s"
  ["server_info"]=>
  NULL
  ["connection_status"]=>
  NULL
  ["case"]=>
  NULL
  ["cursor_name"]=>
  NULL
  ["cursor"]=>
  NULL
  ["oracle_nulls"]=>
  NULL
  ["statement_class"]=>
  NULL
  ["fetch_table_names"]=>
  NULL
  ["fetch_catalog_names"]=>
  NULL
  ["stringify_fetches"]=>
  NULL
  ["max_column_length"]=>
  NULL
  ["default_fetch_mode"]=>
  NULL
}