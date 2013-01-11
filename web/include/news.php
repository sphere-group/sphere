<?php

class SphereNews {

  function addArticle ($icon, $title, $brief, $text) {
    global $DB;
    global $USER;
    if (strlen($title) > 64)
      return "title must be shorter than 64 characters";
    $DB->query("CREATE TABLE IF NOT EXISTS news (date DATETIME, author VARCHAR(32), icon VARCHAR(64), title VARCHAR(64), brief TEXT, text TEXT)");
    $result = $DB->query("INSERT INTO news VALUES (NOW(), '{$USER->username}', '$icon', '$title', '$brief', '$text')");
    if ($result[0] == -1)
      return $result[1];
    return FALSE;
  }

  function getArticle ($id) {
    global $DB;
    $article = $DB->query("SELECT * FROM news WHERE date='$id'");
    if ($article[0] == -1)
      return FALSE;
    return $article[1];
  }

  function getArticles ($startdate, $enddate, $maxarticles) {
    global $DB;
	 if ($startdate and $enddate)
    	return $DB->query("SELECT * FROM news WHERE date BETWEEN '$startdate' AND '$enddate' ORDER BY date DESC LIMIT $maxarticles");
    else if ($startdate)
      return $DB->query("SELECT * FROM news WHERE date > '$startdate' AND ORDER BY date DESC LIMIT $maxarticles");
    else
      return $DB->query("SELECT * FROM news ORDER BY date DESC LIMIT $maxarticles");
  }

}

?>
