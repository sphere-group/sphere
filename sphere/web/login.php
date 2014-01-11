<?php
$DATE = '$Date: 2002/01/03 02:36:16 $';
$AUTHOR = '$Author: jcore $';
$PAGE = 'login.php';

require_once('constants.php');
require_once('include/html.php');

$html = new HtmlGenerator("sphere - user login");

if ($USER) {
  $html->divide("50%");
  $box = new HtmlBox("box", "error");
  $box->puts("you are already logged in");
  $html->appendBox($box, 0);
} else {
  $html->divide("75%");
  $box = new HtmlBox("box", "login");
  $input = <<<TEXT
<form action="$SITEROOT/tools/userlogin.php" method="post">
  <div>
    <label>username</label><br />
    <input type="text" name="username" size="33" maxlength="32" /><br />
    <label>password</label><br />
    <input type="password" name="password" size="13" maxlength="12" /><br />
    <hr />
    <input type="submit" />
  </div>
</form>
TEXT;
  $box->append($input);
  $html->appendBox($box, 0);
  $html->divide("25%");
  $box = new HtmlBox("box", "notes");
  $notes = <<<NOTES
Remember that your username and password are case-sensitive.<br /><br />
Your password will be transmitted in plaintext.<br /><br />
You must have cookies enabled for this site.
We will only set one cookie for your username and one for your password (view our policy <a href="policy.php">here</a>).
NOTES;
  $box->append($notes);
  $html->appendBox($box, 1);
}

$html->generate();

?>
