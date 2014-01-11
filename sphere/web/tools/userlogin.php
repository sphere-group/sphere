<?php
$DATE = '$Date: 2002/01/03 02:36:16 $';
$AUTHOR = '$Author: jcore $';
$PAGE = 'tools/userlogin.php';

require_once('../constants.php');

function failure ($reasons) {
  include_once('../include/html.php');
  $html = new HtmlGenerator("sphere - user login");
  $html->divide("50%");
  $box = new HtmlBox("box", "login has failed for the following reasons");
  $box->puts("<ul>");
  foreach ($reasons as $reason) {
    $box->puts("  <li>$reason</li>");
  }
  $box->puts("</ul>");
  $box->puts("Please go <a href=\"../login.php\">back</a> and try again.");
  $html->appendBox($box, 0);
  $html->generate();
  die;
}

if ($username == NULL)
  failure(array("missing username"));
if ($password == NULL)
  failure(array("missing password"));
$user = new SphereUser($username);
if (!$user->validate($password))
  failure(array("bad username or password"));
/* set login cookies for 1 day */
$time = time() + 86400;
setcookie("sphereusername", $user->username, $time, "/");
setcookie("spherepassword", $user->password, $time, "/");
header("Code: 302");
header("Message: Found");
header("Location: $SITEROOT/");

?>
