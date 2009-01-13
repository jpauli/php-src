--TEST--
Test gzencode() function : variation
--SKIPIF--
<?php 

if( substr(PHP_OS, 0, 3) == "WIN" ) {
   die("skip.. Do not run on Windows");
}

if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded"; 
}	 
?> 
--FILE--
<?php
/* Prototype  : string gzencode  ( string $data  [, int $level  [, int $encoding_mode  ]] )
 * Description: Gzip-compress a string 
 * Source code: ext/zlib/zlib.c
 * Alias to functions: 
 */

include(dirname(__FILE__) . '/data.inc');

echo "*** Testing gzencode() : variation ***\n";

echo "\n-- Testing multiple compression --\n";
$output = gzencode(b"$data");
var_dump(bin2hex(gzencode($output)));

?>
===Done===
--EXPECT--
*** Testing gzencode() : variation ***

-- Testing multiple compression --
unicode(3658) "1f8b0800000000000003010e07f1f81f8b08000000000000036d574d6fe4c80dbdeb57d4ad2f3dfe01eb83e1ec22980e309b4562c067b64449159754dafab0b6e7d7e73d96da1e4c72184c4b2ab2c8f7c847fa25baabba98dc1a8b2b7c38bb324b713ee37f757f56cdc5c7f5b17b9d152f923b157c5ae335e0b75fedd0e2d781c6b98ea3a6ee05affe1dfc3a6527f8f09c52dcb38ba38bb5249934d6ecfe1e53a9ab76ff4c342cf2a64ed2028349fc9a8b139755685352acb82b9fbb67f8bade5cdcb698e1fcec94b7ceba3cb897e806cfc8114350dd1ebbdfa35b62d2478b0056d23ed809b9b95d696d91ce2aa97c911e3fa539c43f84c887554a4d125c9e63ff96711cc08c0866263cb37a0bbe2122ae8f6baecb2284abfb4ddf916db8354cddeef37c1afe5fa02fc7afb3db34f5b3acbdf2eb905490d8f38d7468d253a323d5ebb903760d7944d3b2024e834a99ddce77669bdd823cfbb8e899d4ad4c799677452e6029e80023a03b2374005590641f7d3877df2ad09f3c0e82a54d6a5644fd63049a37ed4bc362016fd9f51264f1e5c630727421ae930b7ed416e93e47b7c71a400390361ffbecb7561bb98f69b5da289e91becc27f08b3b724cb8704f9144d366431d0cb870c56b205deaa2e17636063761a911039fb7e4bf9f06c4f0aecd2ec80e8b41831ca7515e31286166458ea3ef71f2ce7cde2ae269c96d60525724a9c9170b713ed5750758f3cd2a361fc8b288fc92358ce884692e8ea0fe59bd969a0da2eed5831b715749eaae7178f3ebd30fb88c92105f367cce2c882955dc6bf8eca0d5d57540b3092894743ba0fd5b2dad021836191f1afc0bba14dde1642cb0b1aa6879c38907dcefa0720082b801bec61417469219175267dfa047df35b0bd1332001c28cdfafd3bcabe91e74368cdd8d8478e494c190e7ee90c67f2bde288e68ab6b15e883c995be4f8feb6c6dda4278e4f38578ddbdc7be36788daf0c3cb1d1819c73822f7000a0d1813fa94153b572315e51343b536bc64977dff163cebfd8418773261f524017e251fccc60ae29a5770ae097594d52e9c1229d87ce967a36401c46b69945afb249d101c9d420ffa9a123e232c20e76467d5d169202a2dd4c582949e013e745df7958d4b0cc4fd4377a737cd4feea7974070000f314d423e0634cb9a618fdf5dc64fd422181fd59c9230c9f6f9d18dc8fc23e9cccbc7188733b04aa57de83ebea0be3633cff5fa1ff83269be7f44f5a8d84550cc703255fd345dd402034d0b3e11a73ec6e3d4a77f4f685b614329f1b3132ae7af33d02e1e55e291fa6574b758d1f0200e7423dbc852211818043a7c9ce80aa9d59fce0401959f5ea2cf71fde90824f8c9192dbe9d329db143794675ddcf257dd7755273b67340414e3ccad12e3f661f8aad9cf9957dc1275d10a51d3934fa81e68dc6768fb8ee23e373936c8e13feab8b0f50d227f7af76f561fb0950f3d099bbc316c3892a42fb36806d8660e800fa4f43fd4b962d2097d71933a54b77ff948677848eb17bb3a88b621682cfb3bbb49cf42fed6b3944124ad8358ca688aa44dd5f2144c7c9ab16f25b9aca9654ef357ec9ad55c40d324d6cc3d9e3920b863c231d31a95d937fb5520f9c816c79b7dcecc593fb9593cc05a51ebb1eeddd5b49eb437769738d0f64adc579d372b8b7f7c0208487ee3915ebf5766e148ebd77cf4e01f3ec285047011e55838968b6494d517fe29224777b24dd3ddf933101695b102e87db805eef291b74dcfd91628fb2a53f93dbd2968ef2e598746c9204f89fba1f0246fc671610a0591806e46a1346f77c40d910a47c5e20ffb23f003c04b648327a4ed98032c1965bd35bb0044f5344248f56fdb99aa61d6451d68e33489a83bffbe6573541b2da5f64681ea12090f778b2075374778810f73965fa3626a9d41f4df2f83f7c34658cec921b5a9bde49dd5007ec882b02adc514f81aa85898b5cc98e1b137733c0a8789b7f5648d2d231b80bf74978f25d61ce08a8abd11801fd8f995e066676307192ff7641f1cc6e0dee68565b8b22ac3889cd067bf732754a6b270af1044c6a8776811a4f6d8bd0477a9f516064201b920b92d7cd4dc7eee13e6b3eb3528a82f9abb3f388ebe6a8f871393461b73816ec54c99d604174bc5a6801de13908f86aea6a7d0fea107d682bcf1ec348b83872e6b8a316ecd02eb8f8dc86a609bf59a2dd03f1dfa4079436d55e24617be1a2854d008b2b2b1705e2078a7f3946318df1c24f6bf70d4b456eca286ec2b585b28262cc048a098c3e2d5f325a92bb36f691afdc14c822da1b116c9c1c07bb362eb0a04b78834c812134230ebf2044ac2e3c0e3ad00f848dc5010f3bf917ec2fc700b7bf26dacea8440620e04f90f4d97d6dd77cfde8a05c7d3930f1e5811fb8ec5c70964dcc8187ec90e32fdd6b64eec7586413b7d55bed65c4cce39a9b6c15e70e9da94e53fc904e6286f01f5b5562c94211befbc23507e01b2a3865e2f45b5d7b591f290087a5605b82495b4e393f31aa5b37211ec40241a746d903c5eebf117a4d3ddb0d00007b64cbc70e070000"
===Done===