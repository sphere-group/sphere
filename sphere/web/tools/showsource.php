<?php
$AUTHOR = '$Author: jcore $';
$DATE = '$Date: 2002/01/03 02:36:16 $';
$PAGE = 'tools/showsource.php';

require_once('../constants.php');
require_once('../include/html.php');

$html = new HtmlGenerator("sphere - source of $page");
$html->divide("100%");
if (!$page) {
  $box = new HtmlBox("box", "error");
  $box->puts("Error: you must specify a page to view the source of");
  $html->appendBox($box, 0);
} else if (preg_match("/password/", $page)) {
  $box = new HtmlBox("box", "nice try");
  $box->puts("Sorry, but that's one page that we won't show the source to!");
  $html->appendbox($box, 0);
} else {
  $infile = fopen("$DOCROOT/$page", "r");
  if (!$infile) {
    $box = new HtmlBox("box", "error");
    $html->puts("Error: unable to open $file for reading");
    $html->appendBox($box, 0);
  } else {
    $box = new HtmlBox("code", "source of $page");
    while (!feof($infile)) {
      $line = chop(htmlentities(fgets($infile, 4096)));
      $new_line = "";
      $counter = 0;
      while ($line{$counter} == " ") {
        $new_line .= "&nbsp;";
        $counter++;
      }
      $line = $new_line . trim($line);
      $box->puts("$line<br />");
    }
    $html->appendBox($box, 0);
  }
}
$html->generate();

?>
