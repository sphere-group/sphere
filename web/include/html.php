<?php

/* our HTML is plagued with tables */

function fatalError ($message) {
  $html = new HtmlGenerator("fatal error");
  $box = new HtmlBox("box", "error");
  $box->append($message);
  $html->divide("50%");
  $html->appendBox($box, 0);
  $html->generate();
  die;
}

class HtmlBox {
  var $type;
  var $title;
  var $text;

  function HtmlBox ($type, $title) {
    $this->type = $type;
    $this->title = $title;
    $this->text = array();
  }

  function puts ($string) {
    $this->text[] = $string;
  }

  function append ($text) {
    $line = strtok($text, "\n");
    while ($line) {
       $this->puts($line);
       $line = strtok("\n");
    }
  }

  function generate () {
    $_return = <<<HTML
              <table class="{$this->type}">
                <tr>
                  <td class="title">
                    {$this->title}
                  </td>
                </tr>
                <tr>
                  <td class="body">

HTML;
    foreach ($this->text as $line) {
      $_return .= "                    $line\n";
    }
    $_return .= <<<HTML
                  </td>
                </tr>
              </table>

HTML;
    return $_return;
  }

}

class HtmlGenerator {
  var $page_title;
  var $menu_refs;
  var $menu_names;
  var $body;
  var $divisions;
  var $widths;
  var $sidebar;

  function HtmlGenerator ($title) {
    global $SITEROOT;
    $this->page_title = $title;
    $this->menu_refs = array();
    $this->menu_names = array();
    $this->setMenuLink(0, "news", "$SITEROOT/");
    $this->setMenuLink(1, "about", "$SITEROOT/about.php");
    $this->setMenuLink(2, "download", "http://sourceforge.net/project/showfiles.php?group_id=1551");
    $this->setMenuLink(3, "games", "$SITEROOT/games/");
    $this->setMenuLink(4, "forum", "http://aegisknight.org/forum/list.php?f=4/");
    $this->setMenuLink(5, "links", "$SITEROOT/links.php");
    $this->setMenuLink(6, "users", "$SITEROOT/users.php");
    $this->setMenuLink(7, "reserved", "");
    $this->setMenuLink(8, "reserved", "");
    $this->setMenuLink(9, "sourceforge", "http://sourceforge.net/projects/sphere");
    $this->body = "";
    $this->divisions = array();
    $this->sidebar = array();
  }

  function put ($string) {
    $this->body .= "$string";
  }

  function puts ($string) {
    $this->body .= "$string\n";
  }

  function putline ($string) {
    $this->body .= "$string<br />\n";
  }

  /* for now only positions 0-9 will be rendered */
  function setMenuLink ($position, $name, $location) {
    $this->menu_names[$position] = $name;
    $this->menu_refs[$position] = $location;
  }

  function divide ($width) {
    $this->divisions[] = array();
    $this->widths[] = $width;
  }

  function appendBox ($box, $location) {
    $this->divisions[$location][] = $box;
  }

  function createMenu ($greeting) {
    global $SITEROOT;
    $_menu = <<<MENU
        <table class="menu" cellspacing="0" cellpadding="0">
          <tr>
            <td class="menuleft">

MENU;
    for ($lcv = 0; $lcv < 5; $lcv++)
      $_menu .= "               <a href=\"{$this->menu_refs[$lcv]}\">{$this->menu_names[$lcv]}</a>\n";
    $_menu .= <<<MENU
            </td>
            <td class="menucenter">
              <img src="$SITEROOT/images/title.png" alt="sphere.sourceforge.net logo" /><br />
              $greeting
            </td>
            <td class="menuright">

MENU;
    for ($lcv = 5; $lcv < 10; $lcv++)
      $_menu .= "              <a href=\"{$this->menu_refs[$lcv]}\">{$this->menu_names[$lcv]}</a>\n";
    $_menu .= <<<MENU
            </td>
          </tr>
        </table>

MENU;
    return $_menu;
  }

  /* DATE and AUTHOR should be set to the respective CVS keyword expansions before calling tail() */
  function createTail () {
    global $SITEROOT;
    global $DATE;
    global $AUTHOR;
    global $PAGE;
    $moddate = substr($DATE, 7, 10);
    $modtime = substr($DATE, 18, 8);
    $modauthor = substr($AUTHOR, 9, -2);
    $_tail = <<<TAIL
        <div class="references">
          <a href="http://validator.w3.org/check/referer"><img src="http://www.w3.org/Icons/valid-xhtml10" alt="Valid XHTML 1.0!" width="88" height="31" /></a>
          <a href="http://jigsaw.w3.org/css-validator/check/referer"><img src="http://jigsaw.w3.org/css-validator/images/vcss" alt="Valid CSS!" width="88" height="31" /></a>
          <a href="http://sourceforge.net/"><img src="http://sourceforge.net/sflogo.php?group_id=873&amp;type=1" alt="sourceforge logo" width="88" height="31" /></a>
        </div>
        <p class="footer">
          <i>Last Modified on $moddate at $modtime by $modauthor</i><br />
          <a href="$SITEROOT/tools/showsource.php?page=$PAGE">show the source!</a>
        </p>

TAIL;
    return $_tail;
  }

  function generate () {
    global $spherestylesheet;
    global $USER;
    global $SITEROOT;
    
    if (!$spherestylesheet)
      $spherestylesheet = "graphics";
    if ($USER)
      $greeting = "Welcome {$USER->username} (<a href=\"$SITEROOT/tools/userlogout.php\">logout</a>)";
    else
      $greeting = "You are not logged in (<a href=\"$SITEROOT/login.php\">login</a> or <a href=\"$SITEROOT/register.php\">register</a>)";
    /* header */
    echo <<<HTML
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "DTD/xhtml1-strict.dtd">

<html>
<head>
  <title>{$this->page_title}</title>
  <link rel="stylesheet" type="text/css" title="$spherestylesheet" href="$SITEROOT/css/$spherestylesheet.css" />
</head>
<body>
  <table class="main" cellpadding="0" cellspacing="0">
    <tr>                      <!-- menu -->
      <td class="menu">

HTML;
    echo $this->createMenu($greeting);
    echo <<<HTML
      </td>
    </tr>                  <!-- end of menu -->

HTML;
      echo <<<HTML
    <tr>                      <!-- body -->
      <td class="body">

HTML;
    for ($lcv = 0; $lcv < count($this->divisions); $lcv++) {
      $div = $this->divisions[$lcv];
      if (count($this->divisions) == 1)
        echo <<<HTML
        <table class="division" width="{$this->widths[$lcv]}" style="margin: auto">

HTML;
      else
        echo <<<HTML
        <table class="division" width="{$this->widths[$lcv]}">

HTML;
      foreach ($div as $box) {
        echo <<<HTML
          <tr>
            <td>

HTML;
        echo $box->generate();
        echo <<<HTML
            </td>
          </tr>

HTML;
      }
      echo <<<HTML
        </table>

HTML;
    }
    echo <<<HTML
      </td>
    </tr>                <!-- end of divisions -->

HTML;
    echo <<<HTML
    <tr>                      <!-- tail -->
      <td class="tail">

HTML;
    echo $this->createTail();
    echo <<<HTML
      </td>
    </tr>                  <!-- end of tail -->
  </table>
</body>
</html>
HTML;
  }

}

?>
