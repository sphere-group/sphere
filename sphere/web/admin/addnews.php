<?php
$DATE = '$Date: 2002/01/03 03:03:00 $';
$AUTHOR = '$Author: jcore $';
$PAGE = 'admin/addnews.php';
$ACCESS = 'reporter';

require_once('../constants.php');
require_once('../include/html.php');
require_once('../include/news.php');

$news = new SphereNews;
if ($articleicon) {
  if (!$articletitle)
    fatalError("you must include an article title");
  if (!$articlebrief)
    fatalError("you must include an article brief");
  if (strlen($articletitle) > 64)
    fatalError("article title length must be less than 65 characters");
  if (strlen($articlebrief) > 1024)
    die("article brief length must be less than 1025 characters");
  $text = NULL;
  if ($HTTP_POST_FILES['articlefile']['tmp_name'] and $HTTP_POST_FILES['articlefile']['tmp_name'] != 'none') {
    $filename = $HTTP_POST_FILES['articlefile']['tmp_name'];
    $filesize = $HTTP_POST_FILES['articlefile']['size'];
    if (!is_uploaded_file($filename))
      fatalError("please upload a file");
    else
      $file = fopen($filename, "r") or die("unable to open temporary file");
      $text = fread($file, $filesize);
  }
  $result = $news->addArticle($articleicon, $articletitle, $articlebrief, $text);
  if ($result)
    fatalError($result);
  $html = new HtmlGenerator("sphere - news article submission");
  $box = new HtmlBox("box", "success");
  $box->puts("your news article has been submitted successfully");
  $html->divide("50%");
  $html->appendBox($box, 0);
  $html->generate();
} else {
  $html = new HtmlGenerator("sphere - add a news article");
  $html->divide("100%");
  $box = new HtmlBox("box", "news article");
  $input = <<<TEXT
<form enctype="multipart/form-data" action="addnews.php" method="post">
  <div>
    <label>icon</label><br />
    <select name="articleicon">

TEXT;
  foreach ($ICONS as $icon_name => $dummy) {
    $input .= "      <option>$icon_name</option>\n";
  }
  $input .= <<<TEXT
    </select><br />
    <label>title</label><br />
    <input type="text" name="articletitle" size="65" maxlength="64" /><br />
    <label>brief (keep it to 1024 characters or less!)</label><br />
    <textarea class="full" name="articlebrief" rows="3"></textarea><br />
    <input type="hidden" name="MAX_FILE_SIZE" value="6144" />
    <label>text</label><br />
    <input type="file" name="articlefile" />
    <hr>
    <input type="submit" />
  </div>
</form>
TEXT;
  $box->append($input);
  $html->appendBox($box, 0);
  $html->generate();
}

?>
