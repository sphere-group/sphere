<?php
$DATE = '$Date: 2002/01/03 02:36:16 $';
$AUTHOR = '$Author: jcore $';
$PAGE = 'links.php';

require_once('constants.php');
require_once('include/html.php');
  
$html = new HtmlGenerator("sphere - links");
$html->divide("50%");
$links = array();
$links['sphere-related sites'] = <<<TEXT
<a href="http://mastercain.com/flik">Flik's Site</a><br />
<a href="http://mastercain.com/wip">WIP's Site</a><br />
<a href="http://mastercain.com/drosen">DRosen's Site</a><br />
<a href="http://www8.ewebcity.com/fenix159/">Fenix's Domain</a><br />
<a href="http://www.mastercain.com/overlord/">Overlord's Site</a><br />
<a href="http://www.angelfire.com/biz6/xNASH999">Nash's Site</a><br />
<a href="http://rinoa.yi.org/~sugoi/main/info.html">May-chan's Page</a>
TEXT;
$links['independent game scene'] = <<<TEXT
<a href="http://verge-rpg.com/">VERGE</a><br />
<a href="http://verge-rpg.com/~tsb">Ika</a><br />
<a href="http://mastercain.com">Mastercain.com</a><br />
<a href="http://madmonkey.net">MadMonkey</a><br />
<a href="http://zophar.net">Zophar's Domain</a>
TEXT;
$links['helpful resourcese'] = <<<TEXT
<a href="http://developer.netscape.com/docs/manuals/communicator/jsref/index.htm">Javascript Reference</a><br />
<a href="http://mathworld.wolfram.com">World of Mathematics</a><br />
<a href="http://everything2.com">everything<span style="font-size: 60%">2</span></a>
TEXT;
$links['other sites'] = <<<TEXT
<a href="http://zeromus.org">Zeromus's Page</a><br />
<a href="http://falthorn.com">Falthorn's Page</a>
TEXT;

foreach ($links as $key => $value) {
  $box = new HtmlBox("box", $key);
  $box->append($value);
  $html->appendBox($box, 0);
}
$html->generate();

?>
