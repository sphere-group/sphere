<?php
$DATE = '$Date: 2002/01/03 02:36:16 $';
$AUTHOR = '$Author: jcore $';
$PAGE = 'tools/useradd.php';

require_once('../constants.php');
require_once('../include/html.php');

$html = new HtmlGenerator("sphere - useradd");
$user = new SphereUser($username);

function failure ($reasons) {
  global $html;
  $html->divide("100%");
  $box = new HtmlBox("box", "registration has failed for the following reasons");
  $box->puts("<ul>");
  foreach ($reasons as $reason)
    $box->puts("  <li>$reason</li>");
  $box->puts("</ul>");
  $box->puts("Please go <a href=\"../register.php\">back</a> and try again.");
  $html->appendBox($box, 0);
  finish();
  die;
}

function finish () {
  global $html;
  global $user;
  $html->generate();
  $user->cleanup();
}

if ($photo == "http://")
  $photo = NULL;
if ($website == "http://")
  $website = NULL;

if ($user->exists()) 
  failure(array("username $username is already in use"));
$user->setPassword($password, $verifypass);
$user->setEmail($email);
$user->setName($firstname, $lastname);
$user->setBirthdate($year, $month, $day);
$user->setPhoto($photo);
$user->setWebsite($website);
$user->setNotification($notify);
$errors = $user->errors();
if (count($errors) > 0)
  failure($errors);
else {
  $result = $user->commit();
  if ($result)
    failure(array($result));
  else {
    $html->divide("50%");
    $box = new HtmlBox("box", "success");
    $success = <<<TEXT
<p>Congratulations!  You are now a registered member of sphere.sourceforge.net.</p>
<p>You may proceed to <a href="../login.php">login</a>.</p>
TEXT;
    $box->append($success);
    $html->appendBox($box, 0);
    finish();
  }
}

?>
