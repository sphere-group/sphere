<?php
$AUTHOR = '$Author: jcore $';
$DATE = '$Date: 2002/01/05 06:33:31 $';
$PAGE = 'users.php';

require_once('constants.php');
require_once('include/html.php');

if ($username) {
  $html = new HtmlGenerator("sphere - info about $username");
  $user = new SphereUser($username);
  if (!$user->exists()) {
    $html->divide("50%");
    $box = new HtmlBox("box", "error");
    $box->puts("User $username does not exist");
    $html->appendBox($box, 0);
    $html->generate();
  } else {
    $html->divide("100%");
    $userinfo = $user->output();
    $box = new HtmlBox("box", $userinfo[0]);
    $box->append($userinfo[1]);
    $html->appendBox($box, 0);
    $html->generate();
  }
} else {
  $users = getUsers();
  $html = new HtmlGenerator("sphere - users");
  $html->divide("100%");
  foreach ($users as $userobj) {
    $user = new SphereUser($userobj->username);
    $userinfo = $user->output();
    $box = new HtmlBox("box", $userinfo[0]);
    $box->append($userinfo[1]);
    $html->appendBox($box, 0);
  }
  $html->generate();
}

?>
