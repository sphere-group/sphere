<?php
/* all php files are responsible for including constants.php before any other file */

/* make sure the following are correct before checking into CVS */
$SITEROOT = '';
$DOCROOT = '/home/groups/s/sp/sphere/htdocs';
$MYSQL_HOST = 'mysql';
$MYSQL_USER = 'sphere';
$MYSQL_DATABASE = 'sphere';

/* keep icon filenames to less than 64 characters in length */
$ICONS = array(
'article' => 'article.png',
'announcement' => 'announcement.png',
'game' => 'game.png',
'outside' => 'outside.png',
'project' => 'project.png'
);

require_once("$DOCROOT/include/mysql.php");
$DB = new MySQLConnection;

require_once("$DOCROOT/include/user.php");

function clear_login_cookies () {
  global $SITEROOT;
  header("Code: 302");
  header("Message: Found");
  header("Location: $SITEROOT/tools/userlogout.php");
}

/* make sure that cookies haven't been tampered with */
if ($sphereusername or $spherepassword) {
  if ($sphereusername and !$spherepassword)
    clear_login_cookies();
  if ($spherepassword and !$sphereusername)
    clear_login_cookies();
  $USER = new SphereUser($sphereusername);
  if (!$USER->exists())
    clear_login_cookies();
  if (!$USER->verify($spherepassword))
    clear_login_cookies();
} else
  $USER = NULL;

/* make sure the user has access to this area */
if ($ACCESS) {
  if (!$USER or !$USER->access($ACCESS))
    die("You do not have access to this area.");
}

?>
