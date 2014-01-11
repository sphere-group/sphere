<?php
require_once('../constants.php');

/* set cookie one day in the past */
$time = time() - 86400;

if ($sphereusername)
  setcookie("sphereusername", $sphereusername, $time, "/");
if ($spherepassword)
  setcookie("spherepassword", $spherepassword, $time, "/");

header("Code: 302");
header("Message: Found");
header("Location: $SITEROOT/");
?>
