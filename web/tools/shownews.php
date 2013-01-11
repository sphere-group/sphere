<?php
$DATE = '$Date: 2002/01/03 02:36:16 $';
$AUTHOR = '$Author: jcore $';
$PAGE = 'tools/shownews.php';

require('../constants.php');
require('../include/html.php');
require('../include/news.php');

if (!$articleid)
  fatalError("you must provide an article ID");

$html = new HtmlGenerator("sphere - news - article $articleid");
$news = new SphereNews;
$article = $news->getArticle($articleid);
if (!$article)
  fatalError("the article ID that you have specified is invalid");
$article = $article[0];
$html->divide("100%");
$box = new HtmlBox("box", $article->title);
$box->append("{$article->date}<br />by <a href=\"../users.php?username={$article->author}\">{$article->author}</a><br /><br />");
$box->append($article->brief);
$box->puts("<br /><br />");
$box->append($article->text);
$html->appendBox($box, 0);
$html->generate();

?>
