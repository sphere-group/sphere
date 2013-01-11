<?php
require_once("$DOCROOT/passwords.inc.php");

class MySQLConnection {
  var $connection;

  function MySQLConnection () {
    global $MYSQL_HOST;
    global $MYSQL_USER;
    global $MYSQL_DATABASE;
    global $MYSQL_PASSWORD;
    $this->connection = mysql_connect($MYSQL_HOST, $MYSQL_USER, $MYSQL_PASSWORD) or die("could not connect to MySQL server on $MYSQL_HOST");
    mysql_select_db($MYSQL_DATABASE) or die("could not select database $MYSQL_DATABASE");
  }

  function cleanup () {
    mysql_close($this->connection);
  }

  function query ($string) {
    $result = mysql_query($string, $this->connection);
    $error = mysql_error();
    if ($error)
      return array(-1, $error);
    $objects = array();
    $num = 0;
    while ($row = mysql_fetch_object($result)) {
      $objects[] = $row;
      $num++;
    }
    return array($num, $objects);
  }
}

?>
