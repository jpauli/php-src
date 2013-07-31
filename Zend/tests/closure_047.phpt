--TEST--
Closure 047: Rebinding : isBindable()
--FILE--
<?php

class A
{
	public static function staticClosure()
	{
	    return function () { };
	}
	
	public function nonStaticClosure()
	{
	    return function () { };
	}
}
$a = new A;

$staticClosure = A::staticClosure();
$nonStaticClosure = $a->nonStaticClosure();
$globalScopeClosure = function() { };

var_dump($staticClosure->isBindable());
var_dump($nonStaticClosure->isBindable());
var_dump($globalScopeClosure->isBindable());

echo "Done.\n";

--EXPECTF--
bool(false)
bool(true)
bool(true)
Done.
