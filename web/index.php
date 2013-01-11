<?php
$DATE = '$Date: 2002/01/12 07:35:19 $';
$AUTHOR = '$Author: jcore $';
$PAGE = 'index.php';

require_once('constants.php');
require_once('include/html.php');
require_once('include/news.php');

$html = new HtmlGenerator("sphere - news");

/* get articles from the last 10 days, or a maximum of 10 articles */
$news = new SphereNews;
/* $startdate = date("Y-m-d H:i:s", time() - (60 * 60 * 24 * 30));
$enddate = date("Y-m-d H:i:s", time());
$articles = $news->getArticles($startdate, $enddate, 10); */
$articles = $news->getArticles(NULL, NULL, 10);
$html->divide("80%");
$news_side_box = new HtmlBox("box", "latest news");
for ($lcv = 0; $lcv < $articles[0]; $lcv++) {
  $article = $articles[1][$lcv];
  $datetime = $article->date;
  $datetime = str_replace("-", "", $datetime);
  $datetime = str_replace(" ", "", $datetime);
  $datetime = str_replace(":", "", $datetime);
  $title = <<<TITLE
<a name="$lcv" /><img src="$SITEROOT/images/icons/{$ICONS[$article->icon]}" alt="{$article->icon}" />{$article->date}: <b>{$article->title}</b>
TITLE;
  $box = new HtmlBox("news", $title);
  $box->puts($article->brief);
  if ($article->text)
    $box->puts("<br /><div class=\"center\"><a href=\"tools/shownews.php?articleid=$datetime\">view the full article</a>.</div>");
  $box->puts("<div class=\"sig\">- <a href=\"users.php?username={$article->author}\">{$article->author}</a></div>");
  $html->appendBox($box, 0);
  $news_side_box->puts("<a href=\"#$lcv\">{$article->title}</a><br />");
}

$html->divide("20%");

if (!$USER) {
  $box = new HtmlBox("box", "login");
  $text = <<<TEXT
<form action="$SITEROOT/tools/userlogin.php" method="post" id="login">
  <div>
    <label>username: </label>
    <input class="full" type="text" name="username" maxlength="32" /><br />
    <label>password: </label>
    <input class="full" type="password" name="password" maxlength="12" /><br />
    <div class="sig"><a href="Javascript:document.getElementById('login').submit()">login</a></div>
  </div>
</form>

TEXT;
  $box->append($text);
  $html->appendBox($box, 1);
}

$html->appendBox($news_side_box, 1);

$html->generate();
?>
