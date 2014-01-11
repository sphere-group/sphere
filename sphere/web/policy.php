<?php
$DATE = '$Date: 2002/01/03 02:36:16 $';
$AUTHOR = '$Author: jcore $';
$PAGE = 'policy.php';

require_once('constants.php');
require_once('include/html.php');

$html = new HtmlGenerator("sphere - policy");
$html->divide("100%");
$box = new HtmlBox("box", "sphere.sourceforge.net policy");
$policy = <<<TEXT
The creators of sphere.sourceforge.net work hard to maintain the privacy and security of it's users.  We also want to encourage a productive environment that the RPG community can take part in.  The following steps are taken to ensure this.
<ul>
  <li>email addresses
    <ol>
      <li>Giving us your email address is completely optional.  We suggest that you provide it so that you can get in touch with other RPG creators but the entire site is open to you even if you don't.</li>
      <li>We will never give your email address to anyone.  The only time your email address is used is to send you important information (if you opted for that during registration) and to display it in your user information.</li>
      <li>We will not spam you.  Although we feel that allowing us to send you notices about new sphere developments (new games, new tools, etc) is beneficial, it's completely optional.</li>
      <li>We will not display your email address in an easily harvestable form.  Spammers have taken to running bots that search pages for email addresses by looking for common patterns (mostly something@something).  We make it much harder on them by displaying your address in a human-readable form that is much more difficult for a program to parse.</li>
    </ol>
  </li>
  <li>passwords
    <ol>
      <li>All passwords are encrypted.  We will not store or transmit passwords in plaintext.  The only time that your password is vulnerable is when transmitting it to the server during login (and we're working on securing that too).</li>
    </ol>
  </li>
  <li>openness
    <ol>
      <li>You can view the source code to every page on the site by directing your browser to http://sphere.sourceforge.net/tools/showsource.php?page=pathandpagename.php.  We also provide a link to show the source at the bottom of every page.</li>
      <li>We will not censor anything on any page of this site.</li>
    </ol>
  </li>
</ul>
TEXT;
$box->append($policy);
$html->appendBox($box, 0);
$html->generate();

?>
